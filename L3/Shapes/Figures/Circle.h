//
// Created by maxim on 12.09.2026.
//

#ifndef FIGURES_CIRCLE_H
#define FIGURES_CIRCLE_H
#include "IShapeGeometry.h"

namespace shapes
{
class Circle : public IShapeGeometry
{
public:
	Circle(double x, double y, double radius) : m_radius(radius)
	{
		m_x = x;
		m_y = y;
	}
	void Move(double x, double y)
	{
		m_x += x;
		m_y += y;
	}
	virtual ~Circle() = default;
	std::string GetInfo() const override {
		return "circle " +
		   std::to_string(m_x) + " " + std::to_string(m_y) + " " +
		   std::to_string(m_radius);
	}
	void Draw(gfx::ICanvas& canvas) const override {
		canvas.DrawEllipse(m_x, m_y, m_radius, m_radius);
	}
	Bounds GetBounds() const override {
		return {m_x - m_radius, m_y - m_radius, 2 * m_radius, 2 * m_radius};
	}
	bool HitTest(double px, double py) const override {
		if (m_radius <= 0) return false;
		double dx = px - m_x;
		double dy = py - m_y;
		return (dx*dx + dy*dy) <= m_radius * m_radius;
	}
private:
	double m_radius;
	double m_x = 0;
	double m_y = 0;
};
}

#endif //FIGURES_CIRCLE_H