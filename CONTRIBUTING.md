# Contributing to SELZ80Trainer

Thank you for contributing.

## Development setup

1. Clone the repository recursively.
2. Install Qt 6, CMake, Python 3 and zmac.
3. Configure a Debug build.
4. Build before making changes to establish a clean baseline.

See [docs/BUILDING.md](docs/BUILDING.md).

## Branches

Create a focused topic branch from the current default branch.

Examples:

```text
fix/rom-generation
feature/system-view
docs/build-instructions
```

## Coding style

- C++20
- Keep Qt ownership/lifetime rules explicit.
- Prefer target-based CMake.
- Do not add global compiler flags when a target property is sufficient.
- Keep generated build products out of hand-written source changes.
- Keep source and UI changes focused.

The repository contains `.clang-format` and `.editorconfig`.

## ROM changes

`ROMS/SELRom.z80` is the source of truth for the trainer ROM.

When changing ROM code:

1. Edit the `.z80` source.
2. Rebuild the project.
3. Review generated `SELRom.lst`, `SELRom.inc`, `SELRom.h` and `SELRom.bin`.
4. Commit generated source-tree artifacts only when they are intentionally tracked by
   the repository.

Do not manually fix generated symbols in `SELRom.h`; change the source symbol or the
explicit mapping in `scripts/generate_selrom_symbols.py`.

## Commit messages

Use short imperative summaries, for example:

```text
Fix SELRom resource build dependency
Add System 0 trainer view
Document zmac build requirements
```

## Pull requests

A pull request should:

- explain what changed and why;
- build successfully from a clean build directory;
- include screenshots for visible UI changes;
- include relevant ROM/generated-file changes;
- avoid unrelated formatting or refactoring.

## Reporting bugs

Use the GitHub bug report form and include:

- operating system;
- Qt version;
- compiler version;
- CMake version;
- exact build/run command;
- complete relevant error output;
- screenshot when the problem is visual.
