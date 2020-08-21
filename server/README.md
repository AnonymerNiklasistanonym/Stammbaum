# Server

The server which is the core part of this project.
It contains all the necessary logic and can be run separately from the frontend.
Any frontend can connect to it via the TCP protocol.

## Build instructions

1. Fix vendor files:

   ```sh
   cd ..
   ./fix_vendor_files_unix.sh
   ```

2. Run CMake or use the provided CMake bash script with some tweaks for threads and outputs:

   ```sh
   ./build_unix.sh
   ```

   (Keep in mind that `clang` is not supported because it prints errors with the current code while for example gcc doesn't)
