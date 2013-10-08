#include "clean.h"
#include <errno.h>
#include "../term_commands_utils.h"
#include "../../../QS/QS_all.h"

const char term_cmd_clean_brief[] = "Supprime tous les fichiers de match";
const char term_cmd_clean_help[] =
		"Utilisation : clean y\n"
		"\n"
		"y          Indique que l'utilisateur connait la commande\n"
		"           Doit être indiqué pour autoriser la suppression des fichiers de match\n";

int term_cmd_clean(int argc, const char *argv[]) {
	BYTE res;
	Sint16 match;
	char path[12];

	if(argc < 1)
		return EINVAL;

	if(argv[0][0] != 'y' || argv[0][1] != '\0')
		return EINVAL;

	debug_printf("Cleaning all match files - Cela peut prendre plusieurs minutes\n");
	debug_printf("Si vous ne vouliez pas supprimer les matchs enregistrés, RESETTEZ VITE LA CARTE STRATEGIE !!!\n");
	debug_printf("Un point = 100 fichiers de match traités, commence au match 9999\n");

	for(match = 9999; match>=0; match--)
	{
		sprintf(path, "%04d.MCH", match);
		res = f_unlink(path);
		if(res == FR_OK)
			debug_printf("\n%s deleted\n",path);

		if(match % 100 == 0)
			debug_printf(".");
	}
	res = f_unlink("index.inf");
	if(res == FR_OK)
		debug_printf("\nindex.inf deleted\n");

	debug_printf("Vous pouvez maintenant reseter la carte Stratégie (si vous lancez un match maintenant, il ne sera pas enregistré).");

	return 0;
}
