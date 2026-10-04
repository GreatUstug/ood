//
// Created by maxim on 12.09.2026.
//

#ifndef FIGURES_TEXT_H
#define FIGURES_TEXT_H
#include "IShapeGeometry.h"

namespace shapes
{
class Text : public IShapeGeometry
{
public:
	Text(double x, double y, double size, std::string text) : m_size(size), m_text(std::move(text))
	{
		m_x = x;
		m_y = y;
	}
	virtual ~Text() = default;
	void Move(double dx, double dy) override
	{
		m_x += dx;
		m_y += dy;
	};
	std::string GetInfo() const override {
		return "text " +
			   std::to_string(m_x) + " " + std::to_string(m_y) + " " +
			   std::to_string(m_size) + " " + m_text;
	}
	void Draw(gfx::ICanvas& canvas) const override {
		canvas.DrawText(m_x, m_y, m_size, m_text);
	}
private:
	double m_size = 0;
	double m_x = 0;
	double m_y = 0;
	std::string m_text;
};
}
#endif //FIGURES_TEXT_H