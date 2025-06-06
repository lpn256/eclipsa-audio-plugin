#!/bin/bash -eu
#
# Copyright 2025 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#!/bin/bash

# To Use:
# - This script checks for the existence of wraptool helper script
#    - If the wraptool is present, it will use it to sign the plugins and perform notarization
#    - If the wraptool is not present, it will use ad-hoc code signing instead
# - If you intend to perform official code signing and notarization:
#   - This script assumes the existence of a keychain profile for notarization (e.g., "MyNotaryProfile").
#   - To create one, first create an app-specific password (see https://support.apple.com/en-us/102654).
#   - Then run (replace placeholders):
#       xcrun notarytool store-credentials "<Your Keychain Profile Name>" --apple-id "<Your Apple ID>" --team-id "<Your Team ID>"
#   - You will also need to uncomment and modify the official signing/notarization lines indicated with "NOTE:" 
#     replacing the placeholder signing identities with your own.
#
# IMPORTANT: The commented out sections in this script are not disabled functionality - they are template 
# code that needs to be customized with your specific signing identities before use. These sections will
# be needed when building official signed installers.
#

# Plugin Build Config (e.g., 'RelWithDebInfo' for useful core dumps, can be changed to 'Release' when software is more stable).
BUILD_CONFIG="Release"

# Define paths to the plugins
RENDERER_PLUGIN_BINARY="../build/rendererplugin/RendererPlugin_artefacts/$BUILD_CONFIG/AAX/Eclipsa Audio Renderer.aaxplugin/Contents/MacOS/Eclipsa Audio Renderer"
AUDIOELEMENT_PLUGIN_BINARY="../build/audioelementplugin/AudioElementPlugin_artefacts/$BUILD_CONFIG/AAX/Eclipsa Audio Element Plugin.aaxplugin/Contents/MacOS/Eclipsa Audio Element Plugin"
RENDERER_PLUGIN_RESOURCES="../build/rendererplugin/RendererPlugin_artefacts/$BUILD_CONFIG/AAX/Eclipsa Audio Renderer.aaxplugin/Contents/Resources"
AUDIOELEMENT_PLUGIN_RESOURCES="../build/audioelementplugin/AudioElementPlugin_artefacts/$BUILD_CONFIG/AAX/Eclipsa Audio Element Plugin.aaxplugin/Contents/Resources"
RENDERER_PLUGIN="../build/rendererplugin/RendererPlugin_artefacts/$BUILD_CONFIG/AAX/Eclipsa Audio Renderer.aaxplugin"
AUDIOELEMENT_PLUGIN="../build/audioelementplugin/AudioElementPlugin_artefacts/$BUILD_CONFIG/AAX/Eclipsa Audio Element Plugin.aaxplugin"

# Define the directories to write the signed files to
RENDERER_PLUGIN_SIGNING_DIRECTORY="../build/rendererplugin/RendererPlugin_artefacts/$BUILD_CONFIG/AAX/Signed"
AUDIOELEMENT_PLUGIN_SIGNING_DIRECTORY="../build/audioelementplugin/AudioElementPlugin_artefacts/$BUILD_CONFIG/AAX/Signed"
RENDERER_PLUGIN_SIGNED="../build/rendererplugin/RendererPlugin_artefacts/$BUILD_CONFIG/AAX/Signed/Eclipsa Audio Renderer.aaxplugin"
AUDIOELEMENT_PLUGIN_SIGNED="../build/audioelementplugin/AudioElementPlugin_artefacts/$BUILD_CONFIG/AAX/Signed/Eclipsa Audio Element Plugin.aaxplugin"

# Define the path to the signing tool
WRAP_TOOL_HELPER="./wraptool_helper.sh"

# Define the path to the license file
LICENSE_FILE="../LICENSE.txt"

#Define the output installers name
BRANCH_NAME=$(git rev-parse --abbrev-ref HEAD)
INSTALLER_NAME="Eclipsa_$BRANCH_NAME.pkg"


# Function to sign a dylib, either using ad-hoc or codesign
sign_dylib() {
    local dylib="$1"
    local resources="$2"
    
    # If wraptool is present, sign using the developer ID application, else sign using ad-hoc
    if [ -e "$WRAP_TOOL_HELPER" ]; then
        # NOTE: Replace with your Developer ID Application identity if performing official signing
        # codesign -s "Developer ID Application: Your Company Name (TEAMID)" --timestamp --deep --force "$resources/$dylib"
        echo "Wraptool detected, assuming it handles dylib signing. Skipping explicit dylib signing."
    else
        # Default to ad-hoc signing for open-source builds
        sudo codesign --force --deep --sign - "$resources/$dylib"
    fi
}

# Function to dynamically find and remove any ZMQ RPATH and apply ad-hoc code signing
adjust_rpath_and_sign() {
    local target_file="$1"

    if [ ! -f "$target_file" ]; then
        echo "Error: File $target_file does not exist. Skipping."
        return
    fi

    echo "Processing file: $target_file"

    # Dynamically find the ZMQ RPATH
    RPATH_TO_REMOVE=$(otool -l "$target_file" | grep "path .*zeromq-build/lib" | sed -n 's/.*path \([^ ]*\).*/\1/p')

    # If RPATH is found, remove it; otherwise, skip
    if [ -n "$RPATH_TO_REMOVE" ]; then
        install_name_tool -delete_rpath "$RPATH_TO_REMOVE" "$target_file"
        echo "Removed RPATH: $RPATH_TO_REMOVE from $target_file"
    else
        echo "No ZMQ RPATH found in $target_file, skipping."
    fi
}

