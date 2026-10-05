//
// Created by maxim on 04.10.2026.
//

#ifndef L3_EDITORVIEW_H
#define L3_EDITORVIEW_H
#include "Toolbar.h"
#include "gfx/ICanvas.h"
#include "Shapes/Picture.h"
#include "State/DrawSelectionFrame.h"
#include "State/EditorState.h"
class EditorView
{
	public:
		EditorView(const shapes::Picture& picture,
			const EditorState& state,
			gfx::ICanvas& canvas,
			sf::RenderWindow& window, sf::Font& font, const Toolbar& toolbar) :
	m_picture(picture), m_state(state), m_canvas(canvas), m_window(window), m_font(font), m_toolbar(toolbar){}
	void Render()
		{
			m_picture.DrawPicture(m_canvas);
			if (m_state.HasValidSelection(m_picture) && m_picture.HasShape(m_state.selectedId)) {
				auto bounds = m_picture.GetShape(m_state.selectedId).GetBounds();
				DrawSelectionFrame(m_canvas, bounds);
			}
			RenderToolbar();
		};

private:
	void RenderToolbar() {
		for (const auto& b : m_toolbar.Buttons()) {
			sf::RectangleShape rect({(float)b.bounds.width, (float)b.bounds.height});
			rect.setPosition({(float)b.bounds.x, (float)b.bounds.y});
			rect.setFillColor(sf::Color(220, 220, 220));
			rect.setOutlineColor(sf::Color::Black);
			rect.setOutlineThickness(1.f);
			m_window.draw(rect);

			sf::Text text(m_font);
			text.setString(b.label);
			text.setCharacterSize(14);
			text.setFillColor(sf::Color::Black);
			text.setPosition({(float)b.bounds.x + 5, (float)b.bounds.y + 5});
			m_window.draw(text);
		}
	}

	const Toolbar& GetToolbar() const { return m_toolbar; }
	const shapes::Picture& m_picture;
	const EditorState& m_state;
	gfx::ICanvas& m_canvas;
	sf::RenderWindow&      m_window;
	sf::Font&              m_font;
	const Toolbar&			m_toolbar;
};
#endif //L3_EDITORVIEW_H
