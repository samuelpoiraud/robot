cmake_minimum_required(VERSION 2.8)
project(SuperStrat)

#include("toolchain-gcc-arm-embedded.cmake")
set(CMAKE_TOOLCHAIN_FILE "toolchain-gcc-arm-embedded.cmake")

set(CMAKE_VERBOSE_MAKEFILE false)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

get_filename_component(BUILD_DIR_NAME ${CMAKE_BINARY_DIR} NAME)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/${BUILD_DIR_NAME}-bin")

set(SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}")

set(TARGET_NAME "${PROJECT_NAME}.elf")

include_directories("${SOURCE_PATH}/stm32f4xx")
link_directories("${SOURCE_PATH}/stm32f4xx")

# SET(CMAKE_C_FLAGS "-mthumb -mcpu=cortex-m4 -Wall")
# SET(CMAKE_CXX_FLAGS "-mthumb -mcpu=cortex-m4  -Wall")
# SET(CMAKE_ASM_FLAGS "-mthumb -mcpu=cortex-m4")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/QS/stm32f4xx_flash.ld\" -Wl,-Map,\"${EXECUTABLE_OUTPUT_PATH}/${TARGET_NAME}.map\"")

message(${CMAKE_EXE_LINKER_FLAGS})

file(GLOB_RECURSE SRC_LIST "${SOURCE_PATH}/*.c" "${SOURCE_PATH}/*.h")
string(REGEX REPLACE "${CMAKE_BINARY_DIR}/CMakeFiles/[^;]+;?" "" SRC_LIST "${SRC_LIST}")

add_executable(${TARGET_NAME} ${SRC_LIST})
target_link_libraries(${TARGET_NAME} libstm32f4xx.a)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND "${CMAKE_OBJDUMP} -Obinary ${EXECUTABLE_OUTPUT_PATH}/${PROJECT}.elf ${EXECUTABLE_OUTPUT_PATH}/${PROJECT}.bin")