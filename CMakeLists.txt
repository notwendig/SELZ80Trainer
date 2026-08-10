cmake_minimum_required(VERSION 3.21)

project(
    SELZ80Trainer
    VERSION 1.0.0
    LANGUAGES CXX
)

include(GNUInstallDirs)

# ===========================================================================
# Qt / Python
# ===========================================================================

find_package(
    Qt6 6.2 REQUIRED COMPONENTS
        Widgets
        LinguistTools
)

find_package(
    Python3 REQUIRED COMPONENTS
        Interpreter
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ===========================================================================
# Z80 submodule
# ===========================================================================

set(
    Z80_SOURCE_DIR
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

set(
    Z80_BUILD_SHARED
    OFF
    CACHE BOOL
    "Build shared Z80 library"
    FORCE
)

set(
    Z80_BUILD_STATIC
    ON
    CACHE BOOL
    "Build static Z80 library"
    FORCE
)

add_subdirectory(
    "${Z80_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}/Z80"
)

# ===========================================================================
# qvintage submodule
# ===========================================================================

set(
    QVINTAGE_SOURCE_DIR
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

# SELZ80Trainer only needs the widget library.
# Do not build the Qt Designer plugin as part of this project.
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

# ===========================================================================
# zmac
# ===========================================================================

find_program(
    ZMAC_EXECUTABLE
    NAMES zmac
    HINTS
        "$ENV{HOME}/bin"
    REQUIRED
)

message(STATUS "zmac: ${ZMAC_EXECUTABLE}")

# ===========================================================================
# ROM directories
# ===========================================================================

set(
    SELZ80_ROMS_SOURCE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/ROMS"
)

set(
    SELZ80_ROM_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/roms"
)

set(
    SELZ80_RESOURCE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
)

set(
    SELROM_SYMBOL_GENERATOR
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_selrom_symbols.py"
)

if(NOT EXISTS "${SELROM_SYMBOL_GENERATOR}")
    message(FATAL_ERROR
        "SELRom symbol generator is missing:\n"
        "  ${SELROM_SYMBOL_GENERATOR}"
    )
endif()

# ===========================================================================
# All assembler source files
#
# These are also added to the Qt Creator project tree.
# ===========================================================================

file(
    GLOB Z80_ASM_FILES
    CONFIGURE_DEPENDS
    "${SELZ80_ROMS_SOURCE_DIR}/*.z80"
)

file(
    GLOB Z80_AZM_FILES
    CONFIGURE_DEPENDS
    "${SELZ80_ROMS_SOURCE_DIR}/*.azm"
)

if(NOT Z80_ASM_FILES)
    message(FATAL_ERROR
        "No Z80 ROM sources found in:\n"
        "  ${SELZ80_ROMS_SOURCE_DIR}"
    )
endif()

set_source_files_properties(
    ${Z80_ASM_FILES}
    ${Z80_AZM_FILES}
    PROPERTIES
        HEADER_FILE_ONLY TRUE
)

# ===========================================================================
# SELRom
# ===========================================================================

set(
    SELROM_SOURCE
    "${SELZ80_ROMS_SOURCE_DIR}/SELRom.z80"
)

set(
    SELROM_CIM
    "${SELZ80_ROM_DIR}/SELRom.cim"
)

set(
    SELROM_LST
    "${SELZ80_ROM_DIR}/SELRom.lst"
)

set(
    SELROM_BIN
    "${SELZ80_ROM_DIR}/SELRom.bin"
)

# Generated assembler include used by camel80.
set(
    SELROM_INC
    "${SELZ80_ROMS_SOURCE_DIR}/SELRom.inc"
)

# Generated C++ symbol header.
#
# IMPORTANT:
# This is deliberately NOT ROMS/SELRom.h.
#
# ROMS/SELRom.h is the existing static hardware header containing
# SIO8251_Dataport, SIO8251_CntrPort, PIO_8255_CntrlStatus, etc.
set(
    SELROM_GENERATED_HEADER
    "${CMAKE_CURRENT_BINARY_DIR}/generated/SELRomSymbols.h"
)

# Runtime ROM copy used by the Qt application/resources.
set(
    SELROM_RESOURCE_BIN
    "${SELZ80_RESOURCE_DIR}/SELRom.bin"
)

if(NOT EXISTS "${SELROM_SOURCE}")
    message(FATAL_ERROR
        "Missing ROM source:\n"
        "  ${SELROM_SOURCE}"
    )
endif()

# ===========================================================================
# Stage 1
#
# SELRom.z80
#      |
#      +--> SELRom.cim
#      +--> SELRom.lst
#      +--> SELRom.bin
#      +--> Resources/SELRom.bin
# ===========================================================================

add_custom_command(
    OUTPUT
        "${SELROM_CIM}"
        "${SELROM_LST}"
        "${SELROM_BIN}"
        "${SELROM_RESOURCE_BIN}"

    COMMAND
        "${CMAKE_COMMAND}" -E make_directory
        "${SELZ80_ROM_DIR}"

    COMMAND
        "${CMAKE_COMMAND}" -E make_directory
        "${SELZ80_RESOURCE_DIR}"

    COMMAND
        "${ZMAC_EXECUTABLE}"
        -czfmL
        --zmac
        --od "${SELZ80_ROM_DIR}"
        --oo cim,lst
        SELRom.z80

    COMMAND
        "${CMAKE_COMMAND}" -E copy_if_different
        "${SELROM_CIM}"
        "${SELROM_BIN}"

    COMMAND
        "${CMAKE_COMMAND}" -E copy_if_different
        "${SELROM_BIN}"
        "${SELROM_RESOURCE_BIN}"

    DEPENDS
        "${SELROM_SOURCE}"

    WORKING_DIRECTORY
        "${SELZ80_ROMS_SOURCE_DIR}"

    COMMENT
        "Assembling SELRom.z80"

    VERBATIM
)

# ===========================================================================
# Stage 2
#
# SELRom.lst
#      |
#      +--> ROMS/SELRom.inc
#      +--> build/.../generated/SELRomSymbols.h
#
# ROMS/SELRom.h is NOT overwritten.
# ===========================================================================

add_custom_command(
    OUTPUT
        "${SELROM_INC}"
        "${SELROM_GENERATED_HEADER}"

    COMMAND
        "${CMAKE_COMMAND}" -E make_directory
        "${CMAKE_CURRENT_BINARY_DIR}/generated"

    COMMAND
        "${Python3_EXECUTABLE}"
        "${SELROM_SYMBOL_GENERATOR}"
        "${SELROM_LST}"
        "${SELROM_INC}"
        "${SELROM_GENERATED_HEADER}"

    DEPENDS
        "${SELROM_LST}"
        "${SELROM_SYMBOL_GENERATOR}"

    COMMENT
        "Generating SELRom.inc from SELRom.lst"

    VERBATIM
)

# ===========================================================================
# camel80 / CamelForth
# ===========================================================================

set(
    CAMEL80_SOURCE
    "${SELZ80_ROMS_SOURCE_DIR}/camel80.z80"
)

set(
    CAMEL80D_SOURCE
    "${SELZ80_ROMS_SOURCE_DIR}/camel80d.azm"
)

set(
    CAMEL80H_SOURCE
    "${SELZ80_ROMS_SOURCE_DIR}/camel80h.azm"
)

set(
    CAMELSEL_SOURCE
    "${SELZ80_ROMS_SOURCE_DIR}/camelSEL.azm"
)

set(
    CAMEL80_CIM
    "${SELZ80_ROM_DIR}/camel80.cim"
)

set(
    CAMEL80_LST
    "${SELZ80_ROM_DIR}/camel80.lst"
)

set(
    CAMEL80_BIN
    "${SELZ80_ROM_DIR}/camel80.bin"
)

# This is the required runtime/resource copy.
set(
    CAMEL80_RESOURCE_BIN
    "${SELZ80_RESOURCE_DIR}/camel80.bin"
)

# ===========================================================================
# Stage 3
#
# SELRom.inc
#     |
#     v
# camel80.z80 + camel80d.azm + camel80h.azm + camelSEL.azm
#     |
#     +--> camel80.cim
#     +--> camel80.lst
#     +--> camel80.bin
#     +--> Resources/camel80.bin
#
# --nmnv is used because this is the mode verified manually with camel80.
# ===========================================================================

if(EXISTS "${CAMEL80_SOURCE}")

    add_custom_command(
        OUTPUT
            "${CAMEL80_CIM}"
            "${CAMEL80_LST}"
            "${CAMEL80_BIN}"
            "${CAMEL80_RESOURCE_BIN}"

        COMMAND
            "${CMAKE_COMMAND}" -E make_directory
            "${SELZ80_ROM_DIR}"

        COMMAND
            "${CMAKE_COMMAND}" -E make_directory
            "${SELZ80_RESOURCE_DIR}"

        COMMAND
            "${ZMAC_EXECUTABLE}"
            -czfmL
            --zmac
            --nmnv
            --od "${SELZ80_ROM_DIR}"
            --oo cim,lst
            camel80.z80

        COMMAND
            "${CMAKE_COMMAND}" -E copy_if_different
            "${CAMEL80_CIM}"
            "${CAMEL80_BIN}"

        COMMAND
            "${CMAKE_COMMAND}" -E copy_if_different
            "${CAMEL80_BIN}"
            "${CAMEL80_RESOURCE_BIN}"

        DEPENDS
            "${CAMEL80_SOURCE}"
            "${CAMEL80D_SOURCE}"
            "${CAMEL80H_SOURCE}"
            "${CAMELSEL_SOURCE}"
            "${SELROM_INC}"

        WORKING_DIRECTORY
            "${SELZ80_ROMS_SOURCE_DIR}"

        COMMENT
            "Assembling camel80.z80 -> Resources/camel80.bin"

        VERBATIM
    )

endif()

# ===========================================================================
# ROM output list
# ===========================================================================

set(
    SELZ80_ROM_OUTPUTS

    "${SELROM_CIM}"
    "${SELROM_LST}"
    "${SELROM_BIN}"
    "${SELROM_RESOURCE_BIN}"

    "${SELROM_INC}"
    "${SELROM_GENERATED_HEADER}"
)

if(EXISTS "${CAMEL80_SOURCE}")

    list(
        APPEND
        SELZ80_ROM_OUTPUTS

        "${CAMEL80_CIM}"
        "${CAMEL80_LST}"
        "${CAMEL80_BIN}"
        "${CAMEL80_RESOURCE_BIN}"
    )

endif()

# ===========================================================================
# Other *.z80 files
#
# SELRom and camel80 are handled separately because they have explicit
# dependencies and generated files.
# ===========================================================================

foreach(Z80_FILE IN LISTS Z80_ASM_FILES)

    get_filename_component(
        Z80_NAME
        "${Z80_FILE}"
        NAME_WE
    )

    if(
        Z80_NAME STREQUAL "SELRom"
        OR
        Z80_NAME STREQUAL "camel80"
    )
        continue()
    endif()

    set(
        CIM_FILE
        "${SELZ80_ROM_DIR}/${Z80_NAME}.cim"
    )

    set(
        LST_FILE
        "${SELZ80_ROM_DIR}/${Z80_NAME}.lst"
    )

    set(
        BIN_FILE
        "${SELZ80_ROM_DIR}/${Z80_NAME}.bin"
    )

    add_custom_command(
        OUTPUT
            "${CIM_FILE}"
            "${LST_FILE}"
            "${BIN_FILE}"

        COMMAND
            "${CMAKE_COMMAND}" -E make_directory
            "${SELZ80_ROM_DIR}"

        COMMAND
            "${ZMAC_EXECUTABLE}"
            -czfmL
            --zmac
            --od "${SELZ80_ROM_DIR}"
            --oo cim,lst
            "${Z80_FILE}"

        COMMAND
            "${CMAKE_COMMAND}" -E copy_if_different
            "${CIM_FILE}"
            "${BIN_FILE}"

        DEPENDS
            "${Z80_FILE}"

        WORKING_DIRECTORY
            "${SELZ80_ROMS_SOURCE_DIR}"

        COMMENT
            "Assembling ROMS/${Z80_NAME}.z80"

        VERBATIM
    )

    list(
        APPEND
        SELZ80_ROM_OUTPUTS

        "${CIM_FILE}"
        "${LST_FILE}"
        "${BIN_FILE}"
    )

endforeach()

# ===========================================================================
# Generated files
# ===========================================================================

set_source_files_properties(
    "${SELROM_CIM}"
    "${SELROM_LST}"
    "${SELROM_BIN}"
    "${SELROM_RESOURCE_BIN}"
    "${SELROM_INC}"
    "${SELROM_GENERATED_HEADER}"
    PROPERTIES
        GENERATED TRUE
)

set_source_files_properties(
    "${SELROM_INC}"
    "${SELROM_GENERATED_HEADER}"
    PROPERTIES
        HEADER_FILE_ONLY TRUE
)

if(EXISTS "${CAMEL80_SOURCE}")

    set_source_files_properties(
        "${CAMEL80_CIM}"
        "${CAMEL80_LST}"
        "${CAMEL80_BIN}"
        "${CAMEL80_RESOURCE_BIN}"
        PROPERTIES
            GENERATED TRUE
    )

endif()

# ===========================================================================
# ROM target
# ===========================================================================

add_custom_target(
    SELZ80Trainer_roms
    ALL

    DEPENDS
        ${SELZ80_ROM_OUTPUTS}
)

# ===========================================================================
# SELZ80Trainer executable
# ===========================================================================

qt_add_executable(
    SELZ80Trainer

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

    Resources/resources.qrc
)

# Make assembler files visible in Qt Creator.
target_sources(
    SELZ80Trainer
    PRIVATE
        ${Z80_ASM_FILES}
        ${Z80_AZM_FILES}

        # Static hardware definitions used by C++.
        "${SELZ80_ROMS_SOURCE_DIR}/SELRom.h"

        # Generated files.
        "${SELROM_INC}"
        "${SELROM_GENERATED_HEADER}"
)

# ROM generation must finish before C++ compilation/linking.
add_dependencies(
    SELZ80Trainer
    SELZ80Trainer_roms
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
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

target_link_libraries(
    SELZ80Trainer
    PRIVATE
        Qt6::Widgets
        Z80::Z80
        qvintage::Widgets
)

# ===========================================================================
# Translations
# ===========================================================================

qt_add_translations(
    SELZ80Trainer

    TS_FILES
        SELZ80Trainer_de_DE.ts

    RESOURCE_PREFIX
        /i18n
)

# ===========================================================================
# Application properties
# ===========================================================================

set_target_properties(
    SELZ80Trainer
    PROPERTIES
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

# ===========================================================================
# Install
# ===========================================================================

install(
    TARGETS SELZ80Trainer

    BUNDLE DESTINATION .
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
