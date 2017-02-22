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

void VIEW_drawRectangle(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1, bool_e erase, Uint16 color, const imageInfo_s *terrain) {

	VIEW_drawLine(x0, y0, x1, y0, erase, color, terrain);	//Top
	VIEW_drawLine(x0, y0, x0, y1, erase, color, terrain);	//Left
	VIEW_drawLine(x1, y0, x1, y1, erase, color, terrain);	//Right
	VIEW_drawLine(x0, y1, x1, y1, erase, color, terrain);	//Bottom
}

void VIEW_drawLine(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1, bool_e erase, Uint16 color, const imageInfo_s *terrain) {

	Sint16 dx, dy, sx, sy, err, e2;
	Uint16 tmp;
	SSD2119_Options_t opt = SSD2119_getOptions();

	// Check for overflow
	if (x0 >= opt.width) {
		x0 = opt.width - 1;
	}
	if (x1 >= opt.width) {
		x1 = opt.width - 1;
	}
	if (y0 >= opt.height) {
		y0 = opt.height - 1;
	}
	if (y1 >= opt.height) {
		y1 = opt.height - 1;
	}

	// Check correction
	if (x0 > x1) {
		tmp = x0;
		x0 = x1;
		x1 = tmp;
	}
	if (y0 > y1) {
		tmp = y0;
		y0 = y1;
		y1 = tmp;
	}

	dx = x1 - x0;
	dy = y1 - y0;

	// Vertical or horizontal line
	/*if (dx == 0 || dy == 0) {
		SSD2119_INT_fill(x0, y0, x1, y1, color);
		return;
	}*/

	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;
	err = ((dx > dy) ? dx : -dy) / 2;

	while (1) {
		VIEW_drawPixelOnTerrain(x0, y0, erase, color, terrain);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err;
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		}
	}
}
