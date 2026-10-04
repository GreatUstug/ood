//
// Created by maxim on 05.10.2026.
//

#ifndef L3_TOOLBAR_H
#define L3_TOOLBAR_H
#include <vector>

enum class ToolbarAction {
	None,
	AddRectangle,
	AddEllipse,
	AddTriangle,
	Delete
};

struct ToolbarButton {
	shapes::Bounds bounds;
	std::string    label;
	ToolbarAction  action;
};

class Toolbar {
public:
	Toolbar() {
		constexpr double x = 10;
		constexpr double y = 10;
		constexpr double w = 90;
		constexpr double h = 30;
		constexpr double gap = 5;

		m_buttons = {
			{{x,                  y, w, h}, "Rect",   ToolbarAction::AddRectangle},
			{{x + (w + gap),      y, w, h}, "Ellipse",ToolbarAction::AddEllipse},
			{{x + (w + gap)*2,    y, w, h}, "Triangle",ToolbarAction::AddTriangle},
			{{x, y + h + gap,     w, h}, "Delete", ToolbarAction::Delete},
		};
	}

	const std::vector<ToolbarButton>& Buttons() const { return m_buttons; }

	ToolbarAction HitTest(double px, double py) const {
		for (const auto& b : m_buttons) {
			if (px >= b.bounds.x && px <= b.bounds.x + b.bounds.width &&
				py >= b.bounds.y && py <= b.bounds.y + b.bounds.height) {
				return b.action;
				}
		}
		return ToolbarAction::None;
	}

private:
	std::vector<ToolbarButton> m_buttons;
};

#endif //L3_TOOLBAR_H
