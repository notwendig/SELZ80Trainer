#!/usr/bin/env python3

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path


# Symbols required by CamelForth from SELRom.
API_LABELS = {
    "v24out",
    "v24stat",
    "v24in",
    "warmstart",
    "sendds_1",
    "senda",
    "sendhl",
    "gethx",
    "getadr",
    "delay_1",
    "conv7seg",
    "keystat",
}


# zmac symbol table:
#
#   EQU/DEFL:
#       NAME      =1234      4660
#
#   ordinary label:
#       NAME       1234      4660
#
# '/' is used for common symbols and is ignored here.
SYMBOL_RE = re.compile(
    r"""^
        (?P<name>\S+)
        \s+
        (?P<sep>[=/]?)
        \s*
        (?P<hex>[0-9A-Fa-f]{1,8})
        \s+
        (?P<decimal>-?\d+)
        (?:\s+.*)?
        $
    """,
    re.VERBOSE,
)

C_IDENT_RE = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")

CPP_NAME_MAP = {
    "KeyCode_??": "KeyCode_RS",
}


def extract_symbols(text: str) -> list[tuple[str, int]]:
    lines = text.splitlines()

    try:
        start = next(
            i for i, line in enumerate(lines)
            if line.strip() == "Symbol Table:"
        ) + 1
    except StopIteration:
        raise ValueError("zmac 'Symbol Table:' section not found")

    symbols: list[tuple[str, int]] = []
    seen: set[str] = set()
    found_api: set[str] = set()

    for line in lines[start:]:
        line = line.replace("\f", "").rstrip()

        if not line:
            continue

        m = SYMBOL_RE.match(line)
        if not m:
            continue

        name = m.group("name")
        lname = name.lower()
        sep = m.group("sep")
        value = int(m.group("hex"), 16)

        # Keep:
        #   - all EQU/DEFL symbols
        #   - explicitly required SELRom address labels
        if sep != "=" and lname not in API_LABELS:
            continue

        if name in seen:
            continue

        seen.add(name)
        symbols.append((name, value))

        if lname in API_LABELS:
            found_api.add(lname)

    missing = sorted(API_LABELS - found_api)

    if missing:
        raise ValueError(
            "Required SELRom API symbols not found in zmac symbol table: "
            + ", ".join(missing)
        )

    if not symbols:
        raise ValueError("No SELRom symbols found")

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
            f"Symbol '{name}' is not a valid C/C++ identifier "
            "and has no CPP_NAME_MAP entry"
        )

    return mapped


def generate_inc(
    symbols: list[tuple[str, int]],
    source_name: str,
) -> str:

    width = max(len(name) for name, _ in symbols)

    out = [
        "; --------------------------------------------------------------",
        f"; GENERATED FILE - source: {source_name}",
        "; Do not edit by hand.",
        "; SELRom EQU/DEFL symbols plus exported ROM API labels.",
        "; --------------------------------------------------------------",
        "",
    ]

    for name, value in symbols:
        out.append(
            f"{name:<{width}} equ {asm_hex(value)}"
        )

    out.append("")
    return "\n".join(out)


def generate_header(
    symbols: list[tuple[str, int]],
    source_name: str,
) -> str:

    mapped = []
    used = {}

    for asm_name, value in symbols:
        name = cpp_name(asm_name)

        previous = used.get(name)
        if previous is not None and previous != asm_name:
            raise ValueError(
                f"C++ symbol collision: "
                f"{previous} / {asm_name} -> {name}"
            )

        used[name] = asm_name
        mapped.append((name, value, asm_name))

    width = max(len(name) for name, _, _ in mapped)

    out = [
        "#pragma once",
        "",
        "#include <cstdint>",
        "",
        "// --------------------------------------------------------------",
        f"// GENERATED FILE - source: {source_name}",
        "// Do not edit by hand.",
        "// --------------------------------------------------------------",
        "",
    ]

    for name, value, asm_name in mapped:
        suffix = ""

        if name != asm_name:
            suffix = f"  // zmac: {asm_name}"

        out.append(
            f"inline constexpr std::uint32_t "
            f"{name:<{width}} = {cpp_hex(value)}u;{suffix}"
        )

    out.append("")
    return "\n".join(out)


def write_if_changed(path: Path, text: str) -> bool:
    old = None

    if path.exists():
        old = path.read_text(
            encoding="utf-8",
            errors="replace",
        )

    if old == text:
        return False

    path.parent.mkdir(
        parents=True,
        exist_ok=True,
    )

    path.write_text(
        text,
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

        symbols = extract_symbols(text)

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

        header_changed = write_if_changed(
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
        f"({'updated' if header_changed else 'unchanged'})"
    )

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
