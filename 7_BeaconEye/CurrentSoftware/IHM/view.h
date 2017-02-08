#ifndef VIEW_H
	#define VIEW_H

	#include "../QS/QS_all.h"
	#include "../LCD/middleware.h"

	void VIEW_drawEmptyTerrain(const imageInfo_s *terrain);

	void VIEW_drawBeaconPosition(color_e color);

	void VIEW_drawCircle(Uint16 x0, Uint16 y0, Uint16 r, bool_e erase, Uint16 color, const imageInfo_s *terrain);

#endif /* ndef VIEW_H */
