cmake_minimum_required(VERSION 2.8)
set(CMAKE_TOOLCHAIN_FILE "QS/toolchain-gcc-arm-embedded.cmake")

project(Propulsion C ASM)

########################
# Paramètres
########################

set(CMAKE_VERBOSE_MAKEFILE false)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_BUILD_TYPE "Debug")

########################
# Fichiers de sortie
########################

set(TARGET_ELF "${PROJECT_NAME}.elf")
set(TARGET_BIN "${PROJECT_NAME}.bin")
set(TARGET_LST "${PROJECT_NAME}.lst")

########################
# Récup des dossiers de sortie
########################

get_filename_component(BUILD_DIR_NAME ${CMAKE_BINARY_DIR} NAME)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/${BUILD_DIR_NAME}-bin")
set(SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}")

########################
# Flags de compilation
########################

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map,\"${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.map\"")

########################
# Fichiers à compiler
########################


include_directories("${SOURCE_PATH}/provide_parent_dir")
include_directories("${SOURCE_PATH}/stm32f4xx")
link_directories("${SOURCE_PATH}/stm32f4xx")

file(GLOB_RECURSE SRC_LIST FOLLOW_SYMLINKS "${SOURCE_PATH}/*.c" "${SOURCE_PATH}/*.h" "${SOURCE_PATH}/*.S")
string(REGEX REPLACE "${CMAKE_BINARY_DIR}/CMakeFiles/[^;]+;?" "" SRC_LIST "${SRC_LIST}")

########################
# Targets .elf, .bin et .lst
########################

add_executable(${TARGET_ELF} ${SRC_LIST})
target_link_libraries(${TARGET_ELF} libstm32f4xx.a)

add_custom_command(OUTPUT ${EXECUTABLE_OUTPUT_PATH}/${TARGET_BIN} DEPENDS ${TARGET_ELF} COMMAND ${CMAKE_OBJCOPY} "-Obinary" "${EXECUTABLE_OUTPUT_PATH}/${TARGET_ELF}" "${EXECUTABLE_OUTPUT_PATH}/${TARGET_BIN}")
add_custom_target(${TARGET_BIN} ALL DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${TARGET_BIN})

add_custom_command(OUTPUT ${EXECUTABLE_OUTPUT_PATH}/${TARGET_LST} DEPENDS ${TARGET_ELF} COMMAND ${CMAKE_OBJDUMP} "-h" "-S" "${EXECUTABLE_OUTPUT_PATH}/${TARGET_ELF}" > "${EXECUTABLE_OUTPUT_PATH}/${TARGET_LST}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_custom_target(${TARGET_LST} ALL DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${TARGET_LST})
else()
	add_custom_target(${TARGET_LST} DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${TARGET_LST})
endif()

########################
# Target de programmation: program
########################

set(OPENOCD_COMMAND "flash write_image erase \\\"${EXECUTABLE_OUTPUT_PATH}/${TARGET_BIN}\\\" 0x08000000")
separate_arguments(OPENOCD_COMMAND)
add_custom_target(program DEPENDS ${TARGET_BIN} COMMAND openocd -f interface/stlink-v2.cfg -f target/stm32f4x_stlink.cfg -c \"init\" -c \"reset halt\" -c \"${OPENOCD_COMMAND}\" -c \"reset\" -c shutdown)
