#ifndef VIEW_H
#define VIEW_H

#include "../QS/QS_all.h"
#include "../LCD/middleware.h"

// Stationary platforms
#define PLATFORM_SIZE				(10)
#define PLATFORM_X_LEFT				(0)
#define PLATFORM_X_RIGHT			(PLATFORM_SIZE + TERRAIN_WIDTH)
#define PLATFORM_Y_TOP				(0)
#define PLATFORM_Y_MIDDLE			((TERRAIN_HEIGHT / 2) - (PLATFORM_SIZE / 2))
#define PLATFORM_Y_BOTTOM			(TERRAIN_HEIGHT - PLATFORM_SIZE)

void VIEW_drawEmptyTerrain(const imageInfo_s *terrain);

void VIEW_drawBeaconPosition(color_e color);

void VIEW_drawCircle(Uint16 x0, Uint16 y0, Uint16 r, bool_e erase, Uint16 color, const imageInfo_s *terrain);

#endif /* ndef VIEW_H */