# Clean up any existing package files
rm -f ./*.pkg

# Adjust RPATH for each plugin
adjust_rpath_and_sign "$RENDERER_PLUGIN_BINARY"
adjust_rpath_and_sign "$AUDIOELEMENT_PLUGIN_BINARY"

# Sign all the dylibs for the renderer plugin
sign_dylib "libzmq.5.2.6.dylib" "$RENDERER_PLUGIN_RESOURCES"
sign_dylib "libzmq.5.dylib" "$RENDERER_PLUGIN_RESOURCES"
sign_dylib "libzmq.dylib" "$RENDERER_PLUGIN_RESOURCES"
sign_dylib "third_party/iamftools/lib/libiamf_renderer_encoder.dylib" "$RENDERER_PLUGIN_RESOURCES"
sign_dylib "third_party/obr/lib/obr.dylib" "$RENDERER_PLUGIN_RESOURCES"

# Sign all the dylibs for the Audio Element Plugin
sign_dylib "libzmq.5.2.6.dylib" "$AUDIOELEMENT_PLUGIN_RESOURCES"
sign_dylib "libzmq.5.dylib" "$AUDIOELEMENT_PLUGIN_RESOURCES"
sign_dylib "libzmq.dylib" "$AUDIOELEMENT_PLUGIN_RESOURCES"
sign_dylib "third_party/iamftools/lib/libiamf_renderer_encoder.dylib" "$AUDIOELEMENT_PLUGIN_RESOURCES"
sign_dylib "third_party/obr/lib/obr.dylib" "$AUDIOELEMENT_PLUGIN_RESOURCES"


# Create directories to put the signed files in
mkdir -p "$RENDERER_PLUGIN_SIGNING_DIRECTORY"
mkdir -p "$AUDIOELEMENT_PLUGIN_SIGNING_DIRECTORY"

# Sign the .aax files
if [ -e "$WRAP_TOOL_HELPER" ]; then
    # If wraptool is present, sign the plugins using the wraptool
    echo "Applying wraptool"
    $WRAP_TOOL_HELPER "$RENDERER_PLUGIN" "$RENDERER_PLUGIN_SIGNED" "$AUDIOELEMENT_PLUGIN" "$AUDIOELEMENT_PLUGIN_SIGNED"
else
    # If the wraptool is not present, perform ad-hoc signing instead
    echo "Applying ad-hoc code signing"
    sudo codesign --force --deep --sign - "$RENDERER_PLUGIN_BINARY"
    sudo codesign --force --deep --sign - "$AUDIOELEMENT_PLUGIN_BINARY"
    echo "Ad-hoc code signing applied"

    # Copy the files to the signing directory (done automatically by wraptool)
    sudo cp -R "$RENDERER_PLUGIN" "$RENDERER_PLUGIN_SIGNED"
    sudo cp -R "$AUDIOELEMENT_PLUGIN" "$AUDIOELEMENT_PLUGIN_SIGNED"
fi


# Prepare the directory structure for packaging
echo "Copying plugins and license to temporary directory for packaging"
sudo rm -rf "./avid_plugins_pkg"
mkdir -p "./avid_plugins_pkg/Library/Application Support/Avid/Audio/Plug-Ins"
mkdir -p "./avid_plugins_pkg/Library/Application Support/Eclipsa Audio Plugins"

# Copy plugins and license to their respective folders
sudo cp -R "$RENDERER_PLUGIN_SIGNED" "$AUDIOELEMENT_PLUGIN_SIGNED" "./avid_plugins_pkg/Library/Application Support/Avid/Audio/Plug-Ins"
sudo cp "$LICENSE_FILE" "./avid_plugins_pkg/Library/Application Support/Eclipsa Audio Plugins/"

# Set permissions
sudo chown -R root:admin "./avid_plugins_pkg"
sudo chmod -R 755 "./avid_plugins_pkg"

# Build the installer package for the plugins and license file
pkgbuild --root "./avid_plugins_pkg" \
    --identifier com.eclipsaproject.plugins \
    --install-location "/" \
    --version "1.0.0" \
    "./EclipsaPlugins.pkg"

# Build the distribution, signing it if the wraptool is present
if [ -e "$WRAP_TOOL_HELPER" ]; then
    # NOTE: Replace with your Developer ID Installer identity if performing official signing
    # productbuild --distribution "./distribution.xml" --resources "./" --package-path "./" "./$INSTALLER_NAME" --sign "Developer ID Installer: Your Company Name (TEAMID)"
    echo "Wraptool detected, assuming it handles distribution signing. Skipping explicit signing."
    # If not using wraptool but still want official signing, uncomment the line above and replace placeholders.
    # Also, uncomment the notarization lines below and replace the keychain profile name.

    # Perform notorization since we have a valid signed package
    # echo "Notarizing the installer"
    # xcrun notarytool submit ./$INSTALLER_NAME --keychain-profile "<Your Keychain Profile Name>" --wait
    # xcrun stapler staple ./$INSTALLER_NAME
    echo "Skipping notarization for non-wraptool build."
else
    # Default unsigned build for open-source
    productbuild --distribution "./distribution.xml" --resources "./" --package-path "./" "./$INSTALLER_NAME"
fi

# Clean up
sudo rm -rf "./avid_plugins_pkg"
echo "Installer created at ./$INSTALLER_NAME"
