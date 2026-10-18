//
// Created by maxim on 19.10.2026.
//

#ifndef L3_DRAWVISITOR_H
#define L3_DRAWVISITOR_H

#include "Model/Figures/IShapeGeometryVisitor.h"
#include "Model/Figures/Rectangle.h"
#include "Model/Figures/Ellipse.h"
#include "Model/Figures/Triangle.h"
#include "View/gfx/ICanvas.h"

namespace shapes
{
class DrawVisitor : public IShapeGeometryVisitor
{
public:
	DrawVisitor(gfx::ICanvas& canvas, gfx::Color color)
		: m_canvas(canvas), m_color(color) {}

	void Visit(const Rectangle& r) override
	{
		m_canvas.SetColor(m_color);
		m_canvas.DrawRectangle(r.GetX(), r.GetY(), r.GetW(), r.GetH());
	}
	void Visit(const Ellipse& e) override
	{
		m_canvas.SetColor(m_color);
		m_canvas.DrawEllipse(e.GetCx(), e.GetCy(), e.GetRx(), e.GetRy());
	}
	void Visit(const Triangle& t) override
	{
		m_canvas.SetColor(m_color);
		m_canvas.DrawTriangle(t.GetX1(), t.GetY1(), t.GetX2(), t.GetY2(), t.GetX3(), t.GetY3());
	}

private:
	gfx::ICanvas& m_canvas;
	gfx::Color m_color;
};
}

#endif //L3_DRAWVISITOR_H