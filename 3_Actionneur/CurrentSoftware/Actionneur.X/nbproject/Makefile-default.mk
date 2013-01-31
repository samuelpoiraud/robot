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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Actionneur.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Actionneur.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/Can_msg_processing.o ${OBJECTDIR}/_ext/1472/clock.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/maths_home.o ${OBJECTDIR}/_ext/43900113/QS_adc.o ${OBJECTDIR}/_ext/43900113/QS_ax12.o ${OBJECTDIR}/_ext/43900113/QS_buttons.o ${OBJECTDIR}/_ext/43900113/QS_can.o ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o ${OBJECTDIR}/_ext/43900113/QS_DCMotor.o ${OBJECTDIR}/_ext/43900113/QS_extern_it.o ${OBJECTDIR}/_ext/43900113/QS_global_vars.o ${OBJECTDIR}/_ext/43900113/QS_ports.o ${OBJECTDIR}/_ext/43900113/QS_pwm.o ${OBJECTDIR}/_ext/43900113/QS_qei.o ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o ${OBJECTDIR}/_ext/43900113/QS_servo.o ${OBJECTDIR}/_ext/43900113/QS_Step_motor.o ${OBJECTDIR}/_ext/43900113/QS_timer.o ${OBJECTDIR}/_ext/43900113/QS_uart.o ${OBJECTDIR}/_ext/43900113/QS_watchdog.o ${OBJECTDIR}/_ext/1472/queue.o ${OBJECTDIR}/_ext/43900113/QS_configBits.o ${OBJECTDIR}/_ext/1472/Ball_grabber.o ${OBJECTDIR}/_ext/1472/Hammer.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1472/Can_msg_processing.o.d ${OBJECTDIR}/_ext/1472/clock.o.d ${OBJECTDIR}/_ext/1472/main.o.d ${OBJECTDIR}/_ext/1472/maths_home.o.d ${OBJECTDIR}/_ext/43900113/QS_adc.o.d ${OBJECTDIR}/_ext/43900113/QS_ax12.o.d ${OBJECTDIR}/_ext/43900113/QS_buttons.o.d ${OBJECTDIR}/_ext/43900113/QS_can.o.d ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d ${OBJECTDIR}/_ext/43900113/QS_DCMotor.o.d ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d ${OBJECTDIR}/_ext/43900113/QS_ports.o.d ${OBJECTDIR}/_ext/43900113/QS_pwm.o.d ${OBJECTDIR}/_ext/43900113/QS_qei.o.d ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.d ${OBJECTDIR}/_ext/43900113/QS_servo.o.d ${OBJECTDIR}/_ext/43900113/QS_Step_motor.o.d ${OBJECTDIR}/_ext/43900113/QS_timer.o.d ${OBJECTDIR}/_ext/43900113/QS_uart.o.d ${OBJECTDIR}/_ext/43900113/QS_watchdog.o.d ${OBJECTDIR}/_ext/1472/queue.o.d ${OBJECTDIR}/_ext/43900113/QS_configBits.o.d ${OBJECTDIR}/_ext/1472/Ball_grabber.o.d ${OBJECTDIR}/_ext/1472/Hammer.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/Can_msg_processing.o ${OBJECTDIR}/_ext/1472/clock.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/maths_home.o ${OBJECTDIR}/_ext/43900113/QS_adc.o ${OBJECTDIR}/_ext/43900113/QS_ax12.o ${OBJECTDIR}/_ext/43900113/QS_buttons.o ${OBJECTDIR}/_ext/43900113/QS_can.o ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o ${OBJECTDIR}/_ext/43900113/QS_DCMotor.o ${OBJECTDIR}/_ext/43900113/QS_extern_it.o ${OBJECTDIR}/_ext/43900113/QS_global_vars.o ${OBJECTDIR}/_ext/43900113/QS_ports.o ${OBJECTDIR}/_ext/43900113/QS_pwm.o ${OBJECTDIR}/_ext/43900113/QS_qei.o ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o ${OBJECTDIR}/_ext/43900113/QS_servo.o ${OBJECTDIR}/_ext/43900113/QS_Step_motor.o ${OBJECTDIR}/_ext/43900113/QS_timer.o ${OBJECTDIR}/_ext/43900113/QS_uart.o ${OBJECTDIR}/_ext/43900113/QS_watchdog.o ${OBJECTDIR}/_ext/1472/queue.o ${OBJECTDIR}/_ext/43900113/QS_configBits.o ${OBJECTDIR}/_ext/1472/Ball_grabber.o ${OBJECTDIR}/_ext/1472/Hammer.o


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
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/Actionneur.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=30F6010A
MP_LINKER_FILE_OPTION=,-Tp30F6010A.gld
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
${OBJECTDIR}/_ext/1472/Can_msg_processing.o: ../Can_msg_processing.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Can_msg_processing.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Can_msg_processing.o.ok ${OBJECTDIR}/_ext/1472/Can_msg_processing.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Can_msg_processing.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Can_msg_processing.o.d" -o ${OBJECTDIR}/_ext/1472/Can_msg_processing.o ../Can_msg_processing.c    
	
