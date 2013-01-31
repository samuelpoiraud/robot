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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Balise_Recepteur_US.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Balise_Recepteur_US.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/43900113/QS_adc.o ${OBJECTDIR}/_ext/43900113/QS_can.o ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o ${OBJECTDIR}/_ext/43900113/QS_configBits.o ${OBJECTDIR}/_ext/43900113/QS_extern_it.o ${OBJECTDIR}/_ext/43900113/QS_global_vars.o ${OBJECTDIR}/_ext/43900113/QS_ports.o ${OBJECTDIR}/_ext/43900113/QS_pwm.o ${OBJECTDIR}/_ext/43900113/QS_timer.o ${OBJECTDIR}/_ext/43900113/QS_uart.o ${OBJECTDIR}/_ext/43900113/QS_watchdog.o ${OBJECTDIR}/_ext/43900113/QS_spi.o ${OBJECTDIR}/_ext/1472/ReceptionUS.o ${OBJECTDIR}/_ext/1472/Synchro.o ${OBJECTDIR}/_ext/1472/PGA.o ${OBJECTDIR}/_ext/1472/Secretary.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1472/main.o.d ${OBJECTDIR}/_ext/43900113/QS_adc.o.d ${OBJECTDIR}/_ext/43900113/QS_can.o.d ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d ${OBJECTDIR}/_ext/43900113/QS_configBits.o.d ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d ${OBJECTDIR}/_ext/43900113/QS_ports.o.d ${OBJECTDIR}/_ext/43900113/QS_pwm.o.d ${OBJECTDIR}/_ext/43900113/QS_timer.o.d ${OBJECTDIR}/_ext/43900113/QS_uart.o.d ${OBJECTDIR}/_ext/43900113/QS_watchdog.o.d ${OBJECTDIR}/_ext/43900113/QS_spi.o.d ${OBJECTDIR}/_ext/1472/ReceptionUS.o.d ${OBJECTDIR}/_ext/1472/Synchro.o.d ${OBJECTDIR}/_ext/1472/PGA.o.d ${OBJECTDIR}/_ext/1472/Secretary.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/43900113/QS_adc.o ${OBJECTDIR}/_ext/43900113/QS_can.o ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o ${OBJECTDIR}/_ext/43900113/QS_configBits.o ${OBJECTDIR}/_ext/43900113/QS_extern_it.o ${OBJECTDIR}/_ext/43900113/QS_global_vars.o ${OBJECTDIR}/_ext/43900113/QS_ports.o ${OBJECTDIR}/_ext/43900113/QS_pwm.o ${OBJECTDIR}/_ext/43900113/QS_timer.o ${OBJECTDIR}/_ext/43900113/QS_uart.o ${OBJECTDIR}/_ext/43900113/QS_watchdog.o ${OBJECTDIR}/_ext/43900113/QS_spi.o ${OBJECTDIR}/_ext/1472/ReceptionUS.o ${OBJECTDIR}/_ext/1472/Synchro.o ${OBJECTDIR}/_ext/1472/PGA.o ${OBJECTDIR}/_ext/1472/Secretary.o


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
ProjectDir="M:\___ROBOT\_Codes 2013\balise\Software_Recepteur_US\Balise_Recepteur_US.X"
ConfName=default
ImagePath="dist\default\${IMAGE_TYPE}\Balise_Recepteur_US.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}"
ImageDir="dist\default\${IMAGE_TYPE}"
ImageName="Balise_Recepteur_US.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}"

.build-conf:  .pre ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/Balise_Recepteur_US.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
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
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.ok ${OBJECTDIR}/_ext/1472/main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d" -o ${OBJECTDIR}/_ext/1472/main.o ../main.c  
	
${OBJECTDIR}/_ext/43900113/QS_adc.o: ../QS/QS_adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_adc.o.ok ${OBJECTDIR}/_ext/43900113/QS_adc.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_adc.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_adc.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_adc.o ../QS/QS_adc.c  
	
${OBJECTDIR}/_ext/43900113/QS_can.o: ../QS/QS_can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can.o.ok ${OBJECTDIR}/_ext/43900113/QS_can.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can.o ../QS/QS_can.c  
	
