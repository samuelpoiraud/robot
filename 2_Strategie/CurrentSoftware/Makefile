# Fichier : Makefile

# permet d'activer la création du fichier de listing (fichiers .d et .lst) -> + long a la compilation, d'où la désactivation possible !
DEBUG=no

# Nom du Projet (les fichiers .lst, .elf, .bin portent ce nom).
PROJ=Strategie

# Liste des fichiers sources assembleur
S_SRCS=startup_stm32f40xx.S

# Liste des fichiers sources C
C_SRCS=					\
./QS/QS_adc.c			\
./QS/QS_buttons.c  		\
./QS/QS_can.c  			\
./QS/QS_can_over_uart.c \
 ./QS/QS_global_vars.c  \
 ./QS/QS_ports.c   		\
 ./QS/QS_sys.c  		\
 ./QS/QS_timer.c  		\
 ./QS/QS_trap_handler.c \
 ./QS/QS_uart.c  		\
 ./QS/QS_watchdog.c  	\
 ./QS/QS_who_am_i.c  	\
 ./Asser_functions.c  	\
 ./Generic_functions.c  \
 ./Geometry.c  			\
 ./Pathfind.c  			\
 ./Stacks.c  			\
 ./act_functions.c  	\
 ./actions.c  			\
 ./actions_cake.c  		\
 ./actions_cherries.c  	\
 ./actions_gifts.c  	\
 ./actions_glasses.c 	\
 ./actions_utils.c  	\
 ./avoidance.c  		\
 ./brain.c  			\
 ./button.c  			\
 ./can_utils.c  		\
 ./clock.c  			\
 ./detection.c 		 	\
 ./elements.c  			\
 ./environment.c  		\
 ./main.c  				\
 ./maths_home.c  		\
 ./output_log.c  		\
 ./polygon.c  			\
 ./queue.c  			\
 ./sick.c   			\
 ./system_stm32f4xx.c  	\
 ./telemeter.c  		\
 ./zone_mutex.c 		\
 ./actions_tests_krusty.c  			\
 ./actions_tests_krusty_micro.c  	\
 ./actions_tests_tiny.c  			\
 ./actions_tests_tiny_micro.c

# Options d'assemblage
ASFLAGS=-Wall -Wa,$(LSTFLAGS) -c -fmessage-length=0 $(DFLAGS) -mcpu=cortex-m4 -mthumb

# Options de compilation
CFLAGS=-c -I"./stm32f4xx" -O2


# -------------------------------------------------------
# Variables privées
# -------------------------------------------------------

# Liste des fichiers objets à construire
OBJS=$(S_SRCS:.S=.o) $(C_SRCS:.c=.o)

DS = $(OBJS:.o=.d)
# Inclusion des fichiers de dépendances...
-include $(DS)

LSTS = $(OBJS:.o=.lst)

# Options d'édition des liens
LDFLAGS=-T"stm32f4xx_flash.ld" -nostartfiles -L"./stm32f4xx" -Wl,-Map,$(PROJ).map -mcpu=cortex-m4 -mthumb 
DFLAGS=-MMD -MP -MF"$(@:.o=.d)" -MT"$@" 

ifeq ($(DEBUG),yes)
	LSTFLAGS= -adhlns="$(@:.o=.lst)"
	LSTFILE = $(PROJ).lst
else
	LSTFLAGS=
	LSTFILE=
endif

# ---------------------------------------------
# Règles
# ---------------------------------------------

# Règle générique : construire le projet
all: $(PROJ).bin $(LSTFILE)

# Règle de construction du .bin à partir du .elf
$(PROJ).bin: $(PROJ).elf
	arm-none-eabi-objcopy -Obinary ${PROJ}.elf ${PROJ}.bin
	
# Règle d'édition des liens
$(PROJ).elf: $(OBJS)
	arm-none-eabi-gcc $(LDFLAGS) -o $(PROJ).elf $(OBJS) -lstm32f4xx

# Règle de génération du fichier listing
$(PROJ).lst: $(LSTS)
	arm-none-eabi-objdump -h -S $(PROJ).elf > "$(PROJ).lst"
	
# Règle d'assemblage
%.o: %.S
	arm-none-eabi-gcc -x assembler-with-cpp $(ASFLAGS) -o $@ $<

# Règle de compilation
%.o: %.c
	arm-none-eabi-gcc $(CFLAGS) $(ASFLAGS) -o $@ $<
	
# Règle de nettoyage du projet
clean:
	rm -f $(OBJS) $(PROJ).elf $(PROJ).bin $(PROJ).lst $(DS) $(LSTS)
	
