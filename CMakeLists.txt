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

find_package(
    Qt6 6.2 REQUIRED COMPONENTS
        Widgets
        LinguistTools
)

find_package(
    Python3 REQUIRED COMPONENTS
        Interpreter
)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ---------------------------------------------------------------------------
# Z80 submodule
# ---------------------------------------------------------------------------

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

# ---------------------------------------------------------------------------
# qvintage submodule
# ---------------------------------------------------------------------------

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
# Z80 ROM assembler
# ---------------------------------------------------------------------------

find_program(
    ZMAC_EXECUTABLE
    NAMES zmac
    HINTS
        "$ENV{HOME}/bin"
    REQUIRED
)

set(
    SELZ80_ROM_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/roms"
)

set(
    SELROM_SYMBOL_GENERATOR
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_selrom_symbols.py"
)

if(NOT EXISTS "${SELROM_SYMBOL_GENERATOR}")
    message(FATAL_ERROR
        "SELRom symbol generator is missing.\n"
        "Expected:\n"
        "  ${SELROM_SYMBOL_GENERATOR}"
    )
endif()

file(
    GLOB Z80_ASM_FILES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/ROMS/*.z80"
)

if(NOT Z80_ASM_FILES)
    message(FATAL_ERROR
        "No Z80 ROM sources found in:\n"
        "  ${CMAKE_CURRENT_SOURCE_DIR}/ROMS"
    )
endif()

set(
    SELROM_INC
    "${CMAKE_CURRENT_SOURCE_DIR}/ROMS/SELRom.inc"
)

set(
    SELROM_HEADER
    "${CMAKE_CURRENT_SOURCE_DIR}/ROMS/SELRom.h"
)

set(
    SELROM_RESOURCE_BIN
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources/SELRom.bin"
)

set(SELZ80_ROM_OUTPUTS)
set(SELROM_FOUND FALSE)

foreach(Z80_FILE IN LISTS Z80_ASM_FILES)

    get_filename_component(
        Z80_NAME
        "${Z80_FILE}"
        NAME_WE
    )

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

    if(Z80_NAME STREQUAL "SELRom")

        set(SELROM_FOUND TRUE)

        add_custom_command(
            OUTPUT
                "${CIM_FILE}"
                "${LST_FILE}"
                "${BIN_FILE}"
                "${SELROM_RESOURCE_BIN}"
                "${SELROM_INC}"
                "${SELROM_HEADER}"

            COMMAND
                "${CMAKE_COMMAND}" -E make_directory
                "${SELZ80_ROM_DIR}"

            COMMAND
                "${CMAKE_COMMAND}" -E make_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/Resources"

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

            COMMAND
                "${CMAKE_COMMAND}" -E copy_if_different
                "${BIN_FILE}"
                "${SELROM_RESOURCE_BIN}"

            COMMAND
                "${Python3_EXECUTABLE}"
                "${SELROM_SYMBOL_GENERATOR}"
                "${LST_FILE}"
                "${SELROM_INC}"
                "${SELROM_HEADER}"

            DEPENDS
                "${Z80_FILE}"
                "${SELROM_SYMBOL_GENERATOR}"

            COMMENT
                "Assembling SELRom.z80 -> SELRom.bin and generating SELRom.inc/SELRom.h"

            VERBATIM
        )

        list(
            APPEND SELZ80_ROM_OUTPUTS
            "${CIM_FILE}"
            "${LST_FILE}"
            "${BIN_FILE}"
            "${SELROM_RESOURCE_BIN}"
            "${SELROM_INC}"
            "${SELROM_HEADER}"
        )

    else()

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

            COMMENT
                "Assembling ROMS/${Z80_NAME}.z80 -> ${Z80_NAME}.bin"

            VERBATIM
        )

        list(
            APPEND SELZ80_ROM_OUTPUTS
            "${CIM_FILE}"
            "${LST_FILE}"
            "${BIN_FILE}"
        )

    endif()

    set_source_files_properties(
        "${CIM_FILE}"
        "${LST_FILE}"
        "${BIN_FILE}"
        PROPERTIES
            GENERATED TRUE
    )

endforeach()

if(NOT SELROM_FOUND)
    message(FATAL_ERROR
        "ROMS/SELRom.z80 is missing."
    )
endif()

set_source_files_properties(
    ${Z80_ASM_FILES}
    PROPERTIES
        HEADER_FILE_ONLY TRUE
)

set_source_files_properties(
    "${SELROM_RESOURCE_BIN}"
    "${SELROM_INC}"
    "${SELROM_HEADER}"
    PROPERTIES
        GENERATED TRUE
)

set_source_files_properties(
    "${SELROM_INC}"
    "${SELROM_HEADER}"
    PROPERTIES
        HEADER_FILE_ONLY TRUE
)

add_custom_target(
    SELZ80Trainer_roms
    ALL
    DEPENDS
        ${SELZ80_ROM_OUTPUTS}
)

# ---------------------------------------------------------------------------
# SELZ80Trainer executable
# ---------------------------------------------------------------------------

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

target_sources(
    SELZ80Trainer
    PRIVATE
        ${Z80_ASM_FILES}
        "${SELROM_INC}"
        "${SELROM_HEADER}"
)

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
)

target_link_libraries(
    SELZ80Trainer
    PRIVATE
        Qt6::Widgets
        Z80::Z80
        qvintage::Widgets
)

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
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
