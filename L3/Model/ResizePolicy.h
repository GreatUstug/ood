//
// Created by maxim on 05.10.2026.
//

#ifndef L3_RESIZEPOLICY_H
#define L3_RESIZEPOLICY_H
#include "Bounds.h"
#include "Handle.h"
namespace shapes
{
constexpr double MIN_SHAPE_SIZE = 20.0;
inline Bounds ApplyResize(const Bounds& bounds, Handle handle, double dx, double dy)
{
	shapes::Bounds b = bounds;
	switch (handle) {
	case Handle::NW: b.x += dx; b.y += dy; b.width -= dx; b.height -= dy; break;
	case Handle::N:  b.y += dy; b.height -= dy; break;
	case Handle::NE: b.y += dy; b.width += dx; b.height -= dy; break;
	case Handle::E:  b.width += dx; break;
	case Handle::SE: b.width += dx; b.height += dy; break;
	case Handle::S:  b.height += dy; break;
	case Handle::SW: b.x += dx; b.width -= dx; b.height += dy; break;
	case Handle::W:  b.x += dx; b.width -= dx; break;
	default: break;
	}
	if (b.width < MIN_SHAPE_SIZE) {
		if (handle == Handle::NW ||
			handle == Handle::SW ||
			handle == Handle::W)
			b.x = bounds.x + bounds.width - MIN_SHAPE_SIZE;
		b.width = MIN_SHAPE_SIZE;
	}
	if (b.height < MIN_SHAPE_SIZE) {
		if (handle == Handle::NW ||
			handle == Handle::NE ||
			handle == Handle::N)
			b.y = bounds.y + bounds.height - MIN_SHAPE_SIZE;
		b.height = MIN_SHAPE_SIZE;
	}

	return b;
}
inline Bounds ResizeAndClamp(const Bounds& start, Handle handle, double dx, double dy, double W, double H)
{
	return ClampToCanvas(ApplyResize(start, handle, dx, dy), W, H);
}
}
#endif //L3_RESIZEPOLICY_H
