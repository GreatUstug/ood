#ifndef L3_DRAWVISITOR_H
#define L3_DRAWVISITOR_H

#include "Model/GroupFigure.h"
#include "Model/Figures/IShapeGeometryVisitor.h"
#include "Model/Figures/Rectangle.h"
#include "Model/Figures/Ellipse.h"
#include "Model/Figures/Triangle.h"
#include "Model/Figures/Line.h"
#include "Model/Figures/Polygon.h"
#include "View/gfx/ICanvas.h"
#include "Model/EditorState.h"

namespace shapes
{
class DrawVisitor : public IShapeGeometryVisitor
{
public:
	DrawVisitor(gfx::ICanvas& canvas, Color fill, Color stroke,
	            const std::unordered_map<std::string, PreviewShape>* preview = nullptr)
		: m_canvas(canvas), m_fill(fill), m_stroke(stroke), m_preview(preview) {}

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

	void Visit(const GroupFigure& gp) override
	{
		Color oldFill = m_fill;
		Color oldStroke = m_stroke;
		for (const auto& child : gp.GetChildren())
		{
			if (m_preview)
			{
				auto it = m_preview->find(child->GetId());
				if (it != m_preview->end() && it->second.bounds)
				{
					auto clone = child->Clone();
					clone->SetBounds(*it->second.bounds);

					m_fill = clone->GetFillColor();
					m_stroke = clone->GetStrokeColor();
					clone->Accept(*this);
					continue;
				}
			}

			m_fill = child->GetFillColor();
			m_stroke = child->GetStrokeColor();
			child->Accept(*this);
		}
		m_fill = oldFill;
		m_stroke = oldStroke;
	}
private:
	gfx::ICanvas& m_canvas;
	Color m_fill;
	Color m_stroke;
	const std::unordered_map<std::string, PreviewShape>* m_preview;
};
}

#endif //L3_DRAWVISITOR_H