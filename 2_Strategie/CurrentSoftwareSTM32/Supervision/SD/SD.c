
#include "SD.h"
#include "../../QS/QS_ports.h"
#include "../../QS/QS_CANmsgList.h"
#include "../../QS/QS_can_over_uart.h"
#include "../../QS/QS_spi.h"
#include "../Verbose_can_msg.h"
#include "misc_test_term.h"
#include "ff_test_term.h"
#include "Libraries/fat_sd/ff.h"
#include "Libraries/fat_sd/diskio.h"
#include "term_io.h"
#include <stdarg.h>

#define MAX_TIME_BEFORE_SYNC	500	//Durée max entre la demande d'enregistrement d'un message et son enregistrement effectif sur la carte SD.



//TODO :
/*
 *  - Faire fonctionner le DMA + mesurer le gain
 *  - Réglage de la vitesse du SPI... -> ?
 *  - Mettre en place un système d'enregistrement des matchs... utilisant la date et un numéro pour les cas où la RTC ne fonctionne pas
 *  -
 *
 */


static FATFS fatfs;
static FIL file_match;
static FIL file_read_match;
static volatile bool_e file_opened;
static Uint16 time_before_sync = 0;
volatile bool_e data_waiting_for_sync = FALSE;

volatile bool_e sd_ready = FALSE;
volatile Uint16	match_id = 0xFFFF;
volatile Uint16	read_match_id = 0xFFFF;

//@post	SD_printf peut être appelée... (si tout s'est bien passé, les logs peuvent être enregistrés...)
void SD_init(void)
{
	PORTS_spi_init();
	SPI_init();
	SD_process_main();	//Permet d'ouvrir le plus vite possible le fichier de log.
}



/*
 * Si la chaine fabriquée contient un '\n', l'évènement sera envoyé sur la carte SD avec une info de date et de source... Sinon, le texte demandé sera envoyé tel quel.
 */
int SD_printf(char * s, ...)
{
	Uint16 ret;
	Uint16 i;
	bool_e b_insert_time;
	char buf[256];

	va_list args;
	va_start (args, s);
	ret = sprintf(buf, s, args);	//Prépare la chaine à envoyer.
	va_end (args);

	b_insert_time = FALSE;
	for(i=0;buf[i];i++)
	{
		if(buf[i] == '\n')
			b_insert_time = TRUE;
	}

	SD_new_event(FROM_SOFT, NULL, buf, b_insert_time);
	if(SWITCH_VERBOSE)
		debug_printf(buf);	//On en profite pour Verboser l'événement.
	return ret;
}

//Appeler cette fonction pour chaque nouvel évènement.
void SD_new_event(source_e source, CAN_msg_t * can_msg, char * user_string, bool_e insert_time)
{
	char string[128];
	Uint32 n = 0;
	Uint8 i;
	Uint32 written = 0;

	//Dater l'évènement dans le match
	time32_t time_ms = global.env.match_time;

	if(sd_ready == FALSE || file_opened == FALSE)
		return;	//Nothing todo...

	//Source et instant de l'événement.
	n=0;
	string[n++] = STX;	//Début de l'info de temps.
	string[n++] = source;
	string[n++] = HIGHINT(time_ms/2);
	string[n++] = LOWINT(time_ms/2);
	string[n++] = ETX;	//Fin de l'info de temps.

	if(can_msg)
	{
		string[n++] = SOH;	//Début du msg CAN
		string[n++] = HIGHINT(can_msg->sid);
		string[n++] = LOWINT(can_msg->sid);
		for (i=0; i<can_msg->size && i<8; i++)
			string[n++] = can_msg->data[i];
		for (i=can_msg->size; i<8; i++)
			string[n++] = 0xFF;
		string[n++] = can_msg->size;
		string[n++] = EOT;	//Début du msg CAN
	}
	if(user_string)
	{
		for(i=0; user_string[i]; i++)
		{
			if(user_string[i] > 0x03)	//0x00 à 0x03 sont réservés pour les balises de msg can et d'info de temps+source.
				string[n++] = user_string[i];
			else
				debug_printf("Vous ne devriez pas envoyer à SD_new_event() des caractères <= 0x03... ils sont ignorés.");
		}
	}

	f_write(&file_match, string, n, (unsigned int *)&written);
	if(written != n)
		debug_printf("WARNING : SD:wrote failed %ld/%ld", written, n);

	if(data_waiting_for_sync == FALSE)	//La synchro était faite, aucune donnée n'était en attente d'écriture... on relance le compteur.
		time_before_sync = 500;
	data_waiting_for_sync = TRUE;

	if(can_msg && can_msg->sid == BROADCAST_STOP_ALL)
	{
		f_close(&file_match);
		debug_printf("Closing file. Match saved\n");
		time_before_sync = 0;
		data_waiting_for_sync = FALSE;
		file_opened = FALSE;
		read_match_id = match_id;	//pour pouvoir dépiler le match qui vient de se terminer.
	}
}

