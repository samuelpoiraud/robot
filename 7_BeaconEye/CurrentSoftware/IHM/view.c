#include "view.h"

#include "../QS/QS_ports.h"
#include "../environment.h"
#include "../QS/QS_outputlog.h"
#include "../LCD/low layer/ssd2119.h"
#include "../LCD/image/image.h"


#define FIRST_COLOR					(0x0396)
#define SECOND_COLOR				(0xF5E0)
#define BEACON_COLOR				(0x0000)
#define TERRAIN_WIDTH				(300)
#define TERRAIN_HEIGHT				(200)

static void VIEW_drawPixelOnTerrain(Uint16 x, Uint16 y, bool_e erase, Uint16 color, const imageInfo_s *terrain);

void VIEW_drawEmptyTerrain(const imageInfo_s *terrain) {

	if(terrain->height == TERRAIN_HEIGHT && terrain->width == TERRAIN_WIDTH) {

		// On affiche le terrain
		SSD2119_putImage(PLATFORM_SIZE, 0, terrain->width, terrain->height, terrain->image, terrain->size);

		// On dessine les plateformes des balises
		SSD2119_drawFilledRectangle(PLATFORM_X_LEFT, PLATFORM_Y_TOP, PLATFORM_X_LEFT + PLATFORM_SIZE - 1, PLATFORM_Y_TOP + PLATFORM_SIZE - 1, FIRST_COLOR);
		SSD2119_drawFilledRectangle(PLATFORM_X_LEFT, PLATFORM_Y_MIDDLE, PLATFORM_X_LEFT + PLATFORM_SIZE - 1, PLATFORM_Y_MIDDLE + PLATFORM_SIZE - 1, SECOND_COLOR);
		SSD2119_drawFilledRectangle(PLATFORM_X_LEFT, PLATFORM_Y_BOTTOM, PLATFORM_X_LEFT + PLATFORM_SIZE - 1, PLATFORM_Y_BOTTOM + PLATFORM_SIZE - 1, FIRST_COLOR);
		SSD2119_drawFilledRectangle(PLATFORM_X_RIGHT, PLATFORM_Y_TOP, PLATFORM_X_RIGHT + PLATFORM_SIZE - 1, PLATFORM_Y_TOP + PLATFORM_SIZE - 1, SECOND_COLOR);
		SSD2119_drawFilledRectangle(PLATFORM_X_RIGHT, PLATFORM_Y_MIDDLE, PLATFORM_X_RIGHT + PLATFORM_SIZE - 1, PLATFORM_Y_MIDDLE + PLATFORM_SIZE - 1, FIRST_COLOR);
		SSD2119_drawFilledRectangle(PLATFORM_X_RIGHT, PLATFORM_Y_BOTTOM, PLATFORM_X_RIGHT + PLATFORM_SIZE - 1, PLATFORM_Y_BOTTOM + PLATFORM_SIZE - 1, SECOND_COLOR);

	} else {
		debug_printf("La taille de l'image du terrain est incorrecte.\n");
	}
}

void VIEW_drawBeaconPosition(color_e color) {

	SSD2119_drawFilledRectangle(PLATFORM_X_LEFT, PLATFORM_Y_MIDDLE, PLATFORM_X_LEFT + PLATFORM_SIZE - 1, PLATFORM_Y_MIDDLE + PLATFORM_SIZE - 1, SECOND_COLOR);
	SSD2119_drawFilledRectangle(PLATFORM_X_RIGHT, PLATFORM_Y_MIDDLE, PLATFORM_X_RIGHT + PLATFORM_SIZE - 1, PLATFORM_Y_MIDDLE + PLATFORM_SIZE - 1, FIRST_COLOR);

	if(color == TOP_COLOR) {
		SSD2119_putImageWithTransparence(PLATFORM_X_LEFT, PLATFORM_Y_MIDDLE, beacon.width, beacon.height, beacon.image, beacon.colorTransparence, beacon.size);
	} else {
		SSD2119_putImageWithTransparence(PLATFORM_X_RIGHT, PLATFORM_Y_MIDDLE, beacon.width, beacon.height, beacon.image, beacon.colorTransparence, beacon.size);
	}
}

static void VIEW_drawPixelOnTerrain(Uint16 x, Uint16 y, bool_e erase, Uint16 color, const imageInfo_s *terrain) {

	if(x < TERRAIN_WIDTH && y < TERRAIN_HEIGHT) {
		if(erase) {
			SSD2119_drawPixel(x + PLATFORM_SIZE, y, terrain->image[300 * y + x]);
		} else {
			SSD2119_drawPixel(x + PLATFORM_SIZE, y, color);
		}

	}
}

void VIEW_drawCircle(Uint16 x0, Uint16 y0, Uint16 r, bool_e erase, Uint16 color, const imageInfo_s *terrain) {

	Sint16 f = 1 - r;
	Sint16 ddF_x = 1;
	Sint16 ddF_y = -2 * r;
	Sint16 x = 0;
	Sint16 y = r;

	VIEW_drawPixelOnTerrain(x0, y0 + r, erase, color, terrain);
	VIEW_drawPixelOnTerrain(x0, y0 - r, erase, color, terrain);
	VIEW_drawPixelOnTerrain(x0 + r, y0, erase, color, terrain);
	VIEW_drawPixelOnTerrain(x0 - r, y0, erase, color, terrain);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		VIEW_drawPixelOnTerrain(x0 + x, y0 + y, erase, color, terrain);
		VIEW_drawPixelOnTerrain(x0 - x, y0 + y, erase, color, terrain);
		VIEW_drawPixelOnTerrain(x0 + x, y0 - y, erase, color, terrain);
		VIEW_drawPixelOnTerrain(x0 - x, y0 - y, erase, color, terrain);

		VIEW_drawPixelOnTerrain(x0 + y, y0 + x, erase, color, terrain);
		VIEW_drawPixelOnTerrain(x0 - y, y0 + x, erase, color, terrain);
		VIEW_drawPixelOnTerrain(x0 + y, y0 - x, erase, color, terrain);
		VIEW_drawPixelOnTerrain(x0 - y, y0 - x, erase, color, terrain);
	}
}
