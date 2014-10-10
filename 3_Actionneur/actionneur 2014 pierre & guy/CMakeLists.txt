cmake_minimum_required(VERSION 2.8)
set(CMAKE_TOOLCHAIN_FILE "QS/CMakeToolchain.cmake")

project(Actionneur C ASM)

########################
# Paramètres
########################

#Pour changer un mode, ajouter -DCMAKE_BUILD_TYPE=Debug par exemple au arguments de cmake
#(les arguments de cmake sont dans: bouton droit sur le projet => Executer cmake)

set(CMAKE_VERBOSE_MAKEFILE false CACHE BOOL "Print compilation commands")
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type (one of Debug, Release, RelWithDebInfo, MinSizeRel)")
message("Using ${CMAKE_BUILD_TYPE} mode")

# si actif, équivaut à un #define pour tous les fichiers compilés
#add_definitions(-DPREDEF_BUILD_BIG)
#add_definitions(-DPREDEF_BUILD_SMALL)

########################
# Fichiers à compiler
########################

include_directories("${CMAKE_CURRENT_LIST_DIR}/provide_parent_dir")
include_directories("${CMAKE_CURRENT_LIST_DIR}/config")

file(GLOB_RECURSE SRC_LIST FOLLOW_SYMLINKS "*.c" "*.h" "*.S")
string(REGEX REPLACE "${CMAKE_BINARY_DIR}/CMakeFiles/[^;]+;?" "" SRC_LIST "${SRC_LIST}")


########################
# Creation des fichiers de sortie:
# - Si la QS est celle du pic, création de binaire pour le pic
# - Si c'est la QS du stm32, création des binaires pour le stm32 + target program pour programmer la carte
########################

configure_mcu_target("${PROJECT_NAME}" "${SRC_LIST}")
