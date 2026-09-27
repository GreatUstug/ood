//
// Created by maxim on 12.09.2026.
//

#ifndef FIGURES_RECTANGLE_H
#define FIGURES_RECTANGLE_H
#include "IShapeGeometry.h"

#include <string>


namespace shapes
{
class Rectangle : public IShapeGeometry
{
public:
	Rectangle(double x, double y, double width, double height) : m_width(width), m_height(height)
	{
		m_x = x;
		m_y = y;
	}
	virtual ~Rectangle() = default;
	std::string GetInfo() const override {
		return "rectangle " +
			   std::to_string(m_x) + " " + std::to_string(m_y) + " " +
			   std::to_string(m_width) + " " + std::to_string(m_height);
	}
	void Move(double x, double y)
	{
		m_x += x;
		m_y += y;
	}
	void Draw(gfx::ICanvas& canvas) const override {
		canvas.MoveTo(m_x, m_y);
		canvas.LineTo(m_x + m_width, m_y);
		canvas.LineTo(m_x + m_width, m_y + m_height);
		canvas.LineTo(m_x, m_y + m_height);
		canvas.LineTo(m_x, m_y);
	}
private:
	double m_x = 0;
	double m_y = 0;
	double m_width;
	double m_height;
};
}
#endif //FIGURES_RECTANGLE_H