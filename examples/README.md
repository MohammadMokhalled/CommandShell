# Examples

This folder contains example applications that demonstrate how to use the CommandShell library in different environments.

- `examples/desktop-sample` — Minimal terminal app using `CommandShell` + `CommandShellIO`.
- `examples/CommandShellLedArduino` — Arduino sketch controlling the built‑in LED via serial commands.

## Desktop Sample
- Configure and build with CMake:
  - `cmake -S . -B build -DBUILD_SAMPLES=ON`
  - `cmake --build build`
- Run the produced binary (path varies by generator/platform):
  - Unix-like (Ninja/Make): `./build/desktop-sample`
  - MSVC: `build/desktop-sample/desktop-sample.exe`

## Arduino Example
- Open `examples/CommandShellLedArduino/CommandShellLedArduino.ino` in the Arduino IDE (or use PlatformIO).
- Select your board/port and upload.
- Open Serial Monitor at `115200` baud with `Newline` (or NL&CR) line ending.
- Try: `help`, `help list`, `help led`, `led on`, `led off`, `led toggle`, `led status`.

Note on microcontrollers:
- The library is not optimized for very small MCUs and may not work on devices like Arduino Uno/Nano due to RAM/flash limits.
- This repository’s Arduino sketch has been tested primarily on ESP8266 boards.

See each subfolder’s README for details.
