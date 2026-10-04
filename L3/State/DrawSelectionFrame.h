//
// Created by maxim on 04.10.2026.
//

#ifndef L3_DRAWSELECTIONFRAME_H
#define L3_DRAWSELECTIONFRAME_H
#include "EditorState.h"
#include "Shapes/Bounds.h"


#include <array>

const std::size_t COUNT_OF_MARKERS = 8;
const float SIZE_OF_MARKER = 10.0;

inline std::array<std::pair<double, double>, COUNT_OF_MARKERS>
HandlesOf(const shapes::Bounds& b)
{
	double x = b.x, y = b.y, w = b.width, h = b.height;
	return {{
		{x,         y        },
		{x + w/2,   y        },
		{x + w,     y        },
		{x + w,     y + h/2  },
		{x + w,     y + h    },
		{x + w/2,   y + h    },
		{x,         y + h    },
		{x,         y + h/2  },
	}};
}

inline EditorState::Handle HitTestHandle(const shapes::Bounds& b, double px, double py)
{

}

inline void DrawSelectionFrame(gfx::ICanvas& canvas, const shapes::Bounds& bounds)
{
	canvas.SetColor(gfx::Color(0, 0, 255, 255));
	canvas.DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, true);
	for (auto i : HandlesOf(bounds))
	{
		canvas.DrawRectangle(i.first - SIZE_OF_MARKER/2,
							 i.second - SIZE_OF_MARKER/2,
							 SIZE_OF_MARKER,
							 SIZE_OF_MARKER);
	}
}
#endif //L3_DRAWSELECTIONFRAME_H