//renvoie TRUE si la carte SD est montée et accessible.
bool_e SD_analyse(void)
{
	FRESULT res;
	FATFS * fs;
	Uint32 p2;

	if(f_mount( 0, &fatfs ) == FR_OK)
	{
		debug_printf("SD mounted\n");
		res = f_getfree("", (DWORD*)&p2, &fs);
		if (res == FR_OK)
		{
			debug_printf("FAT type = %u (%s)\nBytes/Cluster = %lu\nNumber of FATs = %u\n"
			"Root DIR entries = %u\nSectors/FAT = %lu\nNumber of clusters = %lu\n"
			"FAT start (lba) = %lu\nDIR start (lba,clustor) = %lu\nData start (lba) = %lu\n\n",
			(WORD)fs->fs_type,
			(fs->fs_type==FS_FAT12) ? "FAT12" : (fs->fs_type==FS_FAT16) ? "FAT16" : "FAT32",
			(DWORD)fs->csize * 512, (WORD)fs->n_fats,
			fs->n_rootdir, fs->sects_fat, (DWORD)fs->max_clust - 2,
			fs->fatbase, fs->dirbase, fs->database
			);
			return TRUE;
		}
	}
	else
	{
		//TODO verbose de l'erreur SD + précise.
		debug_printf("SD fail to read infos. We stop using SD card.\n");
	}

	return FALSE;
}

bool_e SD_open_file_for_next_match(void)
{
	Uint32 nb_read, nb_written;
	Uint8 read_buffer[4];
	char path[32];

	//Lit la carte SD et déduit le numéro du fichier à ouvrir.
	read_match_id = 0;	//Valeur par défaut...

	if(f_open(&file_match, "index.inf", FA_READ) == FR_OK)
	{
		nb_read = 0;
		f_read(&file_match, read_buffer, 4, (unsigned int *)&nb_read);
		if(nb_read == 4)
		{
			if(		read_buffer[0] >= '0' && read_buffer[0] <= '9' &&
					read_buffer[1] >= '0' && read_buffer[1] <= '9' &&
					read_buffer[2] >= '0' && read_buffer[2] <= '9' &&
					read_buffer[3] >= '0' && read_buffer[3] <= '9')
			{
				read_match_id = 1000*(read_buffer[0]-'0') + 100*(read_buffer[1]-'0') + 10*(read_buffer[2]-'0') + (read_buffer[3]-'0');
				debug_printf("index.inf read : previous match_id is %04d.MCH\n",read_match_id);
			}
		}
		f_close(&file_match);
	}
	else
		debug_printf("Can't read index.inf : previous match_id is %04d.MCH\n",read_match_id);

		//read_match_id 	correspond au dernier match enregistré (0 si aucun).
		//match_id 			correspond au prochain match...
	match_id = read_match_id + 1;

	sprintf(path, "%04d.MCH", match_id);

	//On enregistre le numéro match_id du nouveau match dans index.inf
	if(f_open(&file_match, "index.inf", FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS) == FR_OK)
	{
		f_write(&file_match, path, 4, (unsigned int *)&nb_written);	//On écrit les 4 premiers octets (juste le numéro).
		f_close(&file_match);
	}

	if(f_open(&file_match, path, FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS) == FR_OK)
	{
		debug_printf("File %s created\n",path);
		file_opened = TRUE;
		sd_ready = TRUE;
	}
	else
		debug_printf("WARNING : Unable to create file %s\nThe events will not be saved in the current match !\n",path);

	return TRUE;
}


void SD_process_main(void)
{
	typedef enum
	{
		INIT = 0,
		SD_FAILED,
		IDLE,
		APPSTATE_FF_TERM,
		APPSTATE_TESTMENU
	}state_e;
	static state_e state = INIT;


	switch (state)
	{
		case INIT:
			state = SD_FAILED;		//Par défaut...
			if(SD_analyse())
				if(SD_open_file_for_next_match())
					state = IDLE;
		break;
		case SD_FAILED:
			break;

		case IDLE:

			//f_sync est forcé "manuellement" pour éviter une perte de message en cas de reset...
			//on sauvegarde les données en buffer, et en attente d'écriture : au plus tard MAX_TIME_BEFORE_SYNC ms après leur demande d'enregistrement.
			if(time_before_sync == 0 && data_waiting_for_sync)
			{
				data_waiting_for_sync = FALSE;
				//LED_USER = 1;
				if(f_sync(&file_match) != FR_OK)
					debug_printf("File sync failed\n");
				else
				{
					//LED_USER = 0;
					//debug_printf("File sync done\n");
				}
			}
			if(get_command())
			{
				debug_printf("Command received - switching to Terminal mode for SD management.\n Next events will not be saved.\n");
				sd_ready = FALSE;
				state = APPSTATE_FF_TERM;
			}
			break;
		case APPSTATE_FF_TERM:
			if ( !ff_test_term() )
			{
				state = APPSTATE_TESTMENU;
			}
			break;
		case APPSTATE_TESTMENU:
			if ( !misc_test_term() )
			{
				state = APPSTATE_FF_TERM;
			}
			break;
		default:
			state = APPSTATE_TESTMENU;
			break;
	}
}



