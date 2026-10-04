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
	virtual ~Triangle() = default;
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