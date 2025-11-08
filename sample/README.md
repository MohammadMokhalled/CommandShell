# Samples

This folder contains example applications that demonstrate how to use the CommandShell library in different contexts.

- desktop-sample: A small terminal-based desktop app showing how to wire `CommandShell` with `CommandShellIO` to provide a prompt, parse commands, and print output.

## Building Samples

Enable samples when configuring your CMake build:

```bash
cmake -S . -B build -DBUILD_SAMPLES=ON
cmake --build build
```

You can then run the desktop sample binary from your build folder (path may vary by generator/platform):

- On Unix-like (Ninja/Make): `./build/desktop-sample`
- On MSVC (default out-of-source): `build/desktop-sample/desktop-sample.exe`

