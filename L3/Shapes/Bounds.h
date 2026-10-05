//
// Created by maxim on 04.10.2026.
//

#ifndef L3_BOUNDS_H
#define L3_BOUNDS_H
namespace shapes {
struct Bounds {
	double x = 0;
	double y = 0;
	double width = 0;
	double height = 0;
};
inline Bounds ClampToCanvas(Bounds bounds, double W, double H )
{
		if (bounds.x < 0) { bounds.width += bounds.x; bounds.x = 0; }
		if (bounds.y < 0) { bounds.height += bounds.y; bounds.y = 0; }
		if (bounds.x + bounds.width  > W) bounds.width  = W - bounds.x;
		if (bounds.y + bounds.height > H) bounds.height = H - bounds.y;
		if (bounds.width  < 0) bounds.width  = 0;
		if (bounds.height < 0) bounds.height = 0;
		return bounds;
}
}
#endif //L3_BOUNDS_H
