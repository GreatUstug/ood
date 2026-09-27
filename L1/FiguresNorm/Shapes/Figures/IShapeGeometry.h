//
// Created by maxim on 12.09.2026.
//

#ifndef FIGURES_SHAPE_H
#define FIGURES_SHAPE_H
#include "../../ShapeParams.h"
#include  "../../gfx/ICanvas.h"

#include <string>

namespace shapes
{
class IShapeGeometry
{
public:
	virtual ~IShapeGeometry() = default;
	virtual std::string GetInfo() const = 0;
	virtual void Move(double dx, double dy) {};
	virtual void Draw(gfx::ICanvas& canvas) const = 0;
};
}

#endif //FIGURES_SHAPE_H