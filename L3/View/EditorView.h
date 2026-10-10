//
// Created by maxim on 04.10.2026.
//

#ifndef L3_EDITORVIEW_H
#define L3_EDITORVIEW_H
#include "Toolbar.h"
#include "gfx/ICanvas.h"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include "Model/Picture.h"
#include "DrawSelectionFrame.h"
#include "../Model/EditorState.h"
#include "CursorType.h"
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
	void UpdateCursor(const sf::Vector2f& pos)
	{
		if (!m_state.HasValidSelection(m_picture)) {
			m_window.setMouseCursor(m_cursorArrow);
			return;
		}

		auto bounds = m_picture.GetShape(m_state.selectedId).GetBounds();
		auto h = GetActualHitTestHandle(bounds, pos.x, pos.y);
		switch (h) {
		case Handle::NW:
		case Handle::SE:
			m_window.setMouseCursor(m_cursorD1);
			break;

		case Handle::NE:
		case Handle::SW:
			m_window.setMouseCursor(m_cursorD2);
			break;

		case Handle::N:
		case Handle::S:
			m_window.setMouseCursor(m_cursorV);
			break;

		case Handle::E:
		case Handle::W:
			m_window.setMouseCursor(m_cursorH);
			break;

		default:
			m_window.setMouseCursor(m_cursorArrow);
			break;
		}
	}
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

	void SetCursor(CursorType type) {
		switch (type) {
		case CursorType::Arrow:         m_window.setMouseCursor(m_cursorArrow); break;
		case CursorType::Horizontal:    m_window.setMouseCursor(m_cursorH);     break;
		case CursorType::Vertical:      m_window.setMouseCursor(m_cursorV);     break;
		case CursorType::DiagonalNWSE:  m_window.setMouseCursor(m_cursorD1);    break;
		case CursorType::DiagonalNESW:  m_window.setMouseCursor(m_cursorD2);    break;
		}
	}

	const Toolbar& GetToolbar() const { return m_toolbar; }
	const shapes::Picture& m_picture;
	const EditorState& m_state;
	gfx::ICanvas& m_canvas;
	sf::RenderWindow&      m_window;
	sf::Font&              m_font;
	const Toolbar&			m_toolbar;
	sf::Cursor m_cursorArrow{sf::Cursor::Type::Arrow};
	sf::Cursor m_cursorH{sf::Cursor::Type::SizeHorizontal};
	sf::Cursor m_cursorV{sf::Cursor::Type::SizeVertical};
	sf::Cursor m_cursorD1{sf::Cursor::Type::SizeTopLeftBottomRight};
	sf::Cursor m_cursorD2{sf::Cursor::Type::SizeBottomLeftTopRight};
};
#endif //L3_EDITORVIEW_H
