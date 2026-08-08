cmake_minimum_required(VERSION 3.21)

project(
    SELZ80Trainer
    VERSION 1.0.0
    LANGUAGES CXX
)

include(GNUInstallDirs)

# ---------------------------------------------------------------------------
# Qt
# ---------------------------------------------------------------------------

find_package(Qt6 6.2 REQUIRED COMPONENTS
    Widgets
    LinguistTools
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ---------------------------------------------------------------------------
# Z80 submodule
#
# SELZ80Trainer/Z80
# ---------------------------------------------------------------------------

set(Z80_SOURCE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/Z80"
    CACHE PATH
    "Path to the Z80 source tree"
)

if(NOT EXISTS "${Z80_SOURCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "Z80 git submodule is missing.\n"
        "Expected:\n"
        "  ${Z80_SOURCE_DIR}/CMakeLists.txt\n"
        "\n"
        "Initialize submodules with:\n"
        "  git submodule update --init --recursive"
    )
endif()

# SELZ80Trainer only needs the static Z80 library.
set(Z80_BUILD_SHARED OFF CACHE BOOL "Build shared Z80 library" FORCE)
set(Z80_BUILD_STATIC ON  CACHE BOOL "Build static Z80 library" FORCE)

add_subdirectory(
    "${Z80_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}/Z80"
)

# ---------------------------------------------------------------------------
# qvintage submodule
#
# SELZ80Trainer/qvintage
# ---------------------------------------------------------------------------

set(QVINTAGE_SOURCE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/qvintage"
    CACHE PATH
    "Path to the qvintage source tree"
)

if(NOT EXISTS "${QVINTAGE_SOURCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "qvintage git submodule is missing.\n"
        "Expected:\n"
        "  ${QVINTAGE_SOURCE_DIR}/CMakeLists.txt\n"
        "\n"
        "Initialize submodules with:\n"
        "  git submodule update --init --recursive"
    )
endif()

# Do not build the Qt Designer plugin for this application.
set(
    QVINTAGE_BUILD_DESIGNER_PLUGIN
    OFF
    CACHE BOOL
    "Build qvintage Qt Designer plugin"
    FORCE
)

add_subdirectory(
    "${QVINTAGE_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}/qvintage"
)

# ---------------------------------------------------------------------------
# SELZ80Trainer
# ---------------------------------------------------------------------------

qt_add_executable(SELZ80Trainer
    src/main.cpp

    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui

    src/SELZ80Trainer.cpp
    src/SELZ80Trainer.h

    src/Manual.cpp
    src/Manual.h
    src/Manual.ui

    src/PowerOn.h

    ROMS/SELRom.h

    Resources/resources.qrc
)

target_compile_features(
    SELZ80Trainer
    PRIVATE
        cxx_std_20
)

target_include_directories(
    SELZ80Trainer
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/ROMS"
)

target_link_libraries(
    SELZ80Trainer
    PRIVATE
        Qt6::Widgets
        Z80::Z80
        qvintage::Widgets
)

# ---------------------------------------------------------------------------
# Z80 assembler source files
#
# Keep *.z80 visible in Qt Creator.
# They are source/input files, not C++ compilation units.
# ---------------------------------------------------------------------------

file(
    GLOB Z80_ASM_FILES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/ROMS/*.z80"
)

if(Z80_ASM_FILES)
    set_source_files_properties(
        ${Z80_ASM_FILES}
        PROPERTIES
            HEADER_FILE_ONLY TRUE
    )

    target_sources(
        SELZ80Trainer
        PRIVATE
            ${Z80_ASM_FILES}
    )
endif()

# ---------------------------------------------------------------------------
# Translations
# ---------------------------------------------------------------------------

qt_add_translations(
    SELZ80Trainer
    TS_FILES
        SELZ80Trainer_de_DE.ts
    RESOURCE_PREFIX
        /i18n
)

# ---------------------------------------------------------------------------
# Platform properties
# ---------------------------------------------------------------------------

set_target_properties(
    SELZ80Trainer
    PROPERTIES
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

# ---------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------

install(
    TARGETS SELZ80Trainer
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
