#!/usr/bin/env python3
"""
Generate SELRom.inc and SELRom.h from the zmac Symbol Table in SELRom.lst.

The assembler include keeps the original zmac symbol names.
For C/C++, known assembler-only names can be mapped explicitly.

Usage:
    python3 generate_selrom_symbols.py SELRom.lst SELRom.inc SELRom.h
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path


SYMBOL_RE = re.compile(
    r"""^
        (?P<name>\S+)
        \s*
        =
        \s*
        (?P<hex>[0-9A-Fa-f]{2,8})
        \s+
        (?P<decimal>-?\d+)
        (?:\s+.*)?
        $
    """,
    re.VERBOSE,
)

C_IDENT_RE = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")

# Explicit translations from legal zmac symbols to legal C/C++ identifiers.
# Keep this list small and intentional; do not silently mangle arbitrary names.
CPP_NAME_MAP: dict[str, str] = {
    "KeyCode_??": "KeyCode_RS",
}


def extract_equates(text: str) -> list[tuple[str, int]]:
    lines = text.splitlines()

    try:
        start = next(
            i for i, line in enumerate(lines)
            if line.strip() == "Symbol Table:"
        ) + 1
    except StopIteration:
        raise ValueError("zmac 'Symbol Table:' section not found in listing")

    symbols: list[tuple[str, int]] = []
    seen: set[str] = set()

    for line in lines[start:]:
        line = line.replace("\f", "").rstrip()
        if not line:
            continue

        m = SYMBOL_RE.match(line)
        if not m:
            continue

        name = m.group("name")
        value = int(m.group("hex"), 16)

        if name in seen:
            continue

        seen.add(name)
        symbols.append((name, value))

    if not symbols:
        raise ValueError(
            "No EQU/DEFL symbols found after zmac 'Symbol Table:' section"
        )

    return symbols


def asm_hex(value: int) -> str:
    if value <= 0xFF:
        width = 2
    elif value <= 0xFFFF:
        width = 4
    else:
        width = 8
    return f"0{value:0{width}X}h"


def cpp_hex(value: int) -> str:
    if value <= 0xFF:
        width = 2
    elif value <= 0xFFFF:
        width = 4
    else:
        width = 8
    return f"0x{value:0{width}X}"


def cpp_name(name: str) -> str:
    mapped = CPP_NAME_MAP.get(name, name)

    if not C_IDENT_RE.match(mapped):
        raise ValueError(
            f"Symbol '{name}' is not a valid C/C++ identifier and has no "
            "explicit mapping in CPP_NAME_MAP"
        )

    return mapped


def generate_inc(
    symbols: list[tuple[str, int]],
    source_name: str,
) -> str:
    width = max(len(name) for name, _ in symbols)

    out = [
        "; ------------------------------------------------------------------",
        f"; GENERATED FILE - source: {source_name}",
        "; Do not edit by hand.",
        "; Original zmac EQU/DEFL symbol names are preserved.",
        "; ------------------------------------------------------------------",
        "",
    ]

    for name, value in symbols:
        out.append(f"{name:<{width}} equ {asm_hex(value)}")

    out.append("")
    return "\n".join(out)


def generate_header(
    symbols: list[tuple[str, int]],
    source_name: str,
) -> str:
    mapped_symbols: list[tuple[str, int, str]] = []
    used_names: dict[str, str] = {}

    for asm_name, value in symbols:
        cxx_name = cpp_name(asm_name)

        previous = used_names.get(cxx_name)
        if previous is not None and previous != asm_name:
            raise ValueError(
                f"C/C++ symbol collision: '{previous}' and '{asm_name}' "
                f"both map to '{cxx_name}'"
            )

        used_names[cxx_name] = asm_name
        mapped_symbols.append((cxx_name, value, asm_name))

    width = max(len(name) for name, _, _ in mapped_symbols)

    out = [
        "#pragma once",
        "",
        "#include <cstdint>",
        "",
        "// ------------------------------------------------------------------",
        f"// GENERATED FILE - source: {source_name}",
        "// Do not edit by hand.",
        "// Generated from zmac EQU/DEFL symbols.",
        "// ------------------------------------------------------------------",
        "",
    ]

    for name, value, asm_name in mapped_symbols:
        suffix = ""
        if name != asm_name:
            suffix = f"  // zmac: {asm_name}"

        out.append(
            f"inline constexpr std::uint32_t "
            f"{name:<{width}} = {cpp_hex(value)}u;{suffix}"
        )

    out.append("")
    return "\n".join(out)


def write_if_changed(path: Path, data: str) -> bool:
    old = None

    if path.exists():
        old = path.read_text(
            encoding="utf-8",
            errors="replace",
        )

    if old == data:
        return False

    path.parent.mkdir(
        parents=True,
        exist_ok=True,
    )

    path.write_text(
        data,
        encoding="utf-8",
        newline="\n",
    )

    return True


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("listing", type=Path)
    ap.add_argument("inc", type=Path)
    ap.add_argument("header", type=Path)
    args = ap.parse_args()

    try:
        text = args.listing.read_text(
            encoding="utf-8",
            errors="replace",
        )

        symbols = extract_equates(text)

        inc = generate_inc(
            symbols,
            args.listing.name,
        )

        header = generate_header(
            symbols,
            args.listing.name,
        )

        inc_changed = write_if_changed(
            args.inc,
            inc,
        )

        h_changed = write_if_changed(
            args.header,
            header,
        )

    except (OSError, ValueError) as exc:
        print(
            f"ERROR: {exc}",
            file=sys.stderr,
        )
        return 1

    print(
        f"Generated {len(symbols)} symbols: "
        f"{args.inc} "
        f"({'updated' if inc_changed else 'unchanged'}), "
        f"{args.header} "
        f"({'updated' if h_changed else 'unchanged'})"
    )

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
