#!/usr/bin/make -f
# Fichier : Makefile

##############################################################################
# Variable modifiables
##############################################################################

# Nom du Projet (les fichiers .lst, .elf, .bin portent ce nom).
# Les fichiers *.files, *.includes doivent avoir ce nom !
PROJECT=SuperStrat

# DEBUG ou RELEASE,  en majuscule !
BUILD_TYPE = RELEASE

# permet d'activer la création du fichier de listing (fichiers .lst) -> + long a la compilation, d'où la désactivation possible !
OUTPUT_LST=no

#BUILD_DIR = build-$(call to_lowercase,$(BUILD_TYPE)) # unsupported by make ...
BUILD_DIR=.

DEST_DIR = bin-$(call to_lowercase,$(BUILD_TYPE))

WARNING_LEVEL = -Wall
SOURCE_LIST = $(shell $(CMD_CAT) $(PROJECT).files)
INCLUDE_LIST = $(shell $(CMD_CAT) $(PROJECT).includes)

# ----------------------------------------------------------------------------
# Flags
# ----------------------------------------------------------------------------

C_FLAGS = -Wall -mcpu=cortex-m4 -mthumb
C_FLAGS_DEBUG = -g
C_FLAGS_RELEASE = -O2

ASM_FLAGS = -Wall -mcpu=cortex-m4 -mthumb
ASM_FLAGS_DEBUG = -g
ASM_FLAGS_RELEASE =

LINKER_FLAGS = -Wall -T"./QS/stm32f4xx_flash.ld" -nostartfiles -Wl,-Map,$(DEST_DIR)/$(PROJECT).map
LINKER_FLAGS_DEBUG = -lstm32f4xx-d
LINKER_FLAGS_RELEASE = stm32f4xx/libstm32f4xx.a

CC = $(TOOLCHAIN_DIR)arm-none-eabi-gcc
OBJDUMP = $(TOOLCHAIN_DIR)arm-none-eabi-objdump
OBJCOPY = $(TOOLCHAIN_DIR)arm-none-eabi-objcopy

##############################################################################
# Variables privées
##############################################################################

#fonction qui recupère tous les flags pour le mode demandé (par ex C_FLAGS avec C_FLAGS_DEBUG en mode debug)
get_final_flags = $($(1)) $($(1)_$(BUILD_TYPE))
to_lowercase = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))
CMD_CAT = cat

ifeq ($(OUTPUT_LST),yes)
	LINKER_FLAGS += -Wa,-adhlns="$(@:.o=.lst)"
endif

# Options d'assemblage
ASFLAGS = $(call get_final_flags,ASM_FLAGS)

# Options de compilation
CFLAGS = $(call get_final_flags,C_FLAGS) -MMD -MP -MF"$(@:.o=.d)" -MT"$@"

# Options de linkage
LDFLAGS = $(call get_final_flags,LINKER_FLAGS)

# Liste des fichiers objets à construire
C_SOURCE_LIST = $(filter %.c %.C,$(SOURCE_LIST))
ASM_SOURCE_LIST = $(filter %.s %.S,$(SOURCE_LIST))
C_OBJECT_LIST = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(C_SOURCE_LIST))))
ASM_OBJECT_LIST = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(ASM_SOURCE_LIST))))
DEP_FILE_LIST = $(addprefix $(BUILD_DIR)/,$(addsuffix .d,$(basename $(C_SOURCE_LIST))))
LST_FILE_LIST = $(addprefix $(BUILD_DIR)/,$(addsuffix .lst,$(basename $(C_SOURCE_LIST))))
DIR_LIST = $(addprefix $(BUILD_DIR)/,$(dir $(C_SOURCE_LIST)))


# ---------------------------------------------
# Règles
# ---------------------------------------------

# Règle générique : construire le projet
all: prepare $(DEST_DIR)/$(PROJECT).bin

# Inclusion des fichiers de dépendances après all pour garder all comme target par défaut
-include $(DEP_FILE_LIST)

prepare:
	mkdir -p $(DEST_DIR)

# Règle de construction du .bin à partir du .elf
$(DEST_DIR)/$(PROJECT).bin: $(DEST_DIR)/$(PROJECT).elf
	$(OBJCOPY) -Obinary $(DEST_DIR)/${PROJECT}.elf $(DEST_DIR)/${PROJECT}.bin

# Règle d'édition des liens
$(DEST_DIR)/$(PROJECT).elf: $(ASM_OBJECT_LIST) $(C_OBJECT_LIST)
	$(CC) $(ASM_OBJECT_LIST) $(C_OBJECT_LIST) $(LDFLAGS) -o $(DEST_DIR)/$(PROJECT).elf

# Règle de génération du fichier listing
$(DEST_DIR)/$(PROJECT).lst: $(DEST_DIR)/$(PROJECT).elf
	$(OBJDUMP) -h -S $(DEST_DIR)/$(PROJECT).elf > "$(DEST_DIR)/$(PROJECT).lst"

# Règle d'assemblage
%.o: %.S
	$(CC) -c -x assembler-with-cpp $(ASFLAGS) $(addsuffix ",$(addprefix -I",$(INCLUDE_LIST))) -o $@ $<

# Règle de compilation
%.o: %.c
	$(CC) -c $(CFLAGS) $(addsuffix ",$(addprefix -I",$(INCLUDE_LIST))) -o $@ $<

# Règle de nettoyage du projet
clean:
	rm -f $(ASM_OBJECT_LIST) $(C_OBJECT_LIST) $(DEP_FILE_LIST) $(LST_FILE_LIST)

mrproper: clean
	rm -f $(DEST_DIR)/$(PROJECT).elf $(DEST_DIR)/$(PROJECT).bin $(DEST_DIR)/$(PROJECT).map $(DEST_DIR)/$(PROJECT).lst

.PHONY : prepare all clean mrproper