void SD_print_previous_match(void)
{
	char path[16];
	char read_byte;
	Uint32 nb_read;
	Uint8 i;
	Uint16 t;
	source_e source;
	CAN_msg_t msg;
	typedef enum
	{
		READ_TEXT,
		READ_TIME,
		READ_CAN_MSG
	}state_e;
	state_e state = READ_TEXT;

/*	if(file_opened)
	{
		file_opened = FALSE;	//Tant pis pour l'éventuel match en cours d'écriture. Il ne sera plus complété... !
		f_close(&file_match);	//Je ferme...
	}
*/
	if(read_match_id == 0)
	{
		debug_printf("Fin des matchs en mémoire\n");
		return;
	}

	sprintf(path, "%04d.MCH", read_match_id);

	//On ouvre le fichier du match précédent.
	if(f_open(&file_read_match, path, FA_READ) == FR_OK)	//Ouverture en lecture seule.
	{
		debug_printf("Match : %s\n",path);
		do
		{
			if(f_read(&file_read_match, &read_byte, 1, (unsigned int *)&nb_read) != FR_OK)
			{
				debug_printf("SD Fail during reading of match %s\n",path);
				break;
			}
			switch(state)
			{
				case READ_TEXT:
					i = 0;
					switch(read_byte)
					{
						case SOH:
							state = READ_CAN_MSG;
							break;
						case STX:
							state = READ_TIME;
							break;
						default:
							debug_printf("%c",read_byte);	//La méthode est un peu lourde... mais tant pis... c'est pas pendant le match.
							break;
					}
				break;
				case READ_TIME:
					switch(i)
					{
						case 0:		source = read_byte;					break;
						case 1:		t = read_byte;						break;
						case 2:		t = U16FROMU8(t, read_byte);		break;
						case 3:
							if(read_byte != ETX)
								debug_printf("Bad time format\n");
							else
							{
								debug_printf("t=%.2d.%03ds ",t/500, (t%500)*2);
								switch(source)
								{
									case FROM_SOFT:				debug_printf("SOFT-> ");	break;
									case FROM_BUS_CAN:			debug_printf("BCAN-> ");	break;
									case FROM_UART1:			debug_printf("U1RX-> ");	break;
									case FROM_UART2:			debug_printf("U2RX-> ");	break;
									case FROM_XBEE:				debug_printf("XBEE-> ");	break;
									case TO_BUSCAN:				debug_printf("BCAN<- ");	break;//Envoyé par nous, sur le bus can
									case TO_UART1:				debug_printf("U1TX<- ");	break;//Envoyé par nous, sur l'uart1
									case TO_UART2:				debug_printf("U2TX<- ");	break;//Envoyé par nous, sur l'uart2
									case TO_XBEE_BROADCAST:		debug_printf("XBEb<- ");	break;//Envoyé par nous, sur le XBee, en Broadcast
									case TO_XBEE_DESTINATION:	debug_printf("XBEd<- ");	break;	//Envoyé par nous, sur le XBee, à la destination par défaut
									default:					debug_printf("UNKN-- ");	break;
								}
							}
							//no break;
						default:
							state = READ_TEXT;
							break;
					}
					i++;
					break;
				case READ_CAN_MSG:
					switch(i)
					{
						case 0:		msg.sid = read_byte;							break;
						case 1:		msg.sid = U16FROMU8(msg.sid, read_byte);		break;
						//case 2 to 9 = datas...
						case 10:	msg.size = read_byte;							break;
						case 11:
							if(read_byte != EOT)
								debug_printf("Bad CAN_MSG format\n");
							else
								VERBOSE_CAN_MSG_print(&msg);
							state = READ_TEXT;
							break;
						default:	//datas...
							msg.data[i-2] = read_byte;
							break;
					}
					i++;
					break;
				default:
					state = READ_TEXT;
					break;
			}

		}while(nb_read == 1);
		f_close(&file_read_match);
	}
	else
	{
		debug_printf("Can't open match : %s\n",path);
	}
	read_match_id--;
}


void SD_process_1ms(void)
{
	static Uint8 time_10ms=0;

	if(time_before_sync)
		time_before_sync--;

	time_10ms++;
	if ( time_10ms >= 10 )
	{
		time_10ms = 0;
		disk_timerproc(); /* to be called every 10ms */
	}

	ff_test_term_timerproc(); /* to be called every ms */
}


