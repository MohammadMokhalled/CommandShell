# Tests

This folder contains GoogleTest-based unit and integration tests for CommandShell.

## Files
- CommandShellTests.cpp — Core CommandShell unit tests: command dispatch, built‑in help, per‑component help, and option rendering in help output.
- CommandShellIOTests.cpp — CommandShellIO behavior: echo vs. no‑echo, prompt printing, input chunking, `splitInput`, `parseCommand`, and overload taking `char*`.
- CommandShellIntegrationTests.cpp — End‑to‑end flow: input through CommandShellIO executing commands in CommandShell and capturing output.

## Running
Using CMake/ctest (Linux/macOS/Windows):
- Configure: `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON`
- Build: `cmake --build build --config Debug --parallel`
- Test: `ctest --test-dir build -C Debug --output-on-failure`

Windows (VS Developer Command Prompt):
- `scripts\\build_and_test.bat` builds Debug and runs tests.

## Adding Tests
- Place new tests in this directory and include headers from `src/` (e.g., `#include "../src/CommandShell.hpp"`).
- Update the root `CMakeLists.txt` to add your new test source file to the `${PROJECT_NAME}_tests` executable (tests are listed explicitly).
- Prefer small, focused tests; keep integration scenarios in a dedicated `*IntegrationTests.cpp` file.

## Notes
- Tests use `gtest_discover_tests` so cases inside the single test binary are automatically discovered by CTest.
- The default prompt in IO tests is `"cmd> "` to match the library default.