${OBJECTDIR}/_ext/1472/clock.o: ../clock.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/clock.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/clock.o.ok ${OBJECTDIR}/_ext/1472/clock.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/clock.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/clock.o.d" -o ${OBJECTDIR}/_ext/1472/clock.o ../clock.c    
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.ok ${OBJECTDIR}/_ext/1472/main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d" -o ${OBJECTDIR}/_ext/1472/main.o ../main.c    
	
${OBJECTDIR}/_ext/1472/maths_home.o: ../maths_home.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/maths_home.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/maths_home.o.ok ${OBJECTDIR}/_ext/1472/maths_home.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/maths_home.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/maths_home.o.d" -o ${OBJECTDIR}/_ext/1472/maths_home.o ../maths_home.c    
	
${OBJECTDIR}/_ext/43900113/QS_adc.o: ../QS/QS_adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_adc.o.ok ${OBJECTDIR}/_ext/43900113/QS_adc.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_adc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_adc.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_adc.o ../QS/QS_adc.c    
	
${OBJECTDIR}/_ext/43900113/QS_ax12.o: ../QS/QS_ax12.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ax12.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ax12.o.ok ${OBJECTDIR}/_ext/43900113/QS_ax12.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_ax12.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_ax12.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_ax12.o ../QS/QS_ax12.c    
	
${OBJECTDIR}/_ext/43900113/QS_buttons.o: ../QS/QS_buttons.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_buttons.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_buttons.o.ok ${OBJECTDIR}/_ext/43900113/QS_buttons.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_buttons.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_buttons.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_buttons.o ../QS/QS_buttons.c    
	
${OBJECTDIR}/_ext/43900113/QS_can.o: ../QS/QS_can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can.o.ok ${OBJECTDIR}/_ext/43900113/QS_can.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can.o ../QS/QS_can.c    
	
