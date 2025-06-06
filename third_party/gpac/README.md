- Compiled from: https://github.com/jwcullen/gpac/tree/iamf_to_mp4
- Commit: https://github.com/jwcullen/gpac/commit/c7a08b82c6ac8889f8bd5e625f3ceec3e17bbdbe
- Version: 1.0.0 (iamf_to_mp4 branch - c7a08b8)

### Compile notes
- Compiled for ARM OSX (Sonoma 14.4.1, clang 15.0.0).
### Steps taken to build the library:
1. Ran the configure script with the following options: ``` ./configure --prefix=<install_path> --static-bin --use-curl=no ```. Building with these options is important as gpac tries to use available external libs, so we explicitly disable them (except for zlib currently).
2. Built and installed with `make -jxx` and `make install-lib`.