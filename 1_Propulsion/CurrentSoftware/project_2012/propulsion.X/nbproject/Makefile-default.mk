#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=cof
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/propulsion.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/propulsion.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/buffer.o ${OBJECTDIR}/_ext/1472/cpld.o ${OBJECTDIR}/_ext/1472/it.o ${OBJECTDIR}/_ext/1472/sequences.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/debug.o ${OBJECTDIR}/_ext/43900113/QS_can.o ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o ${OBJECTDIR}/_ext/43900113/QS_configBits.o ${OBJECTDIR}/_ext/43900113/QS_extern_it.o ${OBJECTDIR}/_ext/43900113/QS_global_vars.o ${OBJECTDIR}/_ext/43900113/QS_ports.o ${OBJECTDIR}/_ext/43900113/QS_pwm.o ${OBJECTDIR}/_ext/43900113/QS_qei.o ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o ${OBJECTDIR}/_ext/43900113/QS_timer.o ${OBJECTDIR}/_ext/43900113/QS_uart.o ${OBJECTDIR}/_ext/43900113/QS_buttons.o ${OBJECTDIR}/_ext/1472/calculator.o ${OBJECTDIR}/_ext/1472/copilot.o ${OBJECTDIR}/_ext/1472/corrector.o ${OBJECTDIR}/_ext/1472/cos_sin.o ${OBJECTDIR}/_ext/1472/encoders.o ${OBJECTDIR}/_ext/1472/motors.o ${OBJECTDIR}/_ext/1472/odometry.o ${OBJECTDIR}/_ext/1472/pilot.o ${OBJECTDIR}/_ext/1472/roadmap.o ${OBJECTDIR}/_ext/1472/secretary.o ${OBJECTDIR}/_ext/1472/supervisor.o ${OBJECTDIR}/_ext/1472/warner.o ${OBJECTDIR}/_ext/1472/joystick.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1472/buffer.o.d ${OBJECTDIR}/_ext/1472/cpld.o.d ${OBJECTDIR}/_ext/1472/it.o.d ${OBJECTDIR}/_ext/1472/sequences.o.d ${OBJECTDIR}/_ext/1472/main.o.d ${OBJECTDIR}/_ext/1472/debug.o.d ${OBJECTDIR}/_ext/43900113/QS_can.o.d ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d ${OBJECTDIR}/_ext/43900113/QS_configBits.o.d ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d ${OBJECTDIR}/_ext/43900113/QS_ports.o.d ${OBJECTDIR}/_ext/43900113/QS_pwm.o.d ${OBJECTDIR}/_ext/43900113/QS_qei.o.d ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.d ${OBJECTDIR}/_ext/43900113/QS_timer.o.d ${OBJECTDIR}/_ext/43900113/QS_uart.o.d ${OBJECTDIR}/_ext/43900113/QS_buttons.o.d ${OBJECTDIR}/_ext/1472/calculator.o.d ${OBJECTDIR}/_ext/1472/copilot.o.d ${OBJECTDIR}/_ext/1472/corrector.o.d ${OBJECTDIR}/_ext/1472/cos_sin.o.d ${OBJECTDIR}/_ext/1472/encoders.o.d ${OBJECTDIR}/_ext/1472/motors.o.d ${OBJECTDIR}/_ext/1472/odometry.o.d ${OBJECTDIR}/_ext/1472/pilot.o.d ${OBJECTDIR}/_ext/1472/roadmap.o.d ${OBJECTDIR}/_ext/1472/secretary.o.d ${OBJECTDIR}/_ext/1472/supervisor.o.d ${OBJECTDIR}/_ext/1472/warner.o.d ${OBJECTDIR}/_ext/1472/joystick.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/buffer.o ${OBJECTDIR}/_ext/1472/cpld.o ${OBJECTDIR}/_ext/1472/it.o ${OBJECTDIR}/_ext/1472/sequences.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/debug.o ${OBJECTDIR}/_ext/43900113/QS_can.o ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o ${OBJECTDIR}/_ext/43900113/QS_configBits.o ${OBJECTDIR}/_ext/43900113/QS_extern_it.o ${OBJECTDIR}/_ext/43900113/QS_global_vars.o ${OBJECTDIR}/_ext/43900113/QS_ports.o ${OBJECTDIR}/_ext/43900113/QS_pwm.o ${OBJECTDIR}/_ext/43900113/QS_qei.o ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o ${OBJECTDIR}/_ext/43900113/QS_timer.o ${OBJECTDIR}/_ext/43900113/QS_uart.o ${OBJECTDIR}/_ext/43900113/QS_buttons.o ${OBJECTDIR}/_ext/1472/calculator.o ${OBJECTDIR}/_ext/1472/copilot.o ${OBJECTDIR}/_ext/1472/corrector.o ${OBJECTDIR}/_ext/1472/cos_sin.o ${OBJECTDIR}/_ext/1472/encoders.o ${OBJECTDIR}/_ext/1472/motors.o ${OBJECTDIR}/_ext/1472/odometry.o ${OBJECTDIR}/_ext/1472/pilot.o ${OBJECTDIR}/_ext/1472/roadmap.o ${OBJECTDIR}/_ext/1472/secretary.o ${OBJECTDIR}/_ext/1472/supervisor.o ${OBJECTDIR}/_ext/1472/warner.o ${OBJECTDIR}/_ext/1472/joystick.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/propulsion.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=30F6010A
MP_LINKER_FILE_OPTION=,-Tp30f6010A.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1472/buffer.o: ../buffer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/buffer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/buffer.o.ok ${OBJECTDIR}/_ext/1472/buffer.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/buffer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/buffer.o.d" -o ${OBJECTDIR}/_ext/1472/buffer.o ../buffer.c    
	
