# Architecture

SELZ80Trainer is split into a small application layer and two reusable submodules.

## Components

### `src/`

Qt 6 desktop application.

- `main.cpp` — application entry point
- `mainwindow.*` — main UI and user interaction
- `SELZ80Trainer.*` — trainer/emulation integration
- `Manual.*` — integrated manual window
- `PowerOn.h` — power-related UI support

### `Z80/`

Git submodule containing the Zilog Z80 CPU emulator.

The application links the static `Z80::Z80` target.

### `qvintage/`

Git submodule containing reusable vintage-style Qt widgets.

The application links `qvintage::Widgets`. The Qt Designer plugin is disabled when
QtVintage is built as a SELZ80Trainer submodule.

### `ROMS/`

Z80 assembler sources and generated symbol interfaces.

`SELRom.z80` is assembled by zmac. The zmac listing is parsed by
`scripts/generate_selrom_symbols.py` to generate interfaces shared with the emulator.

### `Resources/`

Qt resource inputs including images and the generated `SELRom.bin`.

`resources.qrc` embeds the generated ROM in the executable.

## Build dependency flow

```text
                 +----------------+
                 | SELRom.z80     |
                 +-------+--------+
                         |
                       zmac
                         |
                +--------+---------+
                |                  |
          SELRom.cim          SELRom.lst
                |                  |
              copy              parser
                |                  |
          SELRom.bin       +------+------+
                |          |             |
                |     SELRom.inc    SELRom.h
                |
      Resources/SELRom.bin
                |
             Qt RCC
                |
                v
       +------------------+
       | SELZ80Trainer    |
       +------------------+
          ^            ^
          |            |
        Z80         QtVintage
```

## Build ordering

The CMake ROM target must complete before Qt's resource compiler and before C++
sources that include the generated `SELRom.h` are compiled. Generated files therefore
have exactly one owning custom command and are exposed through an explicit dependency.

This ordering is important: multiple custom commands producing the same output will
cause Make/Ninja rule conflicts.
