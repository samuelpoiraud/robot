cmake_minimum_required(VERSION 2.8)
set(CMAKE_TOOLCHAIN_FILE "CMakeToolchain.cmake")

project(stm32f4xx C ASM)

########################
# Paramètres
########################

set(CMAKE_VERBOSE_MAKEFILE true)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_BUILD_TYPE "RelWithDebInfo")

########################
# Fichiers de sortie
########################

set(TARGET_LIB "${PROJECT_NAME}-${CMAKE_BUILD_TYPE}")

########################
# Récup des dossiers de sortie
########################

get_filename_component(BUILD_DIR_NAME ${CMAKE_BINARY_DIR} NAME)

set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/${BUILD_DIR_NAME}-bin")
set(SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}")

########################
# Flags de compilation
########################

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdata-sections -ffunction-sections")

########################
# Fichiers à compiler
########################

include_directories("${SOURCE_PATH}/stm32f4xx")

file(GLOB SRC_LIST_ROOT "${SOURCE_PATH}/*.c" "${SOURCE_PATH}/*.h" "${SOURCE_PATH}/*.S")
file(GLOB SRC_LIST_STM32F4XX "${SOURCE_PATH}/stm32f4xx/*.h")

set(SRC_LIST ${SRC_LIST_ROOT} ${SRC_LIST_STM32F4XX})

########################
# Outils de convertion des binaires
########################

set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)

########################
# Target lib
########################

add_library(${TARGET_LIB} STATIC ${SRC_LIST})
