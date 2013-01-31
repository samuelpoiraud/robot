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
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Balises_Project.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Balises_Project.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/Tableau.o ${OBJECTDIR}/_ext/1472/Beacon.o ${OBJECTDIR}/_ext/1472/Beacon_it.o ${OBJECTDIR}/_ext/43900113/QS_adc.o ${OBJECTDIR}/_ext/43900113/QS_buttons.o ${OBJECTDIR}/_ext/43900113/QS_can.o ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o ${OBJECTDIR}/_ext/43900113/QS_configBits.o ${OBJECTDIR}/_ext/43900113/QS_global_vars.o ${OBJECTDIR}/_ext/43900113/QS_ports.o ${OBJECTDIR}/_ext/43900113/QS_timer.o ${OBJECTDIR}/_ext/43900113/QS_uart.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/43900113/QS_extern_it.o ${OBJECTDIR}/_ext/43900113/QS_can_over_xbee.o ${OBJECTDIR}/_ext/1472/Secretary.o ${OBJECTDIR}/_ext/1472/Brain_US.o ${OBJECTDIR}/_ext/1472/Array_US.o ${OBJECTDIR}/_ext/1472/Eyes.o ${OBJECTDIR}/_ext/1472/Brain_IR.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1472/Tableau.o.d ${OBJECTDIR}/_ext/1472/Beacon.o.d ${OBJECTDIR}/_ext/1472/Beacon_it.o.d ${OBJECTDIR}/_ext/43900113/QS_adc.o.d ${OBJECTDIR}/_ext/43900113/QS_buttons.o.d ${OBJECTDIR}/_ext/43900113/QS_can.o.d ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d ${OBJECTDIR}/_ext/43900113/QS_configBits.o.d ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d ${OBJECTDIR}/_ext/43900113/QS_ports.o.d ${OBJECTDIR}/_ext/43900113/QS_timer.o.d ${OBJECTDIR}/_ext/43900113/QS_uart.o.d ${OBJECTDIR}/_ext/1472/main.o.d ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d ${OBJECTDIR}/_ext/43900113/QS_can_over_xbee.o.d ${OBJECTDIR}/_ext/1472/Secretary.o.d ${OBJECTDIR}/_ext/1472/Brain_US.o.d ${OBJECTDIR}/_ext/1472/Array_US.o.d ${OBJECTDIR}/_ext/1472/Eyes.o.d ${OBJECTDIR}/_ext/1472/Brain_IR.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/Tableau.o ${OBJECTDIR}/_ext/1472/Beacon.o ${OBJECTDIR}/_ext/1472/Beacon_it.o ${OBJECTDIR}/_ext/43900113/QS_adc.o ${OBJECTDIR}/_ext/43900113/QS_buttons.o ${OBJECTDIR}/_ext/43900113/QS_can.o ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o ${OBJECTDIR}/_ext/43900113/QS_configBits.o ${OBJECTDIR}/_ext/43900113/QS_global_vars.o ${OBJECTDIR}/_ext/43900113/QS_ports.o ${OBJECTDIR}/_ext/43900113/QS_timer.o ${OBJECTDIR}/_ext/43900113/QS_uart.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/43900113/QS_extern_it.o ${OBJECTDIR}/_ext/43900113/QS_can_over_xbee.o ${OBJECTDIR}/_ext/1472/Secretary.o ${OBJECTDIR}/_ext/1472/Brain_US.o ${OBJECTDIR}/_ext/1472/Array_US.o ${OBJECTDIR}/_ext/1472/Eyes.o ${OBJECTDIR}/_ext/1472/Brain_IR.o


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

# The following macros may be used in the pre and post step lines
Device=dsPIC30F6010A
ProjectDir="M:\___ROBOT\_Codes 2013\balise\Software_Mere\Balises_Project.X"
ConfName=default
ImagePath="dist\default\${IMAGE_TYPE}\Balises_Project.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}"
ImageDir="dist\default\${IMAGE_TYPE}"
ImageName="Balises_Project.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}"

