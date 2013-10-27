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
set(CMAKE_SYSTEM_PROCESSOR cortex-m4)

CMAKE_FORCE_C_COMPILER(arm-none-eabi-gcc GNU)
CMAKE_FORCE_CXX_COMPILER(arm-none-eabi-g++ GNU)
#set(CMAKE_C_COMPILER       arm-none-eabi-gcc)
#set(CMAKE_CXX_COMPILER     arm-none-eabi-g++)

# Find the target environment prefix..
# First see where gcc is keeping libc.a
execute_process(
  COMMAND ${CMAKE_C_COMPILER} -print-file-name=libc.a
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

SET(CMAKE_C_FLAGS "-mthumb -mcpu=cortex-m4 -Wall" CACHE INTERNAL "c compiler flags")
SET(CMAKE_CXX_FLAGS "-mthumb -mcpu=cortex-m4 -Wall" CACHE INTERNAL "cxx compiler flags")
SET(CMAKE_ASM_FLAGS "-mthumb -mcpu=cortex-m4 -Wall -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")
SET(CMAKE_EXE_LINKER_FLAGS "-nostartfiles -mthumb -mcpu=cortex-m4 -Wall -T\"${CMAKE_CURRENT_LIST_DIR}/stm32f4xx_flash.ld\"" CACHE INTERNAL "exe link flags")

# When we break up long strings in CMake we get semicolon
# separated lists, undo this here...
string(REGEX REPLACE ";" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "")

set(BUILD_SHARED_LIBS OFF)

find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy)
find_program(CMAKE_OBJDUMP arm-none-eabi-objdump)
