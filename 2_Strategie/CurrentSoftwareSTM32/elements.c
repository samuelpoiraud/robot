#include "elements.h"

#define LOG_PREFIX "element: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ELEMENTS
#include "QS/QS_outputlog.h"


ELEMENTS_element_s ELEMENTS_spot[NB_SPOT];
ELEMENTS_element_s ELEMENTS_cup[NB_CUP];


void ELEMENTS_init(){
	Uint8 i;

	for(i=0;i<NB_SPOT;i++)
		ELEMENTS_spot[i].state = AVAILABLE;

	for(i=0;i<NB_CUP;i++)
		ELEMENTS_cup[i].state = AVAILABLE;


	// Initialisation des positions de chaques élements

	// Spot Jaune
	ELEMENTS_spot[0].color = YELLOW;
	ELEMENTS_spot[0].x = 200;
	ELEMENTS_spot[0].y = 50;

	ELEMENTS_spot[1].color = YELLOW;
	ELEMENTS_spot[1].x = 100;
	ELEMENTS_spot[1].y = 850;

	ELEMENTS_spot[2].color = YELLOW;
	ELEMENTS_spot[2].x = 200;
	ELEMENTS_spot[2].y = 850;

	ELEMENTS_spot[3].color = YELLOW;
	ELEMENTS_spot[3].x = 1750;
	ELEMENTS_spot[3].y = 90;

	ELEMENTS_spot[4].color = YELLOW;
	ELEMENTS_spot[4].x = 1850;
	ELEMENTS_spot[4].y = 90;

	ELEMENTS_spot[5].color = YELLOW;
	ELEMENTS_spot[5].x = 1355;
	ELEMENTS_spot[5].y = 870;

	ELEMENTS_spot[6].color = YELLOW;
	ELEMENTS_spot[6].x = 1770;
	ELEMENTS_spot[6].y = 1100;

	ELEMENTS_spot[7].color = YELLOW;
	ELEMENTS_spot[7].x = 1400;
	ELEMENTS_spot[7].y = 1300;


	// Spot Vert
	ELEMENTS_spot[8].color = GREEN;
	ELEMENTS_spot[8].x = 200;
	ELEMENTS_spot[8].y = 2910;

	ELEMENTS_spot[9].color = GREEN;
	ELEMENTS_spot[9].x = 100;
	ELEMENTS_spot[9].y = 2150;

	ELEMENTS_spot[10].color = GREEN;
	ELEMENTS_spot[10].x = 200;
	ELEMENTS_spot[10].y = 2150;

	ELEMENTS_spot[11].color = GREEN;
	ELEMENTS_spot[11].x = 1850;
	ELEMENTS_spot[11].y = 2910;

	ELEMENTS_spot[12].color = GREEN;
	ELEMENTS_spot[12].x = 1750;
	ELEMENTS_spot[12].y = 2910;

	ELEMENTS_spot[13].color = GREEN;
	ELEMENTS_spot[13].x = 1355;
	ELEMENTS_spot[13].y = 2130;

	ELEMENTS_spot[14].color = GREEN;
	ELEMENTS_spot[14].x = 1770;
	ELEMENTS_spot[14].y = 1900;

	ELEMENTS_spot[15].color = GREEN;
	ELEMENTS_spot[15].x = 1400;
	ELEMENTS_spot[15].y = 1700;


	//Cup
	ELEMENTS_cup[0].x = 830;
	ELEMENTS_cup[0].y = 910;

	ELEMENTS_cup[1].x = 1750;
	ELEMENTS_cup[1].y = 250;

	ELEMENTS_cup[2].x = 1650;
	ELEMENTS_cup[2].y = 1500;

	ELEMENTS_cup[3].x = 830;
	ELEMENTS_cup[3].y = 2090;

	ELEMENTS_cup[4].x = 1750;
	ELEMENTS_cup[4].y = 2750;
}
