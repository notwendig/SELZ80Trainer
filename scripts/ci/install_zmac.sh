#!/usr/bin/env bash
set -euo pipefail

workdir="$(mktemp -d)"
trap 'rm -rf "$workdir"' EXIT

git clone --depth 1 https://github.com/gp48k/zmac.git "$workdir/zmac"

# zmac's upstream Makefile documents that its build is not parallel-safe.
make -C "$workdir/zmac/src" -j1

install_dir="${HOME}/bin"
mkdir -p "$install_dir"
install -m 0755 "$workdir/zmac/src/zmac" "$install_dir/zmac"

echo "$install_dir" >> "$GITHUB_PATH"
"$install_dir/zmac" --help >/dev/null 2>&1 || true
echo "zmac installed at $install_dir/zmac"
