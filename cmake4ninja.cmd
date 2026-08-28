rmdir /s build
cmake -S . -B build --toolchain cmake\starm-clang.cmake -G Ninja
