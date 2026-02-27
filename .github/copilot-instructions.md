# Copilot Instructions for Unformat

## Project Overview
Unformat is a single-file, header-only C++11 library (`unformat.h`) for parsing and extracting data from brace-style `{}` formatted strings. Public domain.

## Build
Uses CMake (minimum 3.5). Dependencies (Google Test 1.12.1, Google Benchmark 1.7.1) are auto-downloaded via `DownloadProject`.

```sh
cmake -B build
cmake --build build --config Release
```

On Windows this produces `.\build\Release\*.exe`. Debug builds go to `.\build\Debug\`.

## Test
Tests use Google Test and are in `unformat_test.cpp`.

```sh
.\build\Release\unformat_test.exe   # Windows
./build/unformat_test               # Linux/macOS
```

## Benchmark
Benchmarks use Google Benchmark and are in `unformat_benchmark.cpp`. Always build and run in **Release** for meaningful results.

```sh
.\build\Release\unformat_benchmark.exe   # Windows
./build/unformat_benchmark               # Linux/macOS
```

## Code Conventions
- C++11 standard (`CMAKE_CXX_STANDARD 11`); optional C++17 features behind `UNFORMAT_CPP17` / `_HAS_CXX17`
- All library code lives in `unformat.h` — keep it header-only
- Use `ay::` namespace for public API (`ay::unformat`, `ay::make_format`)
- `constexpr` format strings via `ay::make_format` for compile-time optimization
