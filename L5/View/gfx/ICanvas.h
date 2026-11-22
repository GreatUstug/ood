//
// Created by maxim on 14.09.2026.
//

#ifndef FIGURES_ICANVAS_H
#define FIGURES_ICANVAS_H

// gfx/ICanvas.h
#pragma once
#include "Model/Color.h"

#include <string>
#include <algorithm>
#include <stdexcept>

namespace sf
{
class Color;
}

namespace gfx{

class ICanvas
{
public:
	virtual ~ICanvas() = default;

	virtual void DrawEllipse(double cx, double cy, double rx, double ry,
		const Color& fill, const Color& outline) = 0;

	virtual void DrawRectangle(double x, double y, double w, double h,
		const Color& fill, const Color& outline) = 0;

	virtual void DrawTriangle(double x1, double y1,
		double x2, double y2,
		double x3, double y3,
		const Color& fill, const Color& outline) = 0;

	virtual void DrawLine(double x1, double y1, double x2, double y2,
		const Color& color) = 0;

	virtual void DrawPolygon(const std::vector<std::pair<double, double> >& points,
		const Color& fill, const Color& outline) = 0;
};
}
#endif //FIGURES_ICANVAS_H