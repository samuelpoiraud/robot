cmake_minimum_required(VERSION 2.8)
set(CMAKE_TOOLCHAIN_FILE "QS/CMakeToolchain.cmake")

project(Actionneur C ASM)

########################
# Paramètres
########################

set(CMAKE_VERBOSE_MAKEFILE false)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_BUILD_TYPE "Debug")
#add_definitions(-DPREDEF_BUILD_KRUSTY)
#add_definitions(-DPREDEF_BUILD_TINY)

########################
# Fichiers de sortie
########################

set(TARGET_ELF "${PROJECT_NAME}.elf")
set(TARGET_BIN "${PROJECT_NAME}.bin")
set(TARGET_LST "${PROJECT_NAME}.lst")
set(TARGET_HEX "${PROJECT_NAME}.hex")

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
include_directories("${SOURCE_PATH}/config")

file(GLOB_RECURSE SRC_LIST FOLLOW_SYMLINKS "${SOURCE_PATH}/*.c" "${SOURCE_PATH}/*.h" "${SOURCE_PATH}/*.S")
string(REGEX REPLACE "${CMAKE_BINARY_DIR}/CMakeFiles/[^;]+;?" "" SRC_LIST "${SRC_LIST}")

########################
# Targets .elf, .bin et .lst
########################

add_executable(${TARGET_ELF} ${SRC_LIST})

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m4)
	include_directories("${SOURCE_PATH}/stm32f4xx")
	target_link_libraries(${TARGET_ELF} "${SOURCE_PATH}/stm32f4xx/libstm32f4xx.a")
endif()

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL 30F6010A)
	# ugly workaround,  "-Wl,-Tp30F6010A.gld" must be at the end of the command line...
	target_link_libraries(${TARGET_ELF} -Wl,-Tp30F6010A.gld)
endif()

# elf to bin/hex (utilisé pour programmer sur la cible)
if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m4)
	add_custom_command(OUTPUT ${EXECUTABLE_OUTPUT_PATH}/${TARGET_BIN} DEPENDS ${TARGET_ELF} COMMAND ${CMAKE_OBJCOPY} "-Obinary" "${EXECUTABLE_OUTPUT_PATH}/${TARGET_ELF}" "${EXECUTABLE_OUTPUT_PATH}/${TARGET_BIN}")
	add_custom_target(${TARGET_BIN} ALL DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${TARGET_BIN})
endif()

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL 30F6010A)
	add_custom_command(OUTPUT ${EXECUTABLE_OUTPUT_PATH}/${TARGET_HEX} DEPENDS ${TARGET_ELF} COMMAND ${CMAKE_BIN2HEX} -omf=elf "${EXECUTABLE_OUTPUT_PATH}/${TARGET_ELF}")
	add_custom_target(${TARGET_HEX} ALL DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${TARGET_HEX})
endif()

# fichier lst (listing asm)
add_custom_command(OUTPUT ${EXECUTABLE_OUTPUT_PATH}/${TARGET_LST} DEPENDS ${TARGET_ELF} COMMAND ${CMAKE_OBJDUMP} "-h" "-S" "${EXECUTABLE_OUTPUT_PATH}/${TARGET_ELF}" > "${EXECUTABLE_OUTPUT_PATH}/${TARGET_LST}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_custom_target(${TARGET_LST} ALL DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${TARGET_LST})
else()
	add_custom_target(${TARGET_LST} DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${TARGET_LST})
endif()

########################
# Target de programmation: program
########################

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m4)
	set(OPENOCD_COMMAND "flash write_image erase \\\"${EXECUTABLE_OUTPUT_PATH}/${TARGET_BIN}\\\" 0x08000000")
	separate_arguments(OPENOCD_COMMAND)
	add_custom_target(program DEPENDS ${TARGET_BIN} COMMAND openocd.exe -f interface/stlink-v2.cfg -f target/stm32f4x_stlink.cfg -c \"init\" -c \"reset halt\" -c \"${OPENOCD_COMMAND}\" -c \"reset\" -c shutdown)
endif()
