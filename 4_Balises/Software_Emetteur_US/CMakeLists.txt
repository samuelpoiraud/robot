cmake_minimum_required(VERSION 2.8)
set(CMAKE_TOOLCHAIN_FILE "QS/CMakeToolchain.cmake")

project(CarteBaliseEmetteurUSIR C)

#include("QS/FindPCHSupport.cmake")

########################
# Paramètres
########################

#Pour changer un mode, ajotuer -DCMAKE_BUILD_TYPE=Debug par exemple au arguments de cmake

set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_VERBOSE_MAKEFILE false CACHE BOOL "Print compilation commands")
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type (one of Debug, Release, RelWithDebInfo, MinSizeRel)")
message("Using ${CMAKE_BUILD_TYPE} mode")


########################
# Sources
########################

include_directories("${CMAKE_CURRENT_LIST_DIR}/provide_parent_dir")
include_directories("${CMAKE_CURRENT_LIST_DIR}/config")

set(SRC_LIST
#Sources
	main.c
	EmissionUS.c
	EmissionIR.c
	motor.c
	Synchro.c
	SynchroRF.c

	QS/QS_configBits.c
	QS/QS_ports.c
	QS/QS_pwm.c
	QS/QS_timer.c
	QS/QS_uart.c
	QS/QS_extern_it.c
	QS/QS_global_vars.c
	QS/QS_qei.c
	QS/QS_rf.c
	QS/QS_buffer_fifo.c
	QS/QS_watchdog.c
	QS/QS_outputlog.c
	QS/impl/QS_uart_dspic30f.c

#Headers
	Global_config.h
	Global_vars_types.h
	Global_vars.h
	main.h
	EmissionUS.h
	EmissionIR.h
	motor.h
	Synchro.h
	SynchroRF.h

	QS/p30f6010A.h
	QS/QS_all.h
	QS/QS_config_doc.h
	QS/QS_configCheck.h
	QS/QS_global_vars.h
	QS/QS_macro.h
	QS/QS_ports.h
	QS/QS_pwm.h
	QS/QS_timer.h
	QS/QS_types.h
	QS/QS_uart.h
	QS/QS_extern_it.h
	QS/QS_qei.h
	QS/QS_rf.h
	QS/QS_buffer_fifo.h
	QS/QS_watchdog.h
	QS/QS_outputlog.h
	QS/impl/QS_uart_impl.h
)


########################
# Creation des fichiers de sortie:
# - Si la QS est celle du pic, création de binaire pour le pic
# - Si c'est la QS du stm32, création des binaires pour le stm32 + target program pour programmer la carte
########################

configure_mcu_target("${PROJECT_NAME}" "${SRC_LIST}")
