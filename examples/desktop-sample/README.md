# Desktop Sample

A minimal terminal-based app that demonstrates:
- Registering a component with commands using `CommandShell`
- Wiring line-oriented input/output via `CommandShellIO`
- Providing a simple prompt and handling `help`, custom commands, and exit

## Build

From the repository root:

```bash
cmake -S . -B build -DBUILD_SAMPLES=ON
cmake --build build
```

## Run

Binary location depends on your generator and platform:

- Unix-like (Ninja/Make): `./build/desktop-sample`
- MSVC: `build/desktop-sample/desktop-sample.exe`

You should see a prompt like:

```
CommandShell Desktop Sample
Type 'exit' or 'quit' to leave.
Try commands: help list | help sample | sample about

cmd> 
```

Try a few commands:

- `help list` — list all registered components
- `help sample` — show help for the sample component
- `sample about` — describe this sample application
- `sample echo hello world` — prints `hello world`
- `sample sum 1 2 3` — prints `6`

