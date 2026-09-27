//
// Created by maxim on 12.09.2026.
//

#ifndef FIGURES_LINE_H
#define FIGURES_LINE_H
#include "IShapeGeometry.h"

#include <string>
namespace shapes
{
class Line : public IShapeGeometry
{
public:
	Line(double x, double y, double xEnd, double yEnd) : m_xEnd(xEnd), m_yEnd(yEnd)
	{
		m_x = x;
		m_y = y;
	}
	virtual ~Line() = default;
	std::string GetInfo() const override {
		return "line " +
			   std::to_string(m_x) + " " + std::to_string(m_y) + " " +
			   std::to_string(m_xEnd) + " " + std::to_string(m_yEnd);
	}
	void Draw(gfx::ICanvas& canvas) const override {
		canvas.MoveTo(m_x, m_y);
		canvas.LineTo(m_xEnd, m_yEnd);
	}
	void Move(double dx, double dy) override {
		m_x += dx;
		m_y += dy;
		m_xEnd += dx;
		m_yEnd += dy;
	}
private:
	double m_x = 0;
	double m_y = 0;
	double m_xEnd = 0;
	double m_yEnd = 0;
};
}
#endif //FIGURES_LINE_H