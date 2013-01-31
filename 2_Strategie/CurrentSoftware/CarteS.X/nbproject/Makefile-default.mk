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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/CarteS.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/CarteS.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/Stacks.o ${OBJECTDIR}/_ext/1472/Generic_functions.o ${OBJECTDIR}/_ext/1472/Asser_functions.o ${OBJECTDIR}/_ext/1472/brain.o ${OBJECTDIR}/_ext/1472/environment.o ${OBJECTDIR}/_ext/1472/clock.o ${OBJECTDIR}/_ext/1472/Pathfind.o ${OBJECTDIR}/_ext/1472/actions_tests.o ${OBJECTDIR}/_ext/1472/Geometry.o ${OBJECTDIR}/_ext/1472/maths_home.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/sick.o ${OBJECTDIR}/_ext/1472/act_functions.o ${OBJECTDIR}/_ext/1472/button.o ${OBJECTDIR}/_ext/1472/detection.o ${OBJECTDIR}/_ext/1472/telemeter.o ${OBJECTDIR}/_ext/1472/avoidance.o ${OBJECTDIR}/_ext/1472/can_utils.o ${OBJECTDIR}/_ext/43900113/QS_adc.o ${OBJECTDIR}/_ext/43900113/QS_can.o ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o ${OBJECTDIR}/_ext/43900113/QS_configBits.o ${OBJECTDIR}/_ext/43900113/QS_global_vars.o ${OBJECTDIR}/_ext/43900113/QS_ports.o ${OBJECTDIR}/_ext/43900113/QS_timer.o ${OBJECTDIR}/_ext/43900113/QS_uart.o ${OBJECTDIR}/_ext/1472/polygon.o ${OBJECTDIR}/_ext/43900113/QS_buttons.o ${OBJECTDIR}/_ext/1472/actions.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1472/Stacks.o.d ${OBJECTDIR}/_ext/1472/Generic_functions.o.d ${OBJECTDIR}/_ext/1472/Asser_functions.o.d ${OBJECTDIR}/_ext/1472/brain.o.d ${OBJECTDIR}/_ext/1472/environment.o.d ${OBJECTDIR}/_ext/1472/clock.o.d ${OBJECTDIR}/_ext/1472/Pathfind.o.d ${OBJECTDIR}/_ext/1472/actions_tests.o.d ${OBJECTDIR}/_ext/1472/Geometry.o.d ${OBJECTDIR}/_ext/1472/maths_home.o.d ${OBJECTDIR}/_ext/1472/main.o.d ${OBJECTDIR}/_ext/1472/sick.o.d ${OBJECTDIR}/_ext/1472/act_functions.o.d ${OBJECTDIR}/_ext/1472/button.o.d ${OBJECTDIR}/_ext/1472/detection.o.d ${OBJECTDIR}/_ext/1472/telemeter.o.d ${OBJECTDIR}/_ext/1472/avoidance.o.d ${OBJECTDIR}/_ext/1472/can_utils.o.d ${OBJECTDIR}/_ext/43900113/QS_adc.o.d ${OBJECTDIR}/_ext/43900113/QS_can.o.d ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d ${OBJECTDIR}/_ext/43900113/QS_configBits.o.d ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d ${OBJECTDIR}/_ext/43900113/QS_ports.o.d ${OBJECTDIR}/_ext/43900113/QS_timer.o.d ${OBJECTDIR}/_ext/43900113/QS_uart.o.d ${OBJECTDIR}/_ext/1472/polygon.o.d ${OBJECTDIR}/_ext/43900113/QS_buttons.o.d ${OBJECTDIR}/_ext/1472/actions.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/Stacks.o ${OBJECTDIR}/_ext/1472/Generic_functions.o ${OBJECTDIR}/_ext/1472/Asser_functions.o ${OBJECTDIR}/_ext/1472/brain.o ${OBJECTDIR}/_ext/1472/environment.o ${OBJECTDIR}/_ext/1472/clock.o ${OBJECTDIR}/_ext/1472/Pathfind.o ${OBJECTDIR}/_ext/1472/actions_tests.o ${OBJECTDIR}/_ext/1472/Geometry.o ${OBJECTDIR}/_ext/1472/maths_home.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/sick.o ${OBJECTDIR}/_ext/1472/act_functions.o ${OBJECTDIR}/_ext/1472/button.o ${OBJECTDIR}/_ext/1472/detection.o ${OBJECTDIR}/_ext/1472/telemeter.o ${OBJECTDIR}/_ext/1472/avoidance.o ${OBJECTDIR}/_ext/1472/can_utils.o ${OBJECTDIR}/_ext/43900113/QS_adc.o ${OBJECTDIR}/_ext/43900113/QS_can.o ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o ${OBJECTDIR}/_ext/43900113/QS_configBits.o ${OBJECTDIR}/_ext/43900113/QS_global_vars.o ${OBJECTDIR}/_ext/43900113/QS_ports.o ${OBJECTDIR}/_ext/43900113/QS_timer.o ${OBJECTDIR}/_ext/43900113/QS_uart.o ${OBJECTDIR}/_ext/1472/polygon.o ${OBJECTDIR}/_ext/43900113/QS_buttons.o ${OBJECTDIR}/_ext/1472/actions.o


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
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/CarteS.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

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
${OBJECTDIR}/_ext/1472/Stacks.o: ../Stacks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Stacks.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Stacks.o.ok ${OBJECTDIR}/_ext/1472/Stacks.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Stacks.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/Stacks.o.d" -o ${OBJECTDIR}/_ext/1472/Stacks.o ../Stacks.c    
	