${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o: ../QS/QS_can_over_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.ok ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o ../QS/QS_can_over_uart.c    
	
${OBJECTDIR}/_ext/43900113/QS_DCMotor.o: ../QS/QS_DCMotor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_DCMotor.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_DCMotor.o.ok ${OBJECTDIR}/_ext/43900113/QS_DCMotor.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_DCMotor.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_DCMotor.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_DCMotor.o ../QS/QS_DCMotor.c    
	
${OBJECTDIR}/_ext/43900113/QS_extern_it.o: ../QS/QS_extern_it.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.ok ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_extern_it.o ../QS/QS_extern_it.c    
	
${OBJECTDIR}/_ext/43900113/QS_global_vars.o: ../QS/QS_global_vars.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.ok ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_global_vars.o ../QS/QS_global_vars.c    
	
${OBJECTDIR}/_ext/43900113/QS_ports.o: ../QS/QS_ports.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ports.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ports.o.ok ${OBJECTDIR}/_ext/43900113/QS_ports.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_ports.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_ports.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_ports.o ../QS/QS_ports.c    
	
${OBJECTDIR}/_ext/43900113/QS_pwm.o: ../QS/QS_pwm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_pwm.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_pwm.o.ok ${OBJECTDIR}/_ext/43900113/QS_pwm.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_pwm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_pwm.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_pwm.o ../QS/QS_pwm.c    
	
${OBJECTDIR}/_ext/43900113/QS_qei.o: ../QS/QS_qei.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_qei.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_qei.o.ok ${OBJECTDIR}/_ext/43900113/QS_qei.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_qei.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_qei.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_qei.o ../QS/QS_qei.c    
	
${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o: ../QS/QS_qei_on_it.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.ok ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o ../QS/QS_qei_on_it.c    
	
${OBJECTDIR}/_ext/43900113/QS_servo.o: ../QS/QS_servo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_servo.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_servo.o.ok ${OBJECTDIR}/_ext/43900113/QS_servo.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_servo.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_servo.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_servo.o ../QS/QS_servo.c    
	
${OBJECTDIR}/_ext/43900113/QS_Step_motor.o: ../QS/QS_Step_motor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_Step_motor.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_Step_motor.o.ok ${OBJECTDIR}/_ext/43900113/QS_Step_motor.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_Step_motor.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_Step_motor.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_Step_motor.o ../QS/QS_Step_motor.c    
	
${OBJECTDIR}/_ext/43900113/QS_timer.o: ../QS/QS_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_timer.o.ok ${OBJECTDIR}/_ext/43900113/QS_timer.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_timer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_timer.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_timer.o ../QS/QS_timer.c    
	
${OBJECTDIR}/_ext/43900113/QS_uart.o: ../QS/QS_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_uart.o.ok ${OBJECTDIR}/_ext/43900113/QS_uart.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_uart.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_uart.o ../QS/QS_uart.c    
	
${OBJECTDIR}/_ext/43900113/QS_watchdog.o: ../QS/QS_watchdog.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_watchdog.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_watchdog.o.ok ${OBJECTDIR}/_ext/43900113/QS_watchdog.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_watchdog.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_watchdog.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_watchdog.o ../QS/QS_watchdog.c    
	
${OBJECTDIR}/_ext/1472/queue.o: ../queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/queue.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/queue.o.ok ${OBJECTDIR}/_ext/1472/queue.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/queue.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/queue.o.d" -o ${OBJECTDIR}/_ext/1472/queue.o ../queue.c    
	
${OBJECTDIR}/_ext/43900113/QS_configBits.o: ../QS/QS_configBits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_configBits.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_configBits.o.ok ${OBJECTDIR}/_ext/43900113/QS_configBits.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_configBits.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_configBits.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_configBits.o ../QS/QS_configBits.c    
	
${OBJECTDIR}/_ext/1472/Ball_grabber.o: ../Ball_grabber.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Ball_grabber.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Ball_grabber.o.ok ${OBJECTDIR}/_ext/1472/Ball_grabber.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Ball_grabber.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Ball_grabber.o.d" -o ${OBJECTDIR}/_ext/1472/Ball_grabber.o ../Ball_grabber.c    
	
${OBJECTDIR}/_ext/1472/Hammer.o: ../Hammer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Hammer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Hammer.o.ok ${OBJECTDIR}/_ext/1472/Hammer.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Hammer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Hammer.o.d" -o ${OBJECTDIR}/_ext/1472/Hammer.o ../Hammer.c    
	
else
${OBJECTDIR}/_ext/1472/Can_msg_processing.o: ../Can_msg_processing.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Can_msg_processing.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Can_msg_processing.o.ok ${OBJECTDIR}/_ext/1472/Can_msg_processing.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Can_msg_processing.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Can_msg_processing.o.d" -o ${OBJECTDIR}/_ext/1472/Can_msg_processing.o ../Can_msg_processing.c    
	
${OBJECTDIR}/_ext/1472/clock.o: ../clock.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/clock.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/clock.o.ok ${OBJECTDIR}/_ext/1472/clock.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/clock.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/clock.o.d" -o ${OBJECTDIR}/_ext/1472/clock.o ../clock.c    
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.ok ${OBJECTDIR}/_ext/1472/main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d" -o ${OBJECTDIR}/_ext/1472/main.o ../main.c    
	
${OBJECTDIR}/_ext/1472/maths_home.o: ../maths_home.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/maths_home.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/maths_home.o.ok ${OBJECTDIR}/_ext/1472/maths_home.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/maths_home.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/maths_home.o.d" -o ${OBJECTDIR}/_ext/1472/maths_home.o ../maths_home.c    
	
${OBJECTDIR}/_ext/43900113/QS_adc.o: ../QS/QS_adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_adc.o.ok ${OBJECTDIR}/_ext/43900113/QS_adc.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_adc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_adc.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_adc.o ../QS/QS_adc.c    
	
${OBJECTDIR}/_ext/43900113/QS_ax12.o: ../QS/QS_ax12.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ax12.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ax12.o.ok ${OBJECTDIR}/_ext/43900113/QS_ax12.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_ax12.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_ax12.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_ax12.o ../QS/QS_ax12.c    
	
${OBJECTDIR}/_ext/43900113/QS_buttons.o: ../QS/QS_buttons.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_buttons.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_buttons.o.ok ${OBJECTDIR}/_ext/43900113/QS_buttons.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_buttons.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_buttons.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_buttons.o ../QS/QS_buttons.c    
	
${OBJECTDIR}/_ext/43900113/QS_can.o: ../QS/QS_can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can.o.ok ${OBJECTDIR}/_ext/43900113/QS_can.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can.o ../QS/QS_can.c    
	
${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o: ../QS/QS_can_over_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.ok ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o ../QS/QS_can_over_uart.c    
	
${OBJECTDIR}/_ext/43900113/QS_DCMotor.o: ../QS/QS_DCMotor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_DCMotor.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_DCMotor.o.ok ${OBJECTDIR}/_ext/43900113/QS_DCMotor.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_DCMotor.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_DCMotor.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_DCMotor.o ../QS/QS_DCMotor.c    
	
${OBJECTDIR}/_ext/43900113/QS_extern_it.o: ../QS/QS_extern_it.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.ok ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_extern_it.o ../QS/QS_extern_it.c    
	
${OBJECTDIR}/_ext/43900113/QS_global_vars.o: ../QS/QS_global_vars.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.ok ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_global_vars.o ../QS/QS_global_vars.c    
	
${OBJECTDIR}/_ext/43900113/QS_ports.o: ../QS/QS_ports.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ports.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ports.o.ok ${OBJECTDIR}/_ext/43900113/QS_ports.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_ports.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_ports.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_ports.o ../QS/QS_ports.c    
	
${OBJECTDIR}/_ext/43900113/QS_pwm.o: ../QS/QS_pwm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_pwm.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_pwm.o.ok ${OBJECTDIR}/_ext/43900113/QS_pwm.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_pwm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_pwm.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_pwm.o ../QS/QS_pwm.c    
	
${OBJECTDIR}/_ext/43900113/QS_qei.o: ../QS/QS_qei.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_qei.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_qei.o.ok ${OBJECTDIR}/_ext/43900113/QS_qei.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_qei.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_qei.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_qei.o ../QS/QS_qei.c    
	
${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o: ../QS/QS_qei_on_it.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.ok ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_qei_on_it.o ../QS/QS_qei_on_it.c    
	
${OBJECTDIR}/_ext/43900113/QS_servo.o: ../QS/QS_servo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_servo.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_servo.o.ok ${OBJECTDIR}/_ext/43900113/QS_servo.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_servo.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_servo.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_servo.o ../QS/QS_servo.c    
	
${OBJECTDIR}/_ext/43900113/QS_Step_motor.o: ../QS/QS_Step_motor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_Step_motor.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_Step_motor.o.ok ${OBJECTDIR}/_ext/43900113/QS_Step_motor.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_Step_motor.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_Step_motor.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_Step_motor.o ../QS/QS_Step_motor.c    
	
${OBJECTDIR}/_ext/43900113/QS_timer.o: ../QS/QS_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_timer.o.ok ${OBJECTDIR}/_ext/43900113/QS_timer.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_timer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_timer.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_timer.o ../QS/QS_timer.c    
	
${OBJECTDIR}/_ext/43900113/QS_uart.o: ../QS/QS_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_uart.o.ok ${OBJECTDIR}/_ext/43900113/QS_uart.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_uart.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_uart.o ../QS/QS_uart.c    
	
${OBJECTDIR}/_ext/43900113/QS_watchdog.o: ../QS/QS_watchdog.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_watchdog.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_watchdog.o.ok ${OBJECTDIR}/_ext/43900113/QS_watchdog.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_watchdog.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_watchdog.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_watchdog.o ../QS/QS_watchdog.c    
	
${OBJECTDIR}/_ext/1472/queue.o: ../queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/queue.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/queue.o.ok ${OBJECTDIR}/_ext/1472/queue.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/queue.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/queue.o.d" -o ${OBJECTDIR}/_ext/1472/queue.o ../queue.c    
	
${OBJECTDIR}/_ext/43900113/QS_configBits.o: ../QS/QS_configBits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_configBits.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_configBits.o.ok ${OBJECTDIR}/_ext/43900113/QS_configBits.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_configBits.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_configBits.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_configBits.o ../QS/QS_configBits.c    
	
${OBJECTDIR}/_ext/1472/Ball_grabber.o: ../Ball_grabber.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Ball_grabber.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Ball_grabber.o.ok ${OBJECTDIR}/_ext/1472/Ball_grabber.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Ball_grabber.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Ball_grabber.o.d" -o ${OBJECTDIR}/_ext/1472/Ball_grabber.o ../Ball_grabber.c    
	
${OBJECTDIR}/_ext/1472/Hammer.o: ../Hammer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Hammer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Hammer.o.ok ${OBJECTDIR}/_ext/1472/Hammer.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Hammer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Hammer.o.d" -o ${OBJECTDIR}/_ext/1472/Hammer.o ../Hammer.c    
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/Actionneur.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../QS/libp30F6010A-coff.a 
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=coff -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/Actionneur.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}   ..\QS\libp30F6010A-coff.a      -Wl,--defsym=__MPLAB_BUILD=1,--heap=256,-L"..",-Map="${DISTDIR}/Actionneur.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PICKIT2=1
else
dist/${CND_CONF}/${IMAGE_TYPE}/Actionneur.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../QS/libp30F6010A-coff.a 
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=coff -mcpu=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Actionneur.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}   ..\QS\libp30F6010A-coff.a      -Wl,--defsym=__MPLAB_BUILD=1,--heap=256,-L"..",-Map="${DISTDIR}/Actionneur.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION)
	${MP_CC_DIR}\\pic30-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/Actionneur.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -omf=coff
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
