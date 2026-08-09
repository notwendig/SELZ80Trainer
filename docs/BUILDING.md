# Building SELZ80Trainer

## 1. Prerequisites

SELZ80Trainer requires:

- CMake >= 3.21
- Qt >= 6.2
- C++20 compiler
- Python 3
- zmac
- Git
- Ninja or Make

## Fedora

```bash
sudo dnf install \
    cmake \
    ninja-build \
    gcc \
    gcc-c++ \
    make \
    git \
    python3 \
    bison \
    qt6-qtbase-devel \
    qt6-qttools-devel
```

## Ubuntu / Debian

```bash
sudo apt-get update

sudo apt-get install \
    cmake \
    ninja-build \
    gcc \
    g++ \
    make \
    git \
    python3 \
    bison \
    qt6-base-dev \
    qt6-tools-dev \
    qt6-tools-dev-tools
```

## 2. Clone with submodules

```bash
git clone --recursive git@github.com:notwendig/SELZ80Trainer.git
cd SELZ80Trainer
```

Existing clone:

```bash
git submodule sync --recursive
git submodule update --init --recursive
```

## 3. Build zmac

If `zmac` is not already installed:

```bash
git clone https://github.com/gp48k/zmac.git /tmp/zmac
make -C /tmp/zmac/src
mkdir -p "$HOME/bin"
install -m 0755 /tmp/zmac/src/zmac "$HOME/bin/zmac"
```

CMake searches both `PATH` and `$HOME/bin`.

## 4. Configure and build

### Debug

```bash
cmake --preset debug
cmake --build --preset debug
```

### Release

```bash
cmake --preset release
cmake --build --preset release
```

## 5. Local installation

```bash
cmake --install build/release --prefix "$HOME/.local"
```

## 6. Generated ROM files

A successful build generates at least:

```text
build/debug/roms/SELRom.cim
build/debug/roms/SELRom.lst
build/debug/roms/SELRom.bin
Resources/SELRom.bin
ROMS/SELRom.inc
ROMS/SELRom.h
```

For another preset, replace `build/debug` with that preset's build directory.

## 7. Clean build

```bash
rm -rf build/debug
cmake --preset debug
cmake --build --preset debug
```

For Qt Creator builds using `build/Desktop_Debug`:

```bash
rm -rf build/Desktop_Debug

cmake \
    -S . \
    -B build/Desktop_Debug \
    -DCMAKE_BUILD_TYPE=Debug

cmake --build build/Desktop_Debug -j"$(nproc)"
```

## Troubleshooting

### Submodule missing

```bash
git submodule update --init --recursive
```

### zmac not found

```bash
command -v zmac
ls -l ~/bin/zmac
```

If needed:

```bash
export PATH="$HOME/bin:$PATH"
```

### Stale CMake/Makefiles

After structural CMake changes, remove the affected build directory and configure
again instead of reusing stale generated Makefiles.
