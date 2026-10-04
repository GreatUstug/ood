//
// Created by maxim on 12.09.2026.
//

#ifndef FIGURES_TRIANGLE_H
#define FIGURES_TRIANGLE_H
#include "IShapeGeometry.h"

namespace shapes
{
class Triangle : public IShapeGeometry
{
public:
	Triangle(double x, double y, double x2, double y2, double x3, double y3) : m_x2(x2), m_y2(y2), m_x3(x3), m_y3(y3)
	{
		m_x = x;
		m_y = y;
	}
	std::string GetInfo() const override {
		return "triangle " +
			   std::to_string(m_x) + " " + std::to_string(m_y) + " " +
			   std::to_string(m_x2) + " " + std::to_string(m_y2) + " " +
			   std::to_string(m_x3) + " " + std::to_string(m_y3);
	}
	void Move(double dx, double dy) override {
		m_x += dx;
		m_y += dy;
		m_x2 += dx;
		m_y2 += dy;
		m_x3 += dx;
		m_y3 += dy;
	}
	void Draw(gfx::ICanvas& canvas) const override {
		canvas.DrawTriangle(m_x, m_y, m_x2, m_y2, m_x3, m_y3);
	}
	Bounds GetBounds() const override {
		double minX = std::min({m_x, m_x2, m_x3});
		double minY = std::min({m_y, m_y2, m_y3});
		double maxX = std::max({m_x, m_x2, m_x3});
		double maxY = std::max({m_y, m_y2, m_y3});
		return {minX, minY, maxX - minX, maxY - minY};
	}
	void SetBounds(const Bounds& bounds) override
	{
		Bounds oldBounds = GetBounds();
		if (oldBounds.width == 0 || oldBounds.height == 0) return;
		double sx = bounds.width  / oldBounds.width;
		double sy = bounds.height / oldBounds.height;

		auto scale = [&](double& px, double& py) {
			px = bounds.x + (px - oldBounds.x) * sx;
			py = bounds.y + (py - oldBounds.y) * sy;
		};
		scale(m_x, m_y);
		scale(m_x2, m_y2);
		scale(m_x3, m_y3);
	};
	virtual ~Triangle() = default;
	bool HitTest(double px, double py) const override {
		auto sign = [](double x1, double y1, double x2, double y2,
					   double x3, double y3) {
			return (x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3);
		};

		double d1 = sign(px, py, m_x,  m_y,  m_x2, m_y2);
		double d2 = sign(px, py, m_x2, m_y2, m_x3, m_y3);
		double d3 = sign(px, py, m_x3, m_y3, m_x,  m_y);

		bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
		bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);

		return !(hasNeg && hasPos);
	}
private:
	double m_x = 0;
	double m_y = 0;
	double m_x2 = 0;
	double m_y2 = 0;
	double m_x3 = 0;
	double m_y3 = 0;
};
}
#endif //FIGURES_TRIANGLE_H