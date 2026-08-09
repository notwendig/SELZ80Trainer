#!/usr/bin/env bash
set -euo pipefail

root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$root"

for tool in cmake python3 zmac; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "ERROR: required tool not found: $tool" >&2
        exit 1
    fi
done

if [[ ! -f Z80/CMakeLists.txt || ! -f qvintage/CMakeLists.txt ]]; then
    echo "ERROR: submodules are not initialized." >&2
    echo "Run: git submodule update --init --recursive" >&2
    exit 1
fi

python3 -m py_compile scripts/generate_selrom_symbols.py

cmake --preset debug
cmake --build --preset debug

for file in \
    build/debug/roms/SELRom.cim \
    build/debug/roms/SELRom.lst \
    build/debug/roms/SELRom.bin \
    Resources/SELRom.bin \
    ROMS/SELRom.inc \
    ROMS/SELRom.h
do
    if [[ ! -f "$file" ]]; then
        echo "ERROR: expected generated file is missing: $file" >&2
        exit 1
    fi
done

echo "SELZ80Trainer checks passed."
