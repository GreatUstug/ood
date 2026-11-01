//
// Created by maxim on 05.10.2026.
//

#ifndef L3_TOOLBAR_H
#define L3_TOOLBAR_H
#include <vector>

enum class ToolbarAction
{
	None,
	AddRectangle,
	AddEllipse,
	AddTriangle,
	AddLine,
	AddPolygon,
	Delete,
	SetColorFill,
	SetColorStroke,
};

struct ToolbarButton
{
	shapes::Bounds bounds;
	std::string label;
	ToolbarAction action;
	std::optional<gfx::Color> color;
};

class Toolbar
{
public:
	Toolbar()
	{
		constexpr double x = 10;
		constexpr double y = 10;
		constexpr double w = 90;
		constexpr double h = 30;
		constexpr double gap = 5;

		m_buttons = {
			{ { x, y, w, h }, "Rect", ToolbarAction::AddRectangle },
			{ { x + (w + gap), y, w, h }, "Ellipse", ToolbarAction::AddEllipse },
			{ { x + (w + gap) * 2, y, w, h }, "Triangle", ToolbarAction::AddTriangle },
			{ { x + (w + gap) * 3, y, w, h }, "Line", ToolbarAction::AddLine },
			{ { x + (w + gap) * 4, y, w, h }, "Polygon", ToolbarAction::AddPolygon },
			{ { x, y + h + gap, w, h }, "Delete", ToolbarAction::Delete },
		};
		const std::vector<gfx::Color> palette = {
			gfx::Color(198, 120, 255, 255),
			gfx::Color(255,  80,  80, 255),
			gfx::Color( 80, 200,  80, 255),
			gfx::Color( 80, 120, 255, 255),
			gfx::Color(255, 200,  60, 255),
		};
		double px = x;
		constexpr double py = y + (h + gap) * 2;
		for (const auto& c : palette)
		{
			m_buttons.push_back({ { px, py, h, h }, "", ToolbarAction::SetColorFill, c });
			px += h + gap;
		}
	}

	const std::vector<ToolbarButton>& Buttons() const { return m_buttons; }

	const ToolbarButton* HitTest(double px, double py) const
	{
		for (const auto& b : m_buttons)
		{
			if (px >= b.bounds.x && px <= b.bounds.x + b.bounds.width &&
				py >= b.bounds.y && py <= b.bounds.y + b.bounds.height)
			{
				return &b;
			}
		}
		return nullptr;
	}

private:
	std::vector<ToolbarButton> m_buttons;
};

#endif //L3_TOOLBAR_H