.build-conf:  .pre ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/Balises_Project.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
	@echo "--------------------------------------"
	@echo "User defined post-build step: []"
	@
	@echo "--------------------------------------"

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
${OBJECTDIR}/_ext/1472/Tableau.o: ../Tableau.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Tableau.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Tableau.o.ok ${OBJECTDIR}/_ext/1472/Tableau.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Tableau.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Tableau.o.d" -o ${OBJECTDIR}/_ext/1472/Tableau.o ../Tableau.c  
	
${OBJECTDIR}/_ext/1472/Beacon.o: ../Beacon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Beacon.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Beacon.o.ok ${OBJECTDIR}/_ext/1472/Beacon.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Beacon.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Beacon.o.d" -o ${OBJECTDIR}/_ext/1472/Beacon.o ../Beacon.c  
	
${OBJECTDIR}/_ext/1472/Beacon_it.o: ../Beacon_it.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Beacon_it.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Beacon_it.o.ok ${OBJECTDIR}/_ext/1472/Beacon_it.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Beacon_it.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Beacon_it.o.d" -o ${OBJECTDIR}/_ext/1472/Beacon_it.o ../Beacon_it.c  
	
${OBJECTDIR}/_ext/43900113/QS_adc.o: ../QS/QS_adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_adc.o.ok ${OBJECTDIR}/_ext/43900113/QS_adc.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_adc.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_adc.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_adc.o ../QS/QS_adc.c  
	
${OBJECTDIR}/_ext/43900113/QS_buttons.o: ../QS/QS_buttons.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_buttons.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_buttons.o.ok ${OBJECTDIR}/_ext/43900113/QS_buttons.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_buttons.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_buttons.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_buttons.o ../QS/QS_buttons.c  
	
${OBJECTDIR}/_ext/43900113/QS_can.o: ../QS/QS_can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can.o.ok ${OBJECTDIR}/_ext/43900113/QS_can.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can.o ../QS/QS_can.c  
	
