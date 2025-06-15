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

# Test
./build/main_test

# Benchmark
mkdir -p benchmark
./build/main_benchmark --benchmark_format=json --benchmark_out=benchmark/benchmark_results.json

# Benchmark graphs (will be generated in ./graphs)
mkdir -p graphs
python3 ./utils/benchmark_graphs.py benchmark/benchmark_results.json
```

## Reference
- https://www.pcg-random.org/ (used in the random exam generator)
- [Intel Intrinsics Guide](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html)

## License