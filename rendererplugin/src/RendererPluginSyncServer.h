/*
 * Copyright 2025 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <algorithm>
#include <mutex>
#include <vector>

#include "data_repository/implementation/AudioElementRepository.h"
#include "data_repository/implementation/AudioElementSpatialLayoutRepository.h"
#include "data_repository/repository_base/RepositoryBase.h"
#include "data_structures/src/AudioElementSpatialLayout.h"
#include "data_structures/src/RepositoryItem.h"
#include "substream_rdr/substream_rdr_utils/Speakers.h"

class AudioElementPluginConnection;

class SyncServer {
 public:
  virtual void repositoryUpdated(
      AudioElementPluginConnection* updatedAudioElementPlugin) = 0;
  virtual void connectionLost(AudioElementPluginConnection* connection) = 0;
};

class AudioElementPluginUpdateListener {
 public:
  virtual void updateAudioElementPluginInformation(
      AudioElementSpatialLayout& audioElementSpatialLayout) = 0;
  virtual void removeAudioElementPlugin(
      AudioElementSpatialLayout& audioElementSpatialLayout) = 0;
};

/* ================
AudioElementPlugin Connection class for maintaining information about
connections to Audio Element Plugins
==================*/
class AudioElementPluginConnection : public juce::InterprocessConnection {
 private:
  AudioElementSpatialLayoutRepository sharedRepository_;
  juce::CriticalSection repositoryLock_;  // Use lock to prevent read/write
                                          // conflicts on the repository
  SyncServer* managingServer_;
  bool initialized_;

 public:
  AudioElementPluginConnection(SyncServer* managingServerInstance)
      : sharedRepository_(),
        managingServer_(managingServerInstance),
        initialized_(false) {}

  ~AudioElementPluginConnection() override { disconnect(30000); }

  juce::Uuid getId() const {
    const juce::ScopedLock lock(repositoryLock_);
    if (initialized_) {
      return sharedRepository_.get().getId();
    } else {
      return juce::Uuid("");
    }
  }

  juce::Uuid getAudioElementId() const {
    const juce::ScopedLock lock(repositoryLock_);
    if (initialized_) {
      return sharedRepository_.get().getAudioElementId();
    } else {
      return juce::Uuid("");
    }
  }

  juce::String getName() const {
    const juce::ScopedLock lock(repositoryLock_);

    if (initialized_) {
      return sharedRepository_.get().getName();
    } else {
      return "";
    }
  }

  void sendRepository(juce::MemoryBlock& block) { sendMessage(block); }

  void connectionMade() override {}

  void connectionLost() override { managingServer_->connectionLost(this); }

  void messageReceived(const juce::MemoryBlock& message) override {
    juce::MemoryInputStream stream(message, false);
    juce::ValueTree repository = juce::ValueTree::readFromStream(stream);

    // Lock before we write to ensure no conflicts
    const juce::ScopedLock lock(repositoryLock_);
    sharedRepository_.setStateTree(repository);
    initialized_ = true;

    // Finally, notify the managing server
    managingServer_->repositoryUpdated(this);
  }
};

