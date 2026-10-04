//
// Created by maxim on 12.09.2026.
//

#ifndef FIGURES_CIRCLE_H
#define FIGURES_CIRCLE_H
#include "IShapeGeometry.h"
#include "../Bounds.h"

namespace shapes
{
class Ellipse : public IShapeGeometry
{
public:
	Ellipse(double x, double y, double radiusX, double radiusY) : m_radiusX(radiusX), m_radiusY(radiusY)
	{
		m_x = x;
		m_y = y;
	}
	void Move(double x, double y)
	{
		m_x += x;
		m_y += y;
	}
	virtual ~Ellipse() = default;
	std::string GetInfo() const override {
		return "ellipse " +
		   std::to_string(m_x) + " " + std::to_string(m_y) + " " +
		   std::to_string(m_radiusX) + " " + std::to_string(m_radiusY);
	}
	void Draw(gfx::ICanvas& canvas) const override {
		canvas.DrawEllipse(m_x, m_y, m_radiusX, m_radiusY);
	}
	Bounds GetBounds() const override {
		return {m_x - m_radiusX, m_y - m_radiusY, 2 * m_radiusX, 2 * m_radiusY};
	}
	void SetBounds(const Bounds& bounds) override
	{
		m_x = bounds.x + bounds.width / 2;
		m_y = bounds.y + bounds.height / 2;
		m_radiusX = bounds.width / 2;
		m_radiusY = bounds.height / 2;
	}
	bool HitTest(double px, double py) const override {
		if (m_radiusX <= 0 || m_radiusY <= 0) return false;
		double dx = (px - m_x)/m_radiusX;
		double dy = (py - m_y)/m_radiusY;
		return dx * dx + dy * dy <= 1.0;
	}
private:
	double m_radiusX;
	double m_radiusY;
	double m_x = 0;
	double m_y = 0;
};
}

#endif //FIGURES_CIRCLE_H