${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o: ../QS/QS_can_over_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.ok ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o ../QS/QS_can_over_uart.c  
	
${OBJECTDIR}/_ext/43900113/QS_configBits.o: ../QS/QS_configBits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_configBits.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_configBits.o.ok ${OBJECTDIR}/_ext/43900113/QS_configBits.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_configBits.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_configBits.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_configBits.o ../QS/QS_configBits.c  
	
${OBJECTDIR}/_ext/43900113/QS_global_vars.o: ../QS/QS_global_vars.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.ok ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_global_vars.o ../QS/QS_global_vars.c  
	
${OBJECTDIR}/_ext/43900113/QS_ports.o: ../QS/QS_ports.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ports.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ports.o.ok ${OBJECTDIR}/_ext/43900113/QS_ports.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_ports.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_ports.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_ports.o ../QS/QS_ports.c  
	
${OBJECTDIR}/_ext/43900113/QS_timer.o: ../QS/QS_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_timer.o.ok ${OBJECTDIR}/_ext/43900113/QS_timer.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_timer.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_timer.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_timer.o ../QS/QS_timer.c  
	
${OBJECTDIR}/_ext/43900113/QS_uart.o: ../QS/QS_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_uart.o.ok ${OBJECTDIR}/_ext/43900113/QS_uart.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_uart.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_uart.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_uart.o ../QS/QS_uart.c  
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.ok ${OBJECTDIR}/_ext/1472/main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d" -o ${OBJECTDIR}/_ext/1472/main.o ../main.c  
	
${OBJECTDIR}/_ext/43900113/QS_extern_it.o: ../QS/QS_extern_it.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.ok ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_extern_it.o ../QS/QS_extern_it.c  
	
${OBJECTDIR}/_ext/43900113/QS_can_over_xbee.o: ../QS/QS_can_over_xbee.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_xbee.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_xbee.o.ok ${OBJECTDIR}/_ext/43900113/QS_can_over_xbee.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can_over_xbee.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can_over_xbee.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can_over_xbee.o ../QS/QS_can_over_xbee.c  
	
${OBJECTDIR}/_ext/1472/Secretary.o: ../Secretary.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Secretary.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Secretary.o.ok ${OBJECTDIR}/_ext/1472/Secretary.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Secretary.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Secretary.o.d" -o ${OBJECTDIR}/_ext/1472/Secretary.o ../Secretary.c  
	
${OBJECTDIR}/_ext/1472/Brain_US.o: ../Brain_US.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Brain_US.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Brain_US.o.ok ${OBJECTDIR}/_ext/1472/Brain_US.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Brain_US.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Brain_US.o.d" -o ${OBJECTDIR}/_ext/1472/Brain_US.o ../Brain_US.c  
	
${OBJECTDIR}/_ext/1472/Array_US.o: ../Array_US.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Array_US.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Array_US.o.ok ${OBJECTDIR}/_ext/1472/Array_US.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Array_US.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Array_US.o.d" -o ${OBJECTDIR}/_ext/1472/Array_US.o ../Array_US.c  
	
${OBJECTDIR}/_ext/1472/Eyes.o: ../Eyes.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Eyes.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Eyes.o.ok ${OBJECTDIR}/_ext/1472/Eyes.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Eyes.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Eyes.o.d" -o ${OBJECTDIR}/_ext/1472/Eyes.o ../Eyes.c  
	
${OBJECTDIR}/_ext/1472/Brain_IR.o: ../Brain_IR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Brain_IR.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Brain_IR.o.ok ${OBJECTDIR}/_ext/1472/Brain_IR.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Brain_IR.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Brain_IR.o.d" -o ${OBJECTDIR}/_ext/1472/Brain_IR.o ../Brain_IR.c  
	
else
${OBJECTDIR}/_ext/1472/Tableau.o: ../Tableau.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Tableau.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Tableau.o.ok ${OBJECTDIR}/_ext/1472/Tableau.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Tableau.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Tableau.o.d" -o ${OBJECTDIR}/_ext/1472/Tableau.o ../Tableau.c  
	
${OBJECTDIR}/_ext/1472/Beacon.o: ../Beacon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Beacon.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Beacon.o.ok ${OBJECTDIR}/_ext/1472/Beacon.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Beacon.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Beacon.o.d" -o ${OBJECTDIR}/_ext/1472/Beacon.o ../Beacon.c  
	
${OBJECTDIR}/_ext/1472/Beacon_it.o: ../Beacon_it.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Beacon_it.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Beacon_it.o.ok ${OBJECTDIR}/_ext/1472/Beacon_it.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Beacon_it.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Beacon_it.o.d" -o ${OBJECTDIR}/_ext/1472/Beacon_it.o ../Beacon_it.c  
	
${OBJECTDIR}/_ext/43900113/QS_adc.o: ../QS/QS_adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_adc.o.ok ${OBJECTDIR}/_ext/43900113/QS_adc.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_adc.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_adc.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_adc.o ../QS/QS_adc.c  
	
${OBJECTDIR}/_ext/43900113/QS_buttons.o: ../QS/QS_buttons.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_buttons.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_buttons.o.ok ${OBJECTDIR}/_ext/43900113/QS_buttons.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_buttons.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_buttons.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_buttons.o ../QS/QS_buttons.c  
	
${OBJECTDIR}/_ext/43900113/QS_can.o: ../QS/QS_can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can.o.ok ${OBJECTDIR}/_ext/43900113/QS_can.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can.o ../QS/QS_can.c  
	
${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o: ../QS/QS_can_over_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.ok ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o ../QS/QS_can_over_uart.c  
	
${OBJECTDIR}/_ext/43900113/QS_configBits.o: ../QS/QS_configBits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_configBits.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_configBits.o.ok ${OBJECTDIR}/_ext/43900113/QS_configBits.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_configBits.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_configBits.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_configBits.o ../QS/QS_configBits.c  
	
${OBJECTDIR}/_ext/43900113/QS_global_vars.o: ../QS/QS_global_vars.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.ok ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_global_vars.o ../QS/QS_global_vars.c  
	
${OBJECTDIR}/_ext/43900113/QS_ports.o: ../QS/QS_ports.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ports.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ports.o.ok ${OBJECTDIR}/_ext/43900113/QS_ports.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_ports.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_ports.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_ports.o ../QS/QS_ports.c  
	
${OBJECTDIR}/_ext/43900113/QS_timer.o: ../QS/QS_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_timer.o.ok ${OBJECTDIR}/_ext/43900113/QS_timer.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_timer.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_timer.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_timer.o ../QS/QS_timer.c  
	
${OBJECTDIR}/_ext/43900113/QS_uart.o: ../QS/QS_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_uart.o.ok ${OBJECTDIR}/_ext/43900113/QS_uart.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_uart.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_uart.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_uart.o ../QS/QS_uart.c  
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.ok ${OBJECTDIR}/_ext/1472/main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d" -o ${OBJECTDIR}/_ext/1472/main.o ../main.c  
	
${OBJECTDIR}/_ext/43900113/QS_extern_it.o: ../QS/QS_extern_it.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.ok ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_extern_it.o ../QS/QS_extern_it.c  
	
${OBJECTDIR}/_ext/43900113/QS_can_over_xbee.o: ../QS/QS_can_over_xbee.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_xbee.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_xbee.o.ok ${OBJECTDIR}/_ext/43900113/QS_can_over_xbee.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can_over_xbee.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can_over_xbee.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can_over_xbee.o ../QS/QS_can_over_xbee.c  
	
${OBJECTDIR}/_ext/1472/Secretary.o: ../Secretary.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Secretary.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Secretary.o.ok ${OBJECTDIR}/_ext/1472/Secretary.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Secretary.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Secretary.o.d" -o ${OBJECTDIR}/_ext/1472/Secretary.o ../Secretary.c  
	
${OBJECTDIR}/_ext/1472/Brain_US.o: ../Brain_US.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Brain_US.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Brain_US.o.ok ${OBJECTDIR}/_ext/1472/Brain_US.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Brain_US.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Brain_US.o.d" -o ${OBJECTDIR}/_ext/1472/Brain_US.o ../Brain_US.c  
	
${OBJECTDIR}/_ext/1472/Array_US.o: ../Array_US.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Array_US.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Array_US.o.ok ${OBJECTDIR}/_ext/1472/Array_US.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Array_US.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Array_US.o.d" -o ${OBJECTDIR}/_ext/1472/Array_US.o ../Array_US.c  
	
${OBJECTDIR}/_ext/1472/Eyes.o: ../Eyes.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Eyes.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Eyes.o.ok ${OBJECTDIR}/_ext/1472/Eyes.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Eyes.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Eyes.o.d" -o ${OBJECTDIR}/_ext/1472/Eyes.o ../Eyes.c  
	
${OBJECTDIR}/_ext/1472/Brain_IR.o: ../Brain_IR.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Brain_IR.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Brain_IR.o.ok ${OBJECTDIR}/_ext/1472/Brain_IR.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Brain_IR.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -MMD -MF "${OBJECTDIR}/_ext/1472/Brain_IR.o.d" -o ${OBJECTDIR}/_ext/1472/Brain_IR.o ../Brain_IR.c  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/Balises_Project.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/Balises_Project.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}        -Wl,--defsym=__MPLAB_BUILD=1,-L"..",-Map="${DISTDIR}/Balises_Project.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1
else
dist/${CND_CONF}/${IMAGE_TYPE}/Balises_Project.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Balises_Project.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}        -Wl,--defsym=__MPLAB_BUILD=1,-L"..",-Map="${DISTDIR}/Balises_Project.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION)
	${MP_CC_DIR}\\pic30-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/Balises_Project.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -omf=elf
endif

.pre:
	@echo "--------------------------------------"
	@echo "User defined pre-build step: []"
	@
	@echo "--------------------------------------"

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