${OBJECTDIR}/_ext/1472/cpld.o: ../cpld.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/cpld.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/cpld.o.ok ${OBJECTDIR}/_ext/1472/cpld.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/cpld.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/cpld.o.d" -o ${OBJECTDIR}/_ext/1472/cpld.o ../cpld.c    
	
${OBJECTDIR}/_ext/1472/it.o: ../it.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/it.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/it.o.ok ${OBJECTDIR}/_ext/1472/it.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/it.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/it.o.d" -o ${OBJECTDIR}/_ext/1472/it.o ../it.c    
	
${OBJECTDIR}/_ext/1472/sequences.o: ../sequences.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/sequences.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/sequences.o.ok ${OBJECTDIR}/_ext/1472/sequences.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/sequences.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/sequences.o.d" -o ${OBJECTDIR}/_ext/1472/sequences.o ../sequences.c    
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.ok ${OBJECTDIR}/_ext/1472/main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d" -o ${OBJECTDIR}/_ext/1472/main.o ../main.c    
	
${OBJECTDIR}/_ext/1472/debug.o: ../debug.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/debug.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/debug.o.ok ${OBJECTDIR}/_ext/1472/debug.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/debug.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/debug.o.d" -o ${OBJECTDIR}/_ext/1472/debug.o ../debug.c    
	
${OBJECTDIR}/_ext/43900113/QS_can.o: ../QS/QS_can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can.o.ok ${OBJECTDIR}/_ext/43900113/QS_can.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can.o ../QS/QS_can.c    
	
