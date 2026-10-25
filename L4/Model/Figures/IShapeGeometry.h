//
// Created by maxim on 12.09.2026.
//

#ifndef FIGURES_SHAPE_H
#define FIGURES_SHAPE_H
#include "IShapeGeometryVisitor.h"
#include "Model/Bounds.h"

#include <string>

namespace shapes
{
class IShapeGeometry
{
public:
	virtual ~IShapeGeometry() = default;

	virtual void Accept(IShapeGeometryVisitor& v) const = 0;

	virtual std::string GetInfo() const = 0;

	virtual void Move(double dx, double dy)
	{
	};

	virtual Bounds GetBounds() const = 0;

	virtual std::string GetType() const = 0;

	virtual void SetBounds(const Bounds& bounds) = 0;

	virtual bool HitTest(double x, double y) const = 0;
};
}

#endif //FIGURES_SHAPE_H