/* Martin Thomas 4/2009 */
// Repris et modifié par Nirgal : 201308

#include "integer.h"
#include "fattime.h"
#include "../../../RTC.h"

fattime_t get_fattime (void)
{
	debug_printf("RTC unimplemented\n");
	Uint8 seconds, minutes, hours, day, date, month, year;
	//Le format FAT définit une date sur 32 bits...
	//0 vaut le 01/01/1980 à 0h00m00s
	
	if(RTC_get_time (&seconds, &minutes, &hours, &day, &date, &month, &year))
	{
		return 		(((Uint32)(year) + 20) << 25 ) 	//20 est la différence entre 2000 et 1980.
				| 	((Uint32)(month) << 21 )
				| 	((Uint32)(date) << 16 )
				| 	((Uint32)(hours) << 11 )
				| 	((Uint32)(minutes) << 5 )
				| 	((Uint32)(seconds) >> 1 ) ;
	}
	else
	{
		return 0;
	}
}