${OBJECTDIR}/_ext/1472/Generic_functions.o: ../Generic_functions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Generic_functions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Generic_functions.o.ok ${OBJECTDIR}/_ext/1472/Generic_functions.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Generic_functions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/Generic_functions.o.d" -o ${OBJECTDIR}/_ext/1472/Generic_functions.o ../Generic_functions.c    
	
${OBJECTDIR}/_ext/1472/Asser_functions.o: ../Asser_functions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Asser_functions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Asser_functions.o.ok ${OBJECTDIR}/_ext/1472/Asser_functions.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Asser_functions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/Asser_functions.o.d" -o ${OBJECTDIR}/_ext/1472/Asser_functions.o ../Asser_functions.c    
	
${OBJECTDIR}/_ext/1472/brain.o: ../brain.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/brain.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/brain.o.ok ${OBJECTDIR}/_ext/1472/brain.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/brain.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/brain.o.d" -o ${OBJECTDIR}/_ext/1472/brain.o ../brain.c    
	
${OBJECTDIR}/_ext/1472/environment.o: ../environment.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/environment.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/environment.o.ok ${OBJECTDIR}/_ext/1472/environment.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/environment.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/environment.o.d" -o ${OBJECTDIR}/_ext/1472/environment.o ../environment.c    
	
${OBJECTDIR}/_ext/1472/clock.o: ../clock.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/clock.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/clock.o.ok ${OBJECTDIR}/_ext/1472/clock.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/clock.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/clock.o.d" -o ${OBJECTDIR}/_ext/1472/clock.o ../clock.c    
	
${OBJECTDIR}/_ext/1472/Pathfind.o: ../Pathfind.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Pathfind.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Pathfind.o.ok ${OBJECTDIR}/_ext/1472/Pathfind.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Pathfind.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/Pathfind.o.d" -o ${OBJECTDIR}/_ext/1472/Pathfind.o ../Pathfind.c    
	
${OBJECTDIR}/_ext/1472/actions_tests.o: ../actions_tests.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/actions_tests.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/actions_tests.o.ok ${OBJECTDIR}/_ext/1472/actions_tests.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/actions_tests.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/actions_tests.o.d" -o ${OBJECTDIR}/_ext/1472/actions_tests.o ../actions_tests.c    
	
${OBJECTDIR}/_ext/1472/Geometry.o: ../Geometry.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Geometry.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Geometry.o.ok ${OBJECTDIR}/_ext/1472/Geometry.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Geometry.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/Geometry.o.d" -o ${OBJECTDIR}/_ext/1472/Geometry.o ../Geometry.c    
	
${OBJECTDIR}/_ext/1472/maths_home.o: ../maths_home.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/maths_home.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/maths_home.o.ok ${OBJECTDIR}/_ext/1472/maths_home.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/maths_home.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/maths_home.o.d" -o ${OBJECTDIR}/_ext/1472/maths_home.o ../maths_home.c    
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.ok ${OBJECTDIR}/_ext/1472/main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d" -o ${OBJECTDIR}/_ext/1472/main.o ../main.c    
	
