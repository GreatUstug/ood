//
// Created by maxim on 04.10.2026.
//

#ifndef L3_EDITCONTROLLER_H
#define L3_EDITCONTROLLER_H
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Cursor.hpp"
#include "Shapes/Picture.h"
#include "State/DrawSelectionFrame.h"
#include "State/EditorState.h"

namespace shapes {
class Picture;}class EditorController
{
	public:
	EditorController(shapes::Picture& picture,
					 EditorState& state,
					 sf::RenderWindow& window,
					 int canvasWidth = 800,
					 int canvasHeight = 600)
		: m_picture(picture)
		, m_state(state)
		, m_window(window)
		, m_canvasWidth(canvasWidth)
		, m_canvasHeight(canvasHeight)
	{}
	void HandleEvent(const sf::Event& event)
	{
		if (const auto* mouse = event.getIf<sf::Event::MouseButtonPressed>())
		{
			if (mouse->button == sf::Mouse::Button::Left)
			{
				OnMousePressed(mouse->position);
			}
		}
		if (const auto* key = event.getIf<sf::Event::KeyPressed>())
		{
			OnKeyPressed(*key);
		}
		if (const auto* moved = event.getIf<sf::Event::MouseMoved>())
		{
			OnMouseMoved(moved->position);
		}
		if (const auto* rel = event.getIf<sf::Event::MouseButtonReleased>())
		{
			if (rel->button == sf::Mouse::Button::Left)
			{
				OnMouseReleased();
			}
		}
	}
	private:
	void OnMousePressed(const sf::Vector2i& pixel)
	{
		sf::Vector2f pos = m_window.mapPixelToCoords(pixel);

		if (m_state.HasSelection() && (m_picture.HasShape(m_state.selectedId)))
		{
			auto bounds = m_picture.GetShape(m_state.selectedId).GetBounds();
			auto h = HitTestHandle(bounds, pos.x, pos.y);
			if (h != EditorState::Handle::None) {
				m_state.activeHandle = h;
				m_state.resizeStartBounds = bounds;
				m_state.resizeStartMouseX = pos.x;
				m_state.resizeStartMouseY = pos.y;
				m_state.isDragging = false;
				return;
			}
		}

		std::string hit = m_picture.HitTest(pos.x, pos.y);
		if (hit == m_state.selectedId && m_state.HasSelection())
		{
			auto bounds = m_picture.GetShape(hit).GetBounds();
			m_state.isDragging = true;
			m_state.dragOffsetX = pos.x - bounds.x;
			m_state.dragOffsetY = pos.y - bounds.y;
		} else
		{
			m_state.selectedId = hit;
			m_state.isDragging = false;
		}
	}
	void OnMouseMoved(const sf::Vector2i& pixel)
	{
		sf::Vector2f pos = m_window.mapPixelToCoords(pixel);
		if (!m_state.isDragging && !m_state.IsResizing())
		{
			UpdateCursor(pos);
		}
		if (m_state.HasSelection() && (m_state.IsResizing() || m_state.isDragging))
		{
			m_state.IsResizing() ? Resize(pos) : Drag(pos);
		}
	};
	void UpdateCursor(const sf::Vector2f& pos)
	{
		if (!m_state.HasSelection() || !m_picture.HasShape(m_state.selectedId)) {
			m_window.setMouseCursor(m_cursorArrow);
			return;
		}

		auto bounds = m_picture.GetShape(m_state.selectedId).GetBounds();
		auto h = HitTestHandle(bounds, pos.x, pos.y);
		switch (h) {
		case EditorState::Handle::NW:
		case EditorState::Handle::SE:
			m_window.setMouseCursor(m_cursorD1);
			break;

		case EditorState::Handle::NE:
		case EditorState::Handle::SW:
			m_window.setMouseCursor(m_cursorD2);
			break;

		case EditorState::Handle::N:
		case EditorState::Handle::S:
			m_window.setMouseCursor(m_cursorV);
			break;

		case EditorState::Handle::E:
		case EditorState::Handle::W:
			m_window.setMouseCursor(m_cursorH);
			break;

		default:
			m_window.setMouseCursor(m_cursorArrow);
			break;
		}
	}
	void Resize(const sf::Vector2f& position)
	{
		auto start = m_state.resizeStartBounds;
		double dx = position.x - m_state.resizeStartMouseX;
		double dy = position.y - m_state.resizeStartMouseY;

		shapes::Bounds b = start;
		switch (m_state.activeHandle) {
		case EditorState::Handle::NW: b.x += dx; b.y += dy; b.width -= dx; b.height -= dy; break;
		case EditorState::Handle::N:  b.y += dy; b.height -= dy; break;
		case EditorState::Handle::NE: b.y += dy; b.width += dx; b.height -= dy; break;
		case EditorState::Handle::E:  b.width += dx; break;
		case EditorState::Handle::SE: b.width += dx; b.height += dy; break;
		case EditorState::Handle::S:  b.height += dy; break;
		case EditorState::Handle::SW: b.x += dx; b.width -= dx; b.height += dy; break;
		case EditorState::Handle::W:  b.x += dx; b.width -= dx; break;
		default: break;
		}
		constexpr double MIN = 20.0;
		if (b.width < MIN) {
			if (m_state.activeHandle == EditorState::Handle::NW ||
				m_state.activeHandle == EditorState::Handle::SW ||
				m_state.activeHandle == EditorState::Handle::W)
				b.x = start.x + start.width - MIN;
			b.width = MIN;
		}
		if (b.height < MIN) {
			if (m_state.activeHandle == EditorState::Handle::NW ||
				m_state.activeHandle == EditorState::Handle::NE ||
				m_state.activeHandle == EditorState::Handle::N)
				b.y = start.y + start.height - MIN;
			b.height = MIN;
		}

		if (b.x < 0) { b.width += b.x; b.x = 0; }
		if (b.y < 0) { b.height += b.y; b.y = 0; }
		if (b.x + b.width > m_canvasWidth)  b.width = m_canvasWidth - b.x;
		if (b.y + b.height > m_canvasHeight) b.height = m_canvasHeight - b.y;

		m_picture.SetShapeBounds(m_state.selectedId, b);
	}
	void Drag(const sf::Vector2f& position)
	{
		auto bounds = m_picture.GetShape(m_state.selectedId).GetBounds();
		double targetX = position.x - m_state.dragOffsetX;
		double targetY = position.y - m_state.dragOffsetY;
		const double W = 800, H = 600;
		if (targetX < 0) targetX = 0;
		if (targetY < 0) targetY = 0;
		if (targetX + bounds.width > W)  targetX = W - bounds.width;
		if (targetY + bounds.height > H) targetY = H - bounds.height;

		double dx = targetX - bounds.x;
		double dy = targetY - bounds.y;
		if (dx != 0.0 || dy != 0.0)
		{
			m_picture.MoveShape(m_state.selectedId, dx, dy);
		}
	}
	void OnMouseReleased()
	{
		m_state.isDragging = false;
		m_state.activeHandle = EditorState::Handle::None;
	}
	void OnKeyPressed(const sf::Event::KeyPressed& key)
	{
		if (key.code == sf::Keyboard::Key::Delete && m_state.HasSelection())
		{
			m_picture.DeleteShape(m_state.selectedId);
			m_state.ClearSelection();
		}
	}
	shapes::Picture&  m_picture;
	EditorState&      m_state;
	sf::RenderWindow& m_window;
	int m_canvasWidth;
	int m_canvasHeight;
	sf::Cursor m_cursorArrow{sf::Cursor::Type::Arrow};
	sf::Cursor m_cursorH{sf::Cursor::Type::SizeHorizontal};
	sf::Cursor m_cursorV{sf::Cursor::Type::SizeVertical};
	sf::Cursor m_cursorD1{sf::Cursor::Type::SizeTopLeftBottomRight};
	sf::Cursor m_cursorD2{sf::Cursor::Type::SizeBottomLeftTopRight};
};

#endif //L3_EDITCONTROLLER_H
