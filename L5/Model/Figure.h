//
// Created by maxim on 09.11.2026.
//

#ifndef L5_FIGURE_H
#define L5_FIGURE_H
#pragma once
#include "Bounds.h"
#include "View/gfx/ICanvas.h"
#include "Figures/IShapeGeometryVisitor.h"
#include <memory>
#include <string>
#include <optional>

namespace shapes
{
class Figure
{
public:
	virtual ~Figure() = default;

	virtual const std::string& GetId() const = 0;

	virtual double GetTime() const = 0;
	virtual void SetTime(double t) = 0;
	virtual double GetDuration() const = 0;
	virtual void SetDuration(double d) = 0;

	virtual gfx::Color GetFillColor() const = 0;
	virtual gfx::Color GetStrokeColor() const = 0;
	virtual void SetFillColor(const gfx::Color& c) = 0;
	virtual void SetStrokeColor(const gfx::Color& c) = 0;

	virtual Bounds GetBounds() const = 0;
	virtual void SetBounds(const Bounds& bounds) = 0;
	virtual void Move(double dx, double dy) = 0;
	virtual bool HitTest(double x, double y) const = 0;

	virtual void Accept(IShapeGeometryVisitor& v) const = 0;

	virtual std::string GetInfo() const = 0;
	virtual std::string GetType() const = 0;

	virtual std::unique_ptr<Figure> Clone() const = 0;
	virtual std::unique_ptr<Figure> ExtractChild(const std::string& id) { return nullptr; }
};
}
#endif //L5_FIGURE_H