${OBJECTDIR}/_ext/1472/sick.o: ../sick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/sick.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/sick.o.ok ${OBJECTDIR}/_ext/1472/sick.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/sick.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/sick.o.d" -o ${OBJECTDIR}/_ext/1472/sick.o ../sick.c    
	
${OBJECTDIR}/_ext/1472/act_functions.o: ../act_functions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/act_functions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/act_functions.o.ok ${OBJECTDIR}/_ext/1472/act_functions.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/act_functions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/act_functions.o.d" -o ${OBJECTDIR}/_ext/1472/act_functions.o ../act_functions.c    
	
${OBJECTDIR}/_ext/1472/button.o: ../button.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/button.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/button.o.ok ${OBJECTDIR}/_ext/1472/button.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/button.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/button.o.d" -o ${OBJECTDIR}/_ext/1472/button.o ../button.c    
	
${OBJECTDIR}/_ext/1472/detection.o: ../detection.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/detection.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/detection.o.ok ${OBJECTDIR}/_ext/1472/detection.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/detection.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/detection.o.d" -o ${OBJECTDIR}/_ext/1472/detection.o ../detection.c    
	
${OBJECTDIR}/_ext/1472/telemeter.o: ../telemeter.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/telemeter.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/telemeter.o.ok ${OBJECTDIR}/_ext/1472/telemeter.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/telemeter.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/telemeter.o.d" -o ${OBJECTDIR}/_ext/1472/telemeter.o ../telemeter.c    
	
${OBJECTDIR}/_ext/1472/avoidance.o: ../avoidance.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/avoidance.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/avoidance.o.ok ${OBJECTDIR}/_ext/1472/avoidance.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/avoidance.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/avoidance.o.d" -o ${OBJECTDIR}/_ext/1472/avoidance.o ../avoidance.c    
	
${OBJECTDIR}/_ext/1472/can_utils.o: ../can_utils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/can_utils.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/can_utils.o.ok ${OBJECTDIR}/_ext/1472/can_utils.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/can_utils.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/can_utils.o.d" -o ${OBJECTDIR}/_ext/1472/can_utils.o ../can_utils.c    
	
${OBJECTDIR}/_ext/43900113/QS_adc.o: ../QS/QS_adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_adc.o.ok ${OBJECTDIR}/_ext/43900113/QS_adc.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_adc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_adc.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_adc.o ../QS/QS_adc.c    
	
${OBJECTDIR}/_ext/43900113/QS_can.o: ../QS/QS_can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can.o.ok ${OBJECTDIR}/_ext/43900113/QS_can.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can.o ../QS/QS_can.c    
	
