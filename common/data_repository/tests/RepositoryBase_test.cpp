// Copyright 2025 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <data_structures/data_structures.h>
#include <gtest/gtest.h>
#include <juce_data_structures/juce_data_structures.h>

#include "../repository_base/RepositoryMultiBase.h"
#include "../repository_base/RepositorySingleBase.h"

class TestRepositoryItem final : public RepositoryItemBase {
 public:
  TestRepositoryItem() : RepositoryItemBase({}) {}
  TestRepositoryItem(juce::Uuid id) : RepositoryItemBase(id) {}
  TestRepositoryItem(juce::Uuid id, int testMember)
      : RepositoryItemBase(id), testMember_(testMember) {}

  bool operator==(const TestRepositoryItem& other) const {
    return testMember_ == other.testMember_ && id_ == other.id_;
  }

  static TestRepositoryItem fromTree(const juce::ValueTree tree) {
    jassert(tree.hasProperty(kId));
    jassert(tree.hasProperty(kTestMember));
    return TestRepositoryItem{juce::Uuid{tree[kId]}, tree[kTestMember]};
  }

  virtual juce::ValueTree toValueTree() const override {
    return juce::ValueTree{
        "test", {{kId, getId().toString()}, {kTestMember, testMember_}}};
  }

  inline static const juce::Identifier kTestMember{"test_member"};
  inline static const juce::Identifier kTreeType{"test_repository_item"};

  int testMember_ = 0;
};

using TestRepositoryMulti = RepositoryMultiBase<TestRepositoryItem>;
using TestRepositorySingle = RepositorySingleBase<TestRepositoryItem>;
const juce::Identifier treeType = "test";

TEST(test_base_repository, add_element_multi) {
  TestRepositoryMulti repositoryInstance(juce::ValueTree{treeType});
  repositoryInstance.add({});
  ASSERT_EQ(repositoryInstance.getItemCount(), 1);
}

TEST(test_base_repository, add_duplicate_id) {
  TestRepositoryMulti repositoryInstance(juce::ValueTree{treeType});
  TestRepositoryItem testItem;
  repositoryInstance.add(testItem);
  ASSERT_FALSE(repositoryInstance.add(testItem));
  ASSERT_EQ(repositoryInstance.getItemCount(), 1);
}

TEST(test_base_repository, add_two_different) {
  TestRepositoryMulti repositoryInstance(juce::ValueTree{treeType});
  repositoryInstance.add({});
  repositoryInstance.add({});
  ASSERT_EQ(repositoryInstance.getItemCount(), 2);
}

TEST(test_base_repository, remove_element) {
  TestRepositoryMulti repositoryInstance(juce::ValueTree{treeType});
  TestRepositoryItem testItem;
  repositoryInstance.add(testItem);
  repositoryInstance.remove(testItem);
  ASSERT_EQ(repositoryInstance.getItemCount(), 0);
}

TEST(test_base_repository, remove_nonexistant) {
  TestRepositoryMulti repositoryInstance(juce::ValueTree{treeType});
  TestRepositoryItem testItem;
  repositoryInstance.add(testItem);
  TestRepositoryItem testItem2;
  ASSERT_FALSE(repositoryInstance.remove(testItem2));
  ASSERT_EQ(repositoryInstance.getItemCount(), 1);
}

TEST(test_base_repository, get_element) {
  TestRepositoryMulti repositoryInstance(juce::ValueTree{treeType});
  TestRepositoryItem testItem;
  repositoryInstance.add(testItem);
  std::optional<TestRepositoryItem> fetchedItem =
      repositoryInstance.get(testItem.getId());
  ASSERT_TRUE(fetchedItem.has_value());
  ASSERT_EQ(testItem.getId(), fetchedItem->getId());
}

TEST(test_base_repository, get_nonexistant) {
  TestRepositoryMulti repositoryInstance(juce::ValueTree{treeType});
  TestRepositoryItem testItem;
  repositoryInstance.add(testItem);
  ASSERT_FALSE(repositoryInstance.get({}).has_value());
}

TEST(test_base_repository, update_element) {
  TestRepositoryMulti repositoryInstance(juce::ValueTree{treeType});
  TestRepositoryItem testItem({}, 3);
  repositoryInstance.add(testItem);
  TestRepositoryItem initialState =
      repositoryInstance.get(testItem.getId()).value();
  ASSERT_EQ(testItem, initialState);

  testItem.testMember_ = 4;
  ASSERT_TRUE(repositoryInstance.update(testItem));

  TestRepositoryItem updatedState =
      repositoryInstance.get(testItem.getId()).value();
  ASSERT_NE(updatedState, initialState);
  ASSERT_EQ(updatedState, testItem);
}

TEST(test_base_repository, get_or_add_get) {
  TestRepositoryMulti repositoryInstance(juce::ValueTree{treeType});
  TestRepositoryItem testItem({}, 3);
  repositoryInstance.add(testItem);

  TestRepositoryItem foundOrNewElement =
      repositoryInstance.getOrAdd(testItem.getId());
  ASSERT_NE(foundOrNewElement.testMember_, 0);
  ASSERT_EQ(testItem, foundOrNewElement);
}

TEST(test_base_repository, get_or_add_add) {
  TestRepositoryMulti repositoryInstance(juce::ValueTree{treeType});
  TestRepositoryItem testItem({}, 3);
  repositoryInstance.add(testItem);

  juce::Uuid id;
  TestRepositoryItem foundOrNewElement = repositoryInstance.getOrAdd(id);
  ASSERT_EQ(foundOrNewElement.testMember_, 0);
  ASSERT_NE(testItem, foundOrNewElement);
  ASSERT_EQ(foundOrNewElement.getId(), id);
}

TEST(test_base_repository, get_all) {
  TestRepositoryMulti repositoryInstance(juce::ValueTree{treeType});
  TestRepositoryItem testItem({}, 3);
  TestRepositoryItem testItem2({}, 4);
  repositoryInstance.add(testItem);
  repositoryInstance.add(testItem2);

  juce::OwnedArray<TestRepositoryItem> array;
  repositoryInstance.getAll(array);
  ASSERT_EQ(array.size(), 2);
  ASSERT_NE(*array.getUnchecked(0), *array.getUnchecked(1));
}

TEST(test_base_repository, single_get) {
  TestRepositorySingle repositoryInstance(juce::ValueTree{treeType});
  TestRepositoryItem defaultItem = repositoryInstance.get();

  ASSERT_FALSE(defaultItem.getId().isNull());
}

TEST(test_base_repository, single_update) {
  TestRepositorySingle repositoryInstance(juce::ValueTree{treeType});
  TestRepositoryItem defaultItem = repositoryInstance.get();
  TestRepositoryItem testItem({}, 3);
  ASSERT_NE(defaultItem, testItem);
  repositoryInstance.update(testItem);
  TestRepositoryItem updatedItem = repositoryInstance.get();
  ASSERT_EQ(testItem, updatedItem);
  ASSERT_NE(defaultItem, updatedItem);
}