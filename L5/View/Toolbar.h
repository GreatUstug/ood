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
	SetFillMode,
	SetStrokeMode,
};

struct ToolbarButton
{
	shapes::Bounds bounds;
	std::string label;
	ToolbarAction action;
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
			{ { x, y + (h + gap) * 2, w, h }, "Fill", ToolbarAction::SetFillMode },
			{ { x + (w + gap), y + (h + gap) * 2, w, h }, "Stroke", ToolbarAction::SetStrokeMode },
		};
		m_palette = {
			gfx::Color(198, 120, 255, 255),
			gfx::Color(255,  80,  80, 255),
			gfx::Color( 80, 200,  80, 255),
			gfx::Color( 80, 120, 255, 255),
			gfx::Color(255, 200,  60, 255),
			gfx::Color( 40,  40,  40, 255),
		};
	}

	const std::vector<ToolbarButton>& Buttons() const { return m_buttons; }
	const std::vector<gfx::Color>& Palette() const { return m_palette; }


	std::vector<shapes::Bounds> PaletteBounds() const
	{
		constexpr double x = 10;
		constexpr double y = 10 + 3 * (30 + 5);
		constexpr double size = 30;
		constexpr double gap = 5;
		std::vector<shapes::Bounds> result;
		result.reserve(m_palette.size());
		double px = x;
		for (std::size_t i = 0; i < m_palette.size(); ++i)
		{
			result.push_back({ px, y, size, size });
			px += size + gap;
		}
		return result;
	}

	int PaletteIndexAt(double px, double py) const
	{
		auto bounds = PaletteBounds();
		for (std::size_t i = 0; i < bounds.size(); ++i)
		{
			if (px >= bounds[i].x && px <= bounds[i].x + bounds[i].width &&
				py >= bounds[i].y && py <= bounds[i].y + bounds[i].height)
				return static_cast<int>(i);
		}
		return -1;
	}

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
	std::vector<gfx::Color> m_palette;
};

#endif //L3_TOOLBAR_H