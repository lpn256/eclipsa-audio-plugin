# libiamf

libIAMF (Immersive Audio Model and Format) is a library for decoding and processing audio content conforming to the IAMF standard. It provides tools for rendering immersive audio experiences.

This directory contains the libiamf library, which is used for decoding and processing IAMF (Immersive Audio Model and Format) audio.

## Dependencies

libiamf has the following dependencies:

*   **libopus**: Used for Opus audio decoding.
*   **libfdk-aac**: Used for Fraunhofer FDK AAC audio decoding.
*   **libFLAC**: Used for FLAC audio decoding.

These libraries are vendored in the `third_party/libiamf/third_party/lib/macos` directory for macOS builds. These macOS-specific versions are used for compatibility or performance reasons.

## Inclusion

libiamf is included in this project using the `FetchContent` CMake module. This allows the library to be downloaded and integrated into the build process automatically.

## Building

libiamf is built using CMake. The following options can be set:

*   `CODEC_CAP`: Enable codec capability check (ON/OFF).
*   `MULTICHANNEL_BINAURALIZER`: Enable multichannel binaural rendering (ON/OFF).
*   `HOA_BINAURALIZER`: Enable HOA binaural rendering (ON/OFF).

To include libiamf in your CMake project, use the following:

```cmake
FetchContent_Declare(
  libiamf
  GIT_REPOSITORY https://github.com/AOMediaCodec/libiamf.git
  GIT_TAG 48b8b5dc06971e14826d114fd032dab285f4c944
)

# Set options for libiamf before making it available
set(CODEC_CAP ON CACHE BOOL "Codec capability check" FORCE)
set(MULTICHANNEL_BINAURALIZER OFF CACHE BOOL "Enable multichannel binaural rendering" FORCE)
set(HOA_BINAURALIZER OFF CACHE BOOL "Enable HOA binaural rendering" FORCE)

# Make libiamf available
FetchContent_GetProperties(libiamf)
if(NOT libiamf_POPULATED)
  FetchContent_Populate(libiamf)
  add_subdirectory(${libiamf_SOURCE_DIR}/code ${libiamf_BINARY_DIR})
endif()

  ## Updating Codec Libraries

  The method for updating codec libraries depends on the operating system:

  *   **macOS:** The macOS-specific versions of the codec libraries are located in the `third_party/libiamf/third_party/lib/macos` directory. To update these libraries, you can either build them from source or copy them from Homebrew (see the instructions below).
  *   **Linux:** The codec libraries are installed using Homebrew. If Homebrew is not installed, you will need to install the libraries manually.

  ### Updating macOS Codec Files

  If you need to update the macOS codec files (`libopus.a`, `libfdk-aac.a`, `libFLAC.a`) in the `third_party/libiamf/third_party/lib/macos` directory, you have to:

  **Build from source**

  1.  **Download the updated codec libraries:**
      *   Download the source code for the desired codec library (e.g., from the official Opus, FDK AAC, or FLAC website).
      *   Extract the downloaded archive (e.g., a `.tar.gz` file).

  2.  **Build the libraries for macOS:**
      *   Follow the build instructions for the specific codec library to build it for macOS. This usually involves using CMake or a similar build system.
      *   Ensure that the build process creates static libraries (`.a` files).

  3.  **Replace the existing codec files:**
      *   Once you have built the updated codec libraries, replace the existing `.a` files in the `third_party/libiamf/third_party/lib/macos` directory with the newly built ones.
      *   Make sure to keep the same filenames (`libopus.a`, `libfdk-aac.a`, `libFLAC.a`).
