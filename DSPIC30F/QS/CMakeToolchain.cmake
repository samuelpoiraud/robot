# CMake Toolchain file for the gcc-arm-embedded toolchain.
# https://launchpad.net/gcc-arm-embedded
#
# Copyright (c) 2013 Swift Navigation Inc.
# Contact: Fergus Noble <fergus@swift-nav.com>
#
# This source is subject to the license found in the file 'LICENSE' which must
# be be distributed together with this source. All other rights reserved.
#
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.

include(CMakeForceCompiler)

# Targeting an embedded system, no OS.
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR 30F6010A)

CMAKE_FORCE_C_COMPILER(pic30-gcc GNU)
CMAKE_FORCE_CXX_COMPILER(pic30-g++ GNU)

# Find the target environment prefix..
# First see where gcc is keeping libc-elf.a
execute_process(
  COMMAND ${CMAKE_C_COMPILER} -print-file-name=libc-elf.a
  OUTPUT_VARIABLE CMAKE_INSTALL_PREFIX
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Strip the filename off
get_filename_component( CMAKE_INSTALL_PREFIX
  "${CMAKE_INSTALL_PREFIX}" PATH
)
# Then find the canonical path to the directory one up from there
get_filename_component(CMAKE_INSTALL_PREFIX
  "${CMAKE_INSTALL_PREFIX}/.." REALPATH
)
set(CMAKE_INSTALL_PREFIX  ${CMAKE_INSTALL_PREFIX} CACHE FILEPATH
    "Install path prefix, prepended onto install directories.")

message(STATUS "Toolchain prefix: ${CMAKE_INSTALL_PREFIX}")

set(CMAKE_FIND_ROOT_PATH  ${CMAKE_INSTALL_PREFIX})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(CMAKE_C_FLAGS "-omf=elf -mcpu=30F6010A -Wall" CACHE INTERNAL "c compiler flags")
SET(CMAKE_CXX_FLAGS "-omf=elf -mcpu=30F6010A -Wall" CACHE INTERNAL "cxx compiler flags")
SET(CMAKE_ASM_FLAGS "-omf=elf -mcpu=30F6010A -Wall -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")
SET(CMAKE_EXE_LINKER_FLAGS "-omf=elf -Wall -Wl,--defsym=__MPLAB_BUILD=1" CACHE INTERNAL "exe link flags")

# When we break up long strings in CMake we get semicolon
# separated lists, undo this here...
string(REGEX REPLACE ";" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "")

set(BUILD_SHARED_LIBS OFF)

find_program(CMAKE_OBJDUMP pic30-objdump)
set(CMAKE_OBJDUMP_ARG "-omf=elf")
find_program(CMAKE_BIN2HEX pic30-bin2hex)

macro(configure_mcu_target TARGET_NAME SRC_LIST)
    set(TARGET_ELF "${TARGET_NAME}.elf")
    set(TARGET_BIN "${TARGET_NAME}.bin")
    set(TARGET_LST "${TARGET_NAME}.lst")
    set(TARGET_HEX "${TARGET_NAME}.hex")

    get_filename_component(BUILD_DIR_NAME ${CMAKE_BINARY_DIR} NAME)
    set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/${BUILD_DIR_NAME}-bin")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map,\"${EXECUTABLE_OUTPUT_PATH}/${TARGET_NAME}.map\"")

    add_executable(${TARGET_ELF} ${SRC_LIST})
    # ugly workaround,  "-Wl,-Tp30F6010A.gld" must be at the end of the command line...
    target_link_libraries(${TARGET_ELF} -Wl,-Tp30F6010A.gld)

    add_custom_command(OUTPUT ${EXECUTABLE_OUTPUT_PATH}/${TARGET_HEX} DEPENDS ${TARGET_ELF} COMMAND ${CMAKE_BIN2HEX} "-omf=elf" "${EXECUTABLE_OUTPUT_PATH}/${TARGET_ELF}")
    add_custom_target(${TARGET_HEX} ALL DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${TARGET_HEX})


    # fichier lst (listing asm)
    add_custom_command(OUTPUT ${EXECUTABLE_OUTPUT_PATH}/${TARGET_LST} DEPENDS ${TARGET_ELF} COMMAND ${CMAKE_OBJDUMP} "-omf=elf" "-h" "-S" "${EXECUTABLE_OUTPUT_PATH}/${TARGET_ELF}" > "${EXECUTABLE_OUTPUT_PATH}/${TARGET_LST}")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            add_custom_target(${TARGET_LST} ALL DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${TARGET_LST})
    else()
            add_custom_target(${TARGET_LST} DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${TARGET_LST})
    endif()
endmacro()
