# CommandShellLedArduino

An Arduino sketch demonstrating how to use this library’s `CommandShell` and `CommandShellIO` over the serial port to control the built‑in LED.

Compatibility note:
- This library is not optimized for very small microcontrollers (e.g., Arduino Uno/Nano) and may exceed RAM/flash limits.
- This example has been tested primarily on ESP8266 boards.

## What It Does
- Registers a `led` component with commands: `on`, `off`, `toggle`, `status`.
- Parses lines from UART in the form: `<component> <command> [args]`.
- Sends responses and a prompt via the serial output callback.

See `examples/CommandShellLedArduino/CommandShellLedArduino.ino` for the full sketch.

## Requirements
- Arduino IDE 1.8+/2.x or PlatformIO
- A board with a built‑in LED (`LED_BUILTIN`); falls back to pin `13` if not defined

## Upload (Arduino IDE)
- Open `examples/CommandShellLedArduino/CommandShellLedArduino.ino`.
- Select your Board and Port.
- Upload.

## Serial Monitor Settings
- Baud: `115200`
- Line ending: `Newline` (or `Both NL & CR`)

## Try These Commands
- `help` — show general help
- `help list` — list components
- `help led` — help for the `led` component
- `led on` — turn LED on
- `led off` — turn LED off
- `led toggle` — toggle LED
- `led status` — print current LED state

Example session (Serial Monitor):

```
CommandShellLed ready.
Baud: 115200
Type 'help' or 'help led' for commands.
cmd> led on
OK: LED ON
cmd> led status
LED: ON
cmd> led toggle
LED: OFF
cmd>
```

## Notes
- Uses `LED_BUILTIN` when available; otherwise uses digital pin `13`.
- Input buffer is limited to 127 characters per line; longer input is truncated until newline.

## Troubleshooting
- No output: verify the correct port and `115200` baud are selected.
- Garbled text: baud mismatch or wrong line ending; set to `Newline`.
- Commands ignored until Enter: the sketch executes only on newline (`\n`).
