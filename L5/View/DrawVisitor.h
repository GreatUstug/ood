#ifndef L3_DRAWVISITOR_H
#define L3_DRAWVISITOR_H

#include "Model/Figures/IShapeGeometryVisitor.h"
#include "Model/Figures/Rectangle.h"
#include "Model/Figures/Ellipse.h"
#include "Model/Figures/Triangle.h"
#include "Model/Figures/Line.h"
#include "Model/Figures/Polygon.h"
#include "View/gfx/ICanvas.h"

namespace shapes
{
class DrawVisitor : public IShapeGeometryVisitor
{
public:
	DrawVisitor(gfx::ICanvas& canvas, gfx::Color fill, gfx::Color stroke)
		: m_canvas(canvas), m_fill(fill), m_stroke(stroke) {}

	void Visit(const Rectangle& r) override
	{
		m_canvas.DrawRectangle(r.GetX(), r.GetY(), r.GetW(), r.GetH(), m_fill, m_stroke);
	}
	void Visit(const Ellipse& e) override
	{
		m_canvas.DrawEllipse(e.GetCx(), e.GetCy(), e.GetRx(), e.GetRy(), m_fill, m_stroke);
	}
	void Visit(const Triangle& t) override
	{
		m_canvas.DrawTriangle(t.GetX1(), t.GetY1(), t.GetX2(), t.GetY2(),
							  t.GetX3(), t.GetY3(), m_fill, m_stroke);
	}
	void Visit(const Line& l) override
	{
		m_canvas.DrawLine(l.GetX1(), l.GetY1(), l.GetX2(), l.GetY2(), m_stroke);
	}
	void Visit(const Polygon& p) override
	{
		m_canvas.DrawPolygon(p.Vertices(), m_fill, m_stroke);
	}

private:
	gfx::ICanvas& m_canvas;
	gfx::Color m_fill;
	gfx::Color m_stroke;
};
}

#endif //L3_DRAWVISITOR_H