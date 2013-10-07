#include "reset.h"
#include <errno.h>
#include "stm32f4xx_rcc.h"

const char term_cmd_reset_brief[] = "Reset la carte strategie";
const char term_cmd_reset_help[] =
		"Utilisation : reset y\n"
		"\n"
		"y          Argument indiquant que l'utilisateur est conscient de l'effet de cette commande\n";

int term_cmd_reset(int argc, const char *argv[]) {
	if(argc < 1)
		return EINVAL;

	if(argv[0][0] != 'y' || argv[0][1] != '\0')
		return EINVAL;

	NVIC_SystemReset();

	return 0;
}