${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o: ../QS/QS_can_over_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.ok ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o ../QS/QS_can_over_uart.c    
	
${OBJECTDIR}/_ext/43900113/QS_configBits.o: ../QS/QS_configBits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_configBits.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_configBits.o.ok ${OBJECTDIR}/_ext/43900113/QS_configBits.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_configBits.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_configBits.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_configBits.o ../QS/QS_configBits.c    
	
${OBJECTDIR}/_ext/43900113/QS_extern_it.o: ../QS/QS_extern_it.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.ok ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_extern_it.o ../QS/QS_extern_it.c    
	
${OBJECTDIR}/_ext/43900113/QS_global_vars.o: ../QS/QS_global_vars.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.ok ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_global_vars.o ../QS/QS_global_vars.c    
	
${OBJECTDIR}/_ext/43900113/QS_ports.o: ../QS/QS_ports.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ports.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ports.o.ok ${OBJECTDIR}/_ext/43900113/QS_ports.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_ports.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_ports.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_ports.o ../QS/QS_ports.c    
	
${OBJECTDIR}/_ext/43900113/QS_pwm.o: ../QS/QS_pwm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_pwm.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_pwm.o.ok ${OBJECTDIR}/_ext/43900113/QS_pwm.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_pwm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_pwm.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_pwm.o ../QS/QS_pwm.c    
	
${OBJECTDIR}/_ext/43900113/QS_qei.o: ../QS/QS_qei.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_qei.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_qei.o.ok ${OBJECTDIR}/_ext/43900113/QS_qei.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_qei.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_qei.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_qei.o ../QS/QS_qei.c    
	
${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o: ../QS/QS_qei_on_it.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.ok ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o ../QS/QS_qei_on_it.c    
	
${OBJECTDIR}/_ext/43900113/QS_timer.o: ../QS/QS_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_timer.o.ok ${OBJECTDIR}/_ext/43900113/QS_timer.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_timer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_timer.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_timer.o ../QS/QS_timer.c    
	
${OBJECTDIR}/_ext/43900113/QS_uart.o: ../QS/QS_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_uart.o.ok ${OBJECTDIR}/_ext/43900113/QS_uart.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_uart.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_uart.o ../QS/QS_uart.c    
	
${OBJECTDIR}/_ext/43900113/QS_buttons.o: ../QS/QS_buttons.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_buttons.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_buttons.o.ok ${OBJECTDIR}/_ext/43900113/QS_buttons.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_buttons.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_buttons.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_buttons.o ../QS/QS_buttons.c    
	
${OBJECTDIR}/_ext/1472/calculator.o: ../calculator.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/calculator.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/calculator.o.ok ${OBJECTDIR}/_ext/1472/calculator.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/calculator.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/calculator.o.d" -o ${OBJECTDIR}/_ext/1472/calculator.o ../calculator.c    
	
${OBJECTDIR}/_ext/1472/copilot.o: ../copilot.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/copilot.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/copilot.o.ok ${OBJECTDIR}/_ext/1472/copilot.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/copilot.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/copilot.o.d" -o ${OBJECTDIR}/_ext/1472/copilot.o ../copilot.c    
	
${OBJECTDIR}/_ext/1472/corrector.o: ../corrector.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/corrector.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/corrector.o.ok ${OBJECTDIR}/_ext/1472/corrector.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/corrector.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/corrector.o.d" -o ${OBJECTDIR}/_ext/1472/corrector.o ../corrector.c    
	
${OBJECTDIR}/_ext/1472/cos_sin.o: ../cos_sin.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/cos_sin.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/cos_sin.o.ok ${OBJECTDIR}/_ext/1472/cos_sin.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/cos_sin.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/cos_sin.o.d" -o ${OBJECTDIR}/_ext/1472/cos_sin.o ../cos_sin.c    
	
${OBJECTDIR}/_ext/1472/encoders.o: ../encoders.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/encoders.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/encoders.o.ok ${OBJECTDIR}/_ext/1472/encoders.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/encoders.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/encoders.o.d" -o ${OBJECTDIR}/_ext/1472/encoders.o ../encoders.c    
	
${OBJECTDIR}/_ext/1472/motors.o: ../motors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/motors.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/motors.o.ok ${OBJECTDIR}/_ext/1472/motors.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/motors.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/motors.o.d" -o ${OBJECTDIR}/_ext/1472/motors.o ../motors.c    
	
${OBJECTDIR}/_ext/1472/odometry.o: ../odometry.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/odometry.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/odometry.o.ok ${OBJECTDIR}/_ext/1472/odometry.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/odometry.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/odometry.o.d" -o ${OBJECTDIR}/_ext/1472/odometry.o ../odometry.c    
	
${OBJECTDIR}/_ext/1472/pilot.o: ../pilot.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/pilot.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/pilot.o.ok ${OBJECTDIR}/_ext/1472/pilot.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/pilot.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/pilot.o.d" -o ${OBJECTDIR}/_ext/1472/pilot.o ../pilot.c    
	
${OBJECTDIR}/_ext/1472/roadmap.o: ../roadmap.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/roadmap.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/roadmap.o.ok ${OBJECTDIR}/_ext/1472/roadmap.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/roadmap.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/roadmap.o.d" -o ${OBJECTDIR}/_ext/1472/roadmap.o ../roadmap.c    
	
${OBJECTDIR}/_ext/1472/secretary.o: ../secretary.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/secretary.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/secretary.o.ok ${OBJECTDIR}/_ext/1472/secretary.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/secretary.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/secretary.o.d" -o ${OBJECTDIR}/_ext/1472/secretary.o ../secretary.c    
	
${OBJECTDIR}/_ext/1472/supervisor.o: ../supervisor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/supervisor.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/supervisor.o.ok ${OBJECTDIR}/_ext/1472/supervisor.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/supervisor.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/supervisor.o.d" -o ${OBJECTDIR}/_ext/1472/supervisor.o ../supervisor.c    
	
${OBJECTDIR}/_ext/1472/warner.o: ../warner.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/warner.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/warner.o.ok ${OBJECTDIR}/_ext/1472/warner.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/warner.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/warner.o.d" -o ${OBJECTDIR}/_ext/1472/warner.o ../warner.c    
	
${OBJECTDIR}/_ext/1472/joystick.o: ../joystick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/joystick.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/joystick.o.ok ${OBJECTDIR}/_ext/1472/joystick.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/joystick.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/joystick.o.d" -o ${OBJECTDIR}/_ext/1472/joystick.o ../joystick.c    
	
else
${OBJECTDIR}/_ext/1472/buffer.o: ../buffer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/buffer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/buffer.o.ok ${OBJECTDIR}/_ext/1472/buffer.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/buffer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/buffer.o.d" -o ${OBJECTDIR}/_ext/1472/buffer.o ../buffer.c    
	
${OBJECTDIR}/_ext/1472/cpld.o: ../cpld.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/cpld.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/cpld.o.ok ${OBJECTDIR}/_ext/1472/cpld.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/cpld.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/cpld.o.d" -o ${OBJECTDIR}/_ext/1472/cpld.o ../cpld.c    
	
${OBJECTDIR}/_ext/1472/it.o: ../it.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/it.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/it.o.ok ${OBJECTDIR}/_ext/1472/it.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/it.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/it.o.d" -o ${OBJECTDIR}/_ext/1472/it.o ../it.c    
	
${OBJECTDIR}/_ext/1472/sequences.o: ../sequences.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/sequences.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/sequences.o.ok ${OBJECTDIR}/_ext/1472/sequences.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/sequences.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/sequences.o.d" -o ${OBJECTDIR}/_ext/1472/sequences.o ../sequences.c    
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.ok ${OBJECTDIR}/_ext/1472/main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d" -o ${OBJECTDIR}/_ext/1472/main.o ../main.c    
	
${OBJECTDIR}/_ext/1472/debug.o: ../debug.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/debug.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/debug.o.ok ${OBJECTDIR}/_ext/1472/debug.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/debug.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/debug.o.d" -o ${OBJECTDIR}/_ext/1472/debug.o ../debug.c    
	
${OBJECTDIR}/_ext/43900113/QS_can.o: ../QS/QS_can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can.o.ok ${OBJECTDIR}/_ext/43900113/QS_can.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can.o ../QS/QS_can.c    
	
${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o: ../QS/QS_can_over_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.ok ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o ../QS/QS_can_over_uart.c    
	
${OBJECTDIR}/_ext/43900113/QS_configBits.o: ../QS/QS_configBits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_configBits.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_configBits.o.ok ${OBJECTDIR}/_ext/43900113/QS_configBits.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_configBits.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_configBits.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_configBits.o ../QS/QS_configBits.c    
	
${OBJECTDIR}/_ext/43900113/QS_extern_it.o: ../QS/QS_extern_it.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.ok ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_extern_it.o ../QS/QS_extern_it.c    
	
${OBJECTDIR}/_ext/43900113/QS_global_vars.o: ../QS/QS_global_vars.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.ok ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_global_vars.o ../QS/QS_global_vars.c    
	
${OBJECTDIR}/_ext/43900113/QS_ports.o: ../QS/QS_ports.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ports.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ports.o.ok ${OBJECTDIR}/_ext/43900113/QS_ports.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_ports.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_ports.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_ports.o ../QS/QS_ports.c    
	
${OBJECTDIR}/_ext/43900113/QS_pwm.o: ../QS/QS_pwm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_pwm.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_pwm.o.ok ${OBJECTDIR}/_ext/43900113/QS_pwm.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_pwm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_pwm.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_pwm.o ../QS/QS_pwm.c    
	
${OBJECTDIR}/_ext/43900113/QS_qei.o: ../QS/QS_qei.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_qei.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_qei.o.ok ${OBJECTDIR}/_ext/43900113/QS_qei.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_qei.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_qei.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_qei.o ../QS/QS_qei.c    
	
${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o: ../QS/QS_qei_on_it.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.ok ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o ../QS/QS_qei_on_it.c    
	
${OBJECTDIR}/_ext/43900113/QS_timer.o: ../QS/QS_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_timer.o.ok ${OBJECTDIR}/_ext/43900113/QS_timer.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_timer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_timer.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_timer.o ../QS/QS_timer.c    
	
${OBJECTDIR}/_ext/43900113/QS_uart.o: ../QS/QS_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_uart.o.ok ${OBJECTDIR}/_ext/43900113/QS_uart.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_uart.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_uart.o ../QS/QS_uart.c    
	
${OBJECTDIR}/_ext/43900113/QS_buttons.o: ../QS/QS_buttons.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_buttons.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_buttons.o.ok ${OBJECTDIR}/_ext/43900113/QS_buttons.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_buttons.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_buttons.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_buttons.o ../QS/QS_buttons.c    
	
${OBJECTDIR}/_ext/1472/calculator.o: ../calculator.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/calculator.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/calculator.o.ok ${OBJECTDIR}/_ext/1472/calculator.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/calculator.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/calculator.o.d" -o ${OBJECTDIR}/_ext/1472/calculator.o ../calculator.c    
	
${OBJECTDIR}/_ext/1472/copilot.o: ../copilot.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/copilot.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/copilot.o.ok ${OBJECTDIR}/_ext/1472/copilot.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/copilot.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/copilot.o.d" -o ${OBJECTDIR}/_ext/1472/copilot.o ../copilot.c    
	
${OBJECTDIR}/_ext/1472/corrector.o: ../corrector.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/corrector.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/corrector.o.ok ${OBJECTDIR}/_ext/1472/corrector.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/corrector.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/corrector.o.d" -o ${OBJECTDIR}/_ext/1472/corrector.o ../corrector.c    
	
${OBJECTDIR}/_ext/1472/cos_sin.o: ../cos_sin.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/cos_sin.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/cos_sin.o.ok ${OBJECTDIR}/_ext/1472/cos_sin.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/cos_sin.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/cos_sin.o.d" -o ${OBJECTDIR}/_ext/1472/cos_sin.o ../cos_sin.c    
	
${OBJECTDIR}/_ext/1472/encoders.o: ../encoders.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/encoders.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/encoders.o.ok ${OBJECTDIR}/_ext/1472/encoders.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/encoders.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/encoders.o.d" -o ${OBJECTDIR}/_ext/1472/encoders.o ../encoders.c    
	
${OBJECTDIR}/_ext/1472/motors.o: ../motors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/motors.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/motors.o.ok ${OBJECTDIR}/_ext/1472/motors.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/motors.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/motors.o.d" -o ${OBJECTDIR}/_ext/1472/motors.o ../motors.c    
	
${OBJECTDIR}/_ext/1472/odometry.o: ../odometry.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/odometry.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/odometry.o.ok ${OBJECTDIR}/_ext/1472/odometry.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/odometry.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/odometry.o.d" -o ${OBJECTDIR}/_ext/1472/odometry.o ../odometry.c    
	
${OBJECTDIR}/_ext/1472/pilot.o: ../pilot.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/pilot.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/pilot.o.ok ${OBJECTDIR}/_ext/1472/pilot.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/pilot.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/pilot.o.d" -o ${OBJECTDIR}/_ext/1472/pilot.o ../pilot.c    
	
${OBJECTDIR}/_ext/1472/roadmap.o: ../roadmap.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/roadmap.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/roadmap.o.ok ${OBJECTDIR}/_ext/1472/roadmap.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/roadmap.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/roadmap.o.d" -o ${OBJECTDIR}/_ext/1472/roadmap.o ../roadmap.c    
	
${OBJECTDIR}/_ext/1472/secretary.o: ../secretary.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/secretary.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/secretary.o.ok ${OBJECTDIR}/_ext/1472/secretary.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/secretary.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/secretary.o.d" -o ${OBJECTDIR}/_ext/1472/secretary.o ../secretary.c    
	
${OBJECTDIR}/_ext/1472/supervisor.o: ../supervisor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/supervisor.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/supervisor.o.ok ${OBJECTDIR}/_ext/1472/supervisor.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/supervisor.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/supervisor.o.d" -o ${OBJECTDIR}/_ext/1472/supervisor.o ../supervisor.c    
	
${OBJECTDIR}/_ext/1472/warner.o: ../warner.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/warner.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/warner.o.ok ${OBJECTDIR}/_ext/1472/warner.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/warner.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/warner.o.d" -o ${OBJECTDIR}/_ext/1472/warner.o ../warner.c    
	
${OBJECTDIR}/_ext/1472/joystick.o: ../joystick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/joystick.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/joystick.o.ok ${OBJECTDIR}/_ext/1472/joystick.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/joystick.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/joystick.o.d" -o ${OBJECTDIR}/_ext/1472/joystick.o ../joystick.c    
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/propulsion.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=coff -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/propulsion.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,--heap=256,-L"../../../../../../Program Files/C30/lib",-Map="${DISTDIR}/propulsion.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1
else
dist/${CND_CONF}/${IMAGE_TYPE}/propulsion.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=coff -mcpu=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/propulsion.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,--heap=256,-L"../../../../../../Program Files/C30/lib",-Map="${DISTDIR}/propulsion.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION)
	${MP_CC_DIR}\\pic30-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/propulsion.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -omf=coff
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
