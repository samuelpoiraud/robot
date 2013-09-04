/* Martin Thomas 4/2009 */
// Repris et modifié par Nirgal : 201308

#include "integer.h"
#include "fattime.h"
#include "../../../RTC.h"

fattime_t get_fattime (void)
{
	//Le format FAT définit une date sur 32 bits...
	//0 vaut le 01/01/1980 à 0h00m00s
	date_t date;
	
	if(RTC_get_local_time (&date))
	{
		return 	(	((Uint32)(date.year) + 20) << 25 ) 	//20 est la différence entre 2000 et 1980.
				| 	((Uint32)(date.month) << 21 )
				| 	((Uint32)(date.date) << 16 )
				| 	((Uint32)(date.hours) << 11 )
				| 	((Uint32)(date.minutes) << 5 )
				| 	((Uint32)(date.seconds) >> 1 ) ;
	}
	else
	{
		return 0;
	}
}