/* ================
Renderer Plugin Server class for distributing information about the audio
elements to AudioElementPlugins and registering AudioElementPlugin instances

Note that this needs to all be handled in a header file due the
juce::InterprocessConnectionServer implementation not fixing an incomplete
type error during compilation.
==================*/
class RendererPluginSyncServer : public juce::InterprocessConnectionServer,
                                 juce::ValueTree::Listener,
                                 SyncServer {
 private:
  AudioElementRepository*
      outgoingRepository_;  // The audio element repository to be sent to all
                            // Audio Element Plugins

  std::vector<AudioElementPluginConnection*>
      connections_;  // All currently registered AudioElementPlugins
  juce::CriticalSection
      repositoryLock_;  // Use lock to prevent writing from multiple threads
  AudioElementPluginUpdateListener* listener_;
  int connectionPort;

  // Variables for retrying the connections and handling premature deletion
  bool closing;
  std::thread connectionThread;
  std::mutex connectionMutex;
  std::condition_variable connectionCondition;

 public:
  RendererPluginSyncServer(AudioElementRepository* toShare, int port,
                           AudioElementPluginUpdateListener* listener)
      : outgoingRepository_(toShare),
        listener_(listener),
        connectionPort(port),
        closing(false) {
    outgoingRepository_->registerListener(this);

    connectionThread = std::thread([&]() {
      std::unique_lock<std::mutex> lock(connectionMutex);
      while (true) {
        if (closing) {
          return;
        }
        // Connect to the socket
        bool res = beginWaitingForSocket(connectionPort);
        if (!res) {
          // If the connection fails, wait for a short amount of time and try
          // again We want to retry fairly rapidly since we don't expect there
          // to be multiple instances, and if we don't retry quickly things
          // start to look weird on the AudioElementPlugins. Use a condition
          // variable to wake up if we need to be deleted to avoid locking
          // everything up on delete
          connectionCondition.wait_for(lock, std::chrono::seconds(10));
        } else {
          break;
        }
      }
    });
  }

  ~RendererPluginSyncServer() override {
    for (auto connection : connections_) {
      connection->disconnect();
      delete connection;
    }
    connections_.clear();
    connectionMutex.lock();
    closing = true;
    stop();
    connectionCondition.notify_all();
    connectionMutex.unlock();
    connectionThread.join();
  }

  // This callback is called each time a new client connects
  juce::InterprocessConnection* createConnectionObject() override {
    AudioElementPluginConnection* connection =
        new AudioElementPluginConnection(this);
    connections_.push_back(connection);
    return connection;
  }

  // This callback is called each time a connection is disconnected
  // and is used to remove the connection from the list of tracked connections
  void connectionLost(AudioElementPluginConnection* connection) override {
    AudioElementSpatialLayout audioElementSpatialLayoutInfo =
        AudioElementSpatialLayout(connection->getId(), connection->getName(),
                                  connection->getAudioElementId(), 0,
                                  Speakers::kMono);
    listener_->removeAudioElementPlugin(audioElementSpatialLayoutInfo);
    connections_.erase(
        std::remove(connections_.begin(), connections_.end(), connection),
        connections_.end());
    delete connection;
  }

  void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                const juce::Identifier& property) override {
    juce::ignoreUnused(treeWhosePropertyHasChanged);
    juce::ignoreUnused(property);
    updateClients();
  }

  void valueTreeChildAdded(juce::ValueTree& parentTree,
                           juce::ValueTree& childWhichHasBeenAdded) override {
    juce::ignoreUnused(parentTree);
    juce::ignoreUnused(childWhichHasBeenAdded);
    updateClients();
  }

  void valueTreeChildRemoved(juce::ValueTree& parentTree,
                             juce::ValueTree& childWhichHasBeenRemoved,
                             int indexFromWhichChildWasRemoved) override {
    juce::ignoreUnused(parentTree);
    juce::ignoreUnused(childWhichHasBeenRemoved);
    juce::ignoreUnused(indexFromWhichChildWasRemoved);
    updateClients();
  }

  void updateClients() {
    // Prevent update clients from being called from
    // multiple threads simultaneuously
    juce::ScopedLock lock(repositoryLock_);

    juce::MemoryBlock block;
    juce::MemoryOutputStream stream(block, false);
    outgoingRepository_->writeToStream(stream);
    for (auto connection : connections_) {
      connection->sendRepository(block);
    }
  }

  void repositoryUpdated(AudioElementPluginConnection* updatedPanner) override {
    juce::ignoreUnused(updatedPanner);
    // First, update the clients (thread safe, since this is a callback
    // thread)
    updateClients();

    // Need to callback here to something, probably the renderer plugin, to
    // indicate a repository has been updated
    AudioElementSpatialLayout audioElementSpatialLayoutInfo =
        AudioElementSpatialLayout(
            updatedPanner->getId(), updatedPanner->getName(),
            updatedPanner->getAudioElementId(), 0, Speakers::kMono);
    listener_->updateAudioElementPluginInformation(
        audioElementSpatialLayoutInfo);
  }
};
