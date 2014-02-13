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

macro(configure_mcu_target TARGET_NAME SRC_LIST)
    set(TARGET_ELF "${TARGET_NAME}.elf")
    set(TARGET_BIN "${TARGET_NAME}.bin")
    set(TARGET_LST "${TARGET_NAME}.lst")
    set(TARGET_HEX "${TARGET_NAME}.hex")

    get_filename_component(BUILD_DIR_NAME ${CMAKE_BINARY_DIR} NAME)
    set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/${BUILD_DIR_NAME}-bin")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map,\"${EXECUTABLE_OUTPUT_PATH}/${TARGET_NAME}.map\"")

    add_executable(${TARGET_ELF} ${SRC_LIST})

    include_directories("${CMAKE_CURRENT_LIST_DIR}/stm32f4xx")
    target_link_libraries(${TARGET_ELF} "${CMAKE_CURRENT_LIST_DIR}/stm32f4xx/libstm32f4xx.a")

    # elf to bin/hex (utilisé pour programmer sur la cible)
    add_custom_command(OUTPUT ${EXECUTABLE_OUTPUT_PATH}/${TARGET_BIN} DEPENDS ${TARGET_ELF} COMMAND ${CMAKE_OBJCOPY} "-Obinary" "${EXECUTABLE_OUTPUT_PATH}/${TARGET_ELF}" "${EXECUTABLE_OUTPUT_PATH}/${TARGET_BIN}")
    add_custom_target(${TARGET_BIN} ALL DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${TARGET_BIN})

    # fichier lst (listing asm)
    add_custom_command(OUTPUT ${EXECUTABLE_OUTPUT_PATH}/${TARGET_LST} DEPENDS ${TARGET_ELF} COMMAND ${CMAKE_OBJDUMP} "-h" "-S" "${EXECUTABLE_OUTPUT_PATH}/${TARGET_ELF}" > "${EXECUTABLE_OUTPUT_PATH}/${TARGET_LST}")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            add_custom_target(${TARGET_LST} ALL DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${TARGET_LST})
    else()
            add_custom_target(${TARGET_LST} DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${TARGET_LST})
    endif()

    # Target de programmation: program
    set(OPENOCD_COMMAND "flash write_image erase \\\"${EXECUTABLE_OUTPUT_PATH}/${TARGET_BIN}\\\" 0x08000000")
    separate_arguments(OPENOCD_COMMAND)
    add_custom_target(program DEPENDS ${TARGET_BIN} COMMAND openocd.exe -f interface/stlink-v2.cfg -f target/stm32f4x_stlink.cfg -c \"init\" -c \"reset halt\" -c \"${OPENOCD_COMMAND}\" -c \"reset\" -c shutdown)
endmacro()