${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o: ../QS/QS_can_over_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.ok ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o ../QS/QS_can_over_uart.c  
	
${OBJECTDIR}/_ext/43900113/QS_configBits.o: ../QS/QS_configBits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_configBits.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_configBits.o.ok ${OBJECTDIR}/_ext/43900113/QS_configBits.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_configBits.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_configBits.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_configBits.o ../QS/QS_configBits.c  
	
${OBJECTDIR}/_ext/43900113/QS_extern_it.o: ../QS/QS_extern_it.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.ok ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_extern_it.o ../QS/QS_extern_it.c  
	
${OBJECTDIR}/_ext/43900113/QS_global_vars.o: ../QS/QS_global_vars.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.ok ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_global_vars.o ../QS/QS_global_vars.c  
	
${OBJECTDIR}/_ext/43900113/QS_ports.o: ../QS/QS_ports.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ports.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ports.o.ok ${OBJECTDIR}/_ext/43900113/QS_ports.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_ports.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_ports.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_ports.o ../QS/QS_ports.c  
	
${OBJECTDIR}/_ext/43900113/QS_pwm.o: ../QS/QS_pwm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_pwm.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_pwm.o.ok ${OBJECTDIR}/_ext/43900113/QS_pwm.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_pwm.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_pwm.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_pwm.o ../QS/QS_pwm.c  
	
${OBJECTDIR}/_ext/43900113/QS_timer.o: ../QS/QS_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_timer.o.ok ${OBJECTDIR}/_ext/43900113/QS_timer.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_timer.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_timer.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_timer.o ../QS/QS_timer.c  
	
${OBJECTDIR}/_ext/43900113/QS_uart.o: ../QS/QS_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_uart.o.ok ${OBJECTDIR}/_ext/43900113/QS_uart.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_uart.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_uart.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_uart.o ../QS/QS_uart.c  
	
${OBJECTDIR}/_ext/43900113/QS_watchdog.o: ../QS/QS_watchdog.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_watchdog.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_watchdog.o.ok ${OBJECTDIR}/_ext/43900113/QS_watchdog.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_watchdog.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_watchdog.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_watchdog.o ../QS/QS_watchdog.c  
	
${OBJECTDIR}/_ext/43900113/QS_spi.o: ../QS/QS_spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_spi.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_spi.o.ok ${OBJECTDIR}/_ext/43900113/QS_spi.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_spi.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_spi.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_spi.o ../QS/QS_spi.c  
	
${OBJECTDIR}/_ext/1472/ReceptionUS.o: ../ReceptionUS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/ReceptionUS.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/ReceptionUS.o.ok ${OBJECTDIR}/_ext/1472/ReceptionUS.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/ReceptionUS.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/ReceptionUS.o.d" -o ${OBJECTDIR}/_ext/1472/ReceptionUS.o ../ReceptionUS.c  
	
${OBJECTDIR}/_ext/1472/Synchro.o: ../Synchro.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Synchro.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Synchro.o.ok ${OBJECTDIR}/_ext/1472/Synchro.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Synchro.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/Synchro.o.d" -o ${OBJECTDIR}/_ext/1472/Synchro.o ../Synchro.c  
	
${OBJECTDIR}/_ext/1472/PGA.o: ../PGA.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/PGA.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/PGA.o.ok ${OBJECTDIR}/_ext/1472/PGA.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/PGA.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/PGA.o.d" -o ${OBJECTDIR}/_ext/1472/PGA.o ../PGA.c  
	
${OBJECTDIR}/_ext/1472/Secretary.o: ../Secretary.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Secretary.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Secretary.o.ok ${OBJECTDIR}/_ext/1472/Secretary.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Secretary.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/Secretary.o.d" -o ${OBJECTDIR}/_ext/1472/Secretary.o ../Secretary.c  
	
else
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.ok ${OBJECTDIR}/_ext/1472/main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d" -o ${OBJECTDIR}/_ext/1472/main.o ../main.c  
	
${OBJECTDIR}/_ext/43900113/QS_adc.o: ../QS/QS_adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_adc.o.ok ${OBJECTDIR}/_ext/43900113/QS_adc.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_adc.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_adc.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_adc.o ../QS/QS_adc.c  
	
${OBJECTDIR}/_ext/43900113/QS_can.o: ../QS/QS_can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can.o.ok ${OBJECTDIR}/_ext/43900113/QS_can.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can.o ../QS/QS_can.c  
	
${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o: ../QS/QS_can_over_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.ok ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_can_over_uart.o ../QS/QS_can_over_uart.c  
	
${OBJECTDIR}/_ext/43900113/QS_configBits.o: ../QS/QS_configBits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_configBits.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_configBits.o.ok ${OBJECTDIR}/_ext/43900113/QS_configBits.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_configBits.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_configBits.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_configBits.o ../QS/QS_configBits.c  
	
${OBJECTDIR}/_ext/43900113/QS_extern_it.o: ../QS/QS_extern_it.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.ok ${OBJECTDIR}/_ext/43900113/QS_extern_it.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_extern_it.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_extern_it.o ../QS/QS_extern_it.c  
	
${OBJECTDIR}/_ext/43900113/QS_global_vars.o: ../QS/QS_global_vars.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.ok ${OBJECTDIR}/_ext/43900113/QS_global_vars.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_global_vars.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_global_vars.o ../QS/QS_global_vars.c  
	
${OBJECTDIR}/_ext/43900113/QS_ports.o: ../QS/QS_ports.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ports.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_ports.o.ok ${OBJECTDIR}/_ext/43900113/QS_ports.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_ports.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_ports.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_ports.o ../QS/QS_ports.c  
	
${OBJECTDIR}/_ext/43900113/QS_pwm.o: ../QS/QS_pwm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_pwm.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_pwm.o.ok ${OBJECTDIR}/_ext/43900113/QS_pwm.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_pwm.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_pwm.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_pwm.o ../QS/QS_pwm.c  
	
${OBJECTDIR}/_ext/43900113/QS_timer.o: ../QS/QS_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_timer.o.ok ${OBJECTDIR}/_ext/43900113/QS_timer.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_timer.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_timer.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_timer.o ../QS/QS_timer.c  
	
${OBJECTDIR}/_ext/43900113/QS_uart.o: ../QS/QS_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_uart.o.ok ${OBJECTDIR}/_ext/43900113/QS_uart.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_uart.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_uart.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_uart.o ../QS/QS_uart.c  
	
${OBJECTDIR}/_ext/43900113/QS_watchdog.o: ../QS/QS_watchdog.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_watchdog.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_watchdog.o.ok ${OBJECTDIR}/_ext/43900113/QS_watchdog.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_watchdog.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_watchdog.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_watchdog.o ../QS/QS_watchdog.c  
	
${OBJECTDIR}/_ext/43900113/QS_spi.o: ../QS/QS_spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/43900113 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_spi.o.d 
	@${RM} ${OBJECTDIR}/_ext/43900113/QS_spi.o.ok ${OBJECTDIR}/_ext/43900113/QS_spi.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/43900113/QS_spi.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/43900113/QS_spi.o.d" -o ${OBJECTDIR}/_ext/43900113/QS_spi.o ../QS/QS_spi.c  
	
${OBJECTDIR}/_ext/1472/ReceptionUS.o: ../ReceptionUS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/ReceptionUS.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/ReceptionUS.o.ok ${OBJECTDIR}/_ext/1472/ReceptionUS.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/ReceptionUS.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/ReceptionUS.o.d" -o ${OBJECTDIR}/_ext/1472/ReceptionUS.o ../ReceptionUS.c  
	
${OBJECTDIR}/_ext/1472/Synchro.o: ../Synchro.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Synchro.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Synchro.o.ok ${OBJECTDIR}/_ext/1472/Synchro.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Synchro.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/Synchro.o.d" -o ${OBJECTDIR}/_ext/1472/Synchro.o ../Synchro.c  
	
${OBJECTDIR}/_ext/1472/PGA.o: ../PGA.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/PGA.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/PGA.o.ok ${OBJECTDIR}/_ext/1472/PGA.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/PGA.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/PGA.o.d" -o ${OBJECTDIR}/_ext/1472/PGA.o ../PGA.c  
	
${OBJECTDIR}/_ext/1472/Secretary.o: ../Secretary.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Secretary.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/Secretary.o.ok ${OBJECTDIR}/_ext/1472/Secretary.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Secretary.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../QS" -O2 -MMD -MF "${OBJECTDIR}/_ext/1472/Secretary.o.d" -o ${OBJECTDIR}/_ext/1472/Secretary.o ../Secretary.c  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/Balise_Recepteur_US.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=coff -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/Balise_Recepteur_US.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}   ..\QS\libp30F6010A-coff.a     -Wl,--defsym=__MPLAB_BUILD=1,--heap=256,-L"/C:/Program Files/Microchip/MPLAB C30/lib",-Map="${DISTDIR}/Balise_Recepteur_US.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1
else
dist/${CND_CONF}/${IMAGE_TYPE}/Balise_Recepteur_US.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=coff -mcpu=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Balise_Recepteur_US.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}   ..\QS\libp30F6010A-coff.a     -Wl,--defsym=__MPLAB_BUILD=1,--heap=256,-L"/C:/Program Files/Microchip/MPLAB C30/lib",-Map="${DISTDIR}/Balise_Recepteur_US.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION)
	${MP_CC_DIR}\\pic30-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/Balise_Recepteur_US.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -omf=coff
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