${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o: ../QS/QS_can_over_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.ok ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o ../QS/QS_can_over_uart.c    
	
${OBJECTDIR}/_ext/43900113/QS_configBits.o: ../QS/QS_configBits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_configBits.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_configBits.o.ok ${OBJECTDIR}/_ext/43900113/QS_configBits.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_configBits.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_configBits.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_configBits.o ../QS/QS_configBits.c    
	
${OBJECTDIR}/_ext/43900113/QS_global_vars.o: ../QS/QS_global_vars.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.ok ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_global_vars.o ../QS/QS_global_vars.c    
	
${OBJECTDIR}/_ext/43900113/QS_ports.o: ../QS/QS_ports.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ports.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ports.o.ok ${OBJECTDIR}/_ext/43900113/QS_ports.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_ports.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_ports.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_ports.o ../QS/QS_ports.c    
	
${OBJECTDIR}/_ext/43900113/QS_timer.o: ../QS/QS_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_timer.o.ok ${OBJECTDIR}/_ext/43900113/QS_timer.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_timer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_timer.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_timer.o ../QS/QS_timer.c    
	
${OBJECTDIR}/_ext/43900113/QS_uart.o: ../QS/QS_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_uart.o.ok ${OBJECTDIR}/_ext/43900113/QS_uart.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_uart.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_uart.o ../QS/QS_uart.c    
	
${OBJECTDIR}/_ext/1472/polygon.o: ../polygon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/polygon.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/polygon.o.ok ${OBJECTDIR}/_ext/1472/polygon.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/polygon.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/polygon.o.d" -o ${OBJECTDIR}/_ext/1472/polygon.o ../polygon.c    
	
${OBJECTDIR}/_ext/43900113/QS_buttons.o: ../QS/QS_buttons.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_buttons.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_buttons.o.ok ${OBJECTDIR}/_ext/43900113/QS_buttons.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_buttons.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_buttons.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_buttons.o ../QS/QS_buttons.c    
	
${OBJECTDIR}/_ext/1472/actions.o: ../actions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/actions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/actions.o.ok ${OBJECTDIR}/_ext/1472/actions.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/actions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/actions.o.d" -o ${OBJECTDIR}/_ext/1472/actions.o ../actions.c    
	
else
${OBJECTDIR}/_ext/1472/Stacks.o: ../Stacks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Stacks.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Stacks.o.ok ${OBJECTDIR}/_ext/1472/Stacks.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Stacks.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/Stacks.o.d" -o ${OBJECTDIR}/_ext/1472/Stacks.o ../Stacks.c    
	
${OBJECTDIR}/_ext/1472/Generic_functions.o: ../Generic_functions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Generic_functions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Generic_functions.o.ok ${OBJECTDIR}/_ext/1472/Generic_functions.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Generic_functions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/Generic_functions.o.d" -o ${OBJECTDIR}/_ext/1472/Generic_functions.o ../Generic_functions.c    
	
${OBJECTDIR}/_ext/1472/Asser_functions.o: ../Asser_functions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Asser_functions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Asser_functions.o.ok ${OBJECTDIR}/_ext/1472/Asser_functions.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Asser_functions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/Asser_functions.o.d" -o ${OBJECTDIR}/_ext/1472/Asser_functions.o ../Asser_functions.c    
	
${OBJECTDIR}/_ext/1472/brain.o: ../brain.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/brain.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/brain.o.ok ${OBJECTDIR}/_ext/1472/brain.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/brain.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/brain.o.d" -o ${OBJECTDIR}/_ext/1472/brain.o ../brain.c    
	
${OBJECTDIR}/_ext/1472/environment.o: ../environment.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/environment.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/environment.o.ok ${OBJECTDIR}/_ext/1472/environment.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/environment.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/environment.o.d" -o ${OBJECTDIR}/_ext/1472/environment.o ../environment.c    
	
${OBJECTDIR}/_ext/1472/clock.o: ../clock.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/clock.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/clock.o.ok ${OBJECTDIR}/_ext/1472/clock.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/clock.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/clock.o.d" -o ${OBJECTDIR}/_ext/1472/clock.o ../clock.c    
	
${OBJECTDIR}/_ext/1472/Pathfind.o: ../Pathfind.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Pathfind.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Pathfind.o.ok ${OBJECTDIR}/_ext/1472/Pathfind.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Pathfind.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/Pathfind.o.d" -o ${OBJECTDIR}/_ext/1472/Pathfind.o ../Pathfind.c    
	
${OBJECTDIR}/_ext/1472/actions_tests.o: ../actions_tests.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/actions_tests.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/actions_tests.o.ok ${OBJECTDIR}/_ext/1472/actions_tests.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/actions_tests.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/actions_tests.o.d" -o ${OBJECTDIR}/_ext/1472/actions_tests.o ../actions_tests.c    
	
${OBJECTDIR}/_ext/1472/Geometry.o: ../Geometry.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Geometry.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Geometry.o.ok ${OBJECTDIR}/_ext/1472/Geometry.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Geometry.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/Geometry.o.d" -o ${OBJECTDIR}/_ext/1472/Geometry.o ../Geometry.c    
	
${OBJECTDIR}/_ext/1472/maths_home.o: ../maths_home.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/maths_home.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/maths_home.o.ok ${OBJECTDIR}/_ext/1472/maths_home.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/maths_home.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/maths_home.o.d" -o ${OBJECTDIR}/_ext/1472/maths_home.o ../maths_home.c    
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.ok ${OBJECTDIR}/_ext/1472/main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d" -o ${OBJECTDIR}/_ext/1472/main.o ../main.c    
	
${OBJECTDIR}/_ext/1472/sick.o: ../sick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/sick.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/sick.o.ok ${OBJECTDIR}/_ext/1472/sick.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/sick.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/sick.o.d" -o ${OBJECTDIR}/_ext/1472/sick.o ../sick.c    
	
${OBJECTDIR}/_ext/1472/act_functions.o: ../act_functions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/act_functions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/act_functions.o.ok ${OBJECTDIR}/_ext/1472/act_functions.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/act_functions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/act_functions.o.d" -o ${OBJECTDIR}/_ext/1472/act_functions.o ../act_functions.c    
	
${OBJECTDIR}/_ext/1472/button.o: ../button.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/button.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/button.o.ok ${OBJECTDIR}/_ext/1472/button.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/button.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/button.o.d" -o ${OBJECTDIR}/_ext/1472/button.o ../button.c    
	
${OBJECTDIR}/_ext/1472/detection.o: ../detection.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/detection.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/detection.o.ok ${OBJECTDIR}/_ext/1472/detection.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/detection.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/detection.o.d" -o ${OBJECTDIR}/_ext/1472/detection.o ../detection.c    
	
${OBJECTDIR}/_ext/1472/telemeter.o: ../telemeter.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/telemeter.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/telemeter.o.ok ${OBJECTDIR}/_ext/1472/telemeter.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/telemeter.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/telemeter.o.d" -o ${OBJECTDIR}/_ext/1472/telemeter.o ../telemeter.c    
	
${OBJECTDIR}/_ext/1472/avoidance.o: ../avoidance.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/avoidance.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/avoidance.o.ok ${OBJECTDIR}/_ext/1472/avoidance.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/avoidance.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/avoidance.o.d" -o ${OBJECTDIR}/_ext/1472/avoidance.o ../avoidance.c    
	
${OBJECTDIR}/_ext/1472/can_utils.o: ../can_utils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/can_utils.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/can_utils.o.ok ${OBJECTDIR}/_ext/1472/can_utils.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/can_utils.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/can_utils.o.d" -o ${OBJECTDIR}/_ext/1472/can_utils.o ../can_utils.c    
	
${OBJECTDIR}/_ext/43900113/QS_adc.o: ../QS/QS_adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_adc.o.ok ${OBJECTDIR}/_ext/43900113/QS_adc.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_adc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_adc.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_adc.o ../QS/QS_adc.c    
	
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
	
${OBJECTDIR}/_ext/1472/polygon.o: ../polygon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/polygon.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/polygon.o.ok ${OBJECTDIR}/_ext/1472/polygon.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/polygon.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/polygon.o.d" -o ${OBJECTDIR}/_ext/1472/polygon.o ../polygon.c    
	
${OBJECTDIR}/_ext/43900113/QS_buttons.o: ../QS/QS_buttons.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_buttons.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_buttons.o.ok ${OBJECTDIR}/_ext/43900113/QS_buttons.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_buttons.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_buttons.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_buttons.o ../QS/QS_buttons.c    
	
${OBJECTDIR}/_ext/1472/actions.o: ../actions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/actions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/actions.o.ok ${OBJECTDIR}/_ext/1472/actions.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/actions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -mlarge-code -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/actions.o.d" -o ${OBJECTDIR}/_ext/1472/actions.o ../actions.c    
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/CarteS.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../QS/libp30F6010A-coff.a 
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=coff -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/CarteS.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}   ..\QS\libp30F6010A-coff.a      -Wl,--defsym=__MPLAB_BUILD=1,--heap=256,-L"../../../../../../../../Program Files/Microchip/MPLAB C30/lib",-Map="${DISTDIR}/CarteS.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PICKIT2=1
else
dist/${CND_CONF}/${IMAGE_TYPE}/CarteS.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../QS/libp30F6010A-coff.a 
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=coff -mcpu=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/CarteS.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}   ..\QS\libp30F6010A-coff.a      -Wl,--defsym=__MPLAB_BUILD=1,--heap=256,-L"../../../../../../../../Program Files/Microchip/MPLAB C30/lib",-Map="${DISTDIR}/CarteS.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION)
	${MP_CC_DIR}\\pic30-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/CarteS.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -omf=coff
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
