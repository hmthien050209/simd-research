# simd-research
This repository contains code of my SIMD experiment.

## Requirements
- CMake (>= 3.20)
- GCC (>=10.0, with C++20 support)
- vcpkg
- pkg-config
- An x86_64 CPU with at least AVX2 support.

## Run
```shell
cmake --preset=default
cmake --build build
./build/main
```

## Reference
- https://www.pcg-random.org/ (used in the random exam generator)

## License