//
// Created by maxim on 12.10.2026.
//

#ifndef L3_INPUTROUTER_H
#define L3_INPUTROUTER_H

#include "Commands/ColorCommand.h"
#include "Controller/ShapeCreateController.h"
#include "Controller/ShapeTransformController.h"
#include "Controller/ShortcutController.h"

#include "Model/Picture.h"
#include "Model/EditorState.h"

#include "View/EditorView.h"
#include "View/ShapeListView.h"
#include "View/Toolbar.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"
#include "View/TimelineView.h"

class InputRouter
{
public:
	InputRouter(Document& document,
		EditorState& state,
		EditorView& view,
		sf::RenderWindow& window,
		Toolbar& toolbar,
		ShapeListView& listView,
		ShapeCreateController& create,
		ShapeTransformController& transform,
		ShortcutController& shortcuts,
		TimelineView& timelineView)
		: m_document(document)
		  , m_state(state)
		  , m_view(view)
		  , m_window(window)
		  , m_toolbar(toolbar)
		  , m_listView(listView)
		  , m_create(create)
		  , m_transform(transform)
		  , m_shortcuts(shortcuts)
		  , m_timeline(timelineView)
	{
	}

	WindowAction HandleEvent(const sf::Event& event)
	{
		if (const auto* mouse = event.getIf<sf::Event::MouseButtonPressed>())
		{
			if (mouse->button == sf::Mouse::Button::Left)
				OnMousePressed(mouse->position);
		}
		if (const auto* key = event.getIf<sf::Event::KeyPressed>())
		{
			return m_shortcuts.OnKeyPressed(*key);
		}
		if (const auto* moved = event.getIf<sf::Event::MouseMoved>())
		{
			OnMouseMoved(moved->position);
		}
		if (const auto* rel = event.getIf<sf::Event::MouseButtonReleased>())
		{
			if (rel->button == sf::Mouse::Button::Left)
				OnMouseReleased();
		}
		if (const auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>())
		{
			const bool ctrl = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
							  sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);

			if (ctrl)
			{
				if (const sf::Vector2f pos = m_window.mapPixelToCoords(wheel->position); m_timeline.HitTest(pos.x, pos.y))
				{
					m_timeline.Zoom(wheel->delta > 0 ? 1.2 : 1.0 / 1.2, pos.x);
				}
			}
		}
		return WindowAction::None;
	}

private:
	void OnMousePressed(const sf::Vector2i& pixel)
	{
		sf::Vector2f pos = m_window.mapPixelToCoords(pixel);
		if (m_timeline.HitTest(pos.x, pos.y))
		{
			if (m_timeline.HandleClick(pos.x, pos.y))
				return;
		}
		if (m_listView.HitTest(pos.x, pos.y))
		{
			m_listView.HandleClick(pos.x, pos.y);
			return;
		}
		if (HandleToolbarClick(pos))
			return;
		if (m_state.mode == EditorState::Mode::Creating)
		{
			m_create.Begin(pos);
			return;
		}
		if (m_transform.TryStartResize(pos))
			return;
		m_transform.HandleShapeClick(pos);
	}

	void OnMouseMoved(const sf::Vector2i& pixel)
	{
		sf::Vector2f pos = m_window.mapPixelToCoords(pixel);
		if (m_timeline.HitTest(pos.x, pos.y) || m_timeline.IsDragging()) {
			m_timeline.UpdatePreview(pos.x, pos.y);
			return;
		}
		switch (m_state.mode)
		{
		case EditorState::Mode::Dragging:
		case EditorState::Mode::Resizing:
			m_transform.Update(pos);
			break;
		case EditorState::Mode::Creating:
			m_create.UpdatePreview(pos);
			break;
		default:
			m_view.UpdateCursor(pos);
		}
	}

	void OnMouseReleased()
	{
		if (auto cmd = m_timeline.Release()) {
			m_document.GetHistory().Execute(std::move(cmd));
			return;
		}
		if (m_state.mode == EditorState::Mode::Creating)
		{
			m_create.Commit();
			return;
		}
		m_transform.Release();
	}

	bool HandleToolbarClick(const sf::Vector2f& pos)
	{
		int idx = m_toolbar.PaletteIndexAt(pos.x, pos.y);
		if (idx >= 0)
		{
			ApplyColorToSelection(m_toolbar.Palette()[idx]);
			return true;
		}
		auto btn = m_toolbar.HitTest(pos.x, pos.y);
		if (!btn)
			return false;
		HandleToolbarAction(*btn);
		return true;
	}

	void HandleToolbarAction(const ToolbarButton& btn)
	{
		switch (btn.action)
		{
		case ToolbarAction::AddRectangle:
			m_create.Start("rectangle");
			break;
		case ToolbarAction::AddEllipse:
			m_create.Start("ellipse");
			break;
		case ToolbarAction::AddTriangle:
			m_create.Start("triangle");
			break;
		case ToolbarAction::AddLine:
			m_create.Start("line");
			break;
		case ToolbarAction::AddPolygon:
			m_create.Start("polygon");
			break;
		case ToolbarAction::Delete:
			if (m_state.HasValidSelection(m_document.GetPicture()))
			{
				std::vector<std::string> selectedIds(m_state.GetSelectedIds().begin(), m_state.GetSelectedIds().end());
				m_document.GetHistory().Execute(std::make_unique<DeleteCommand>(m_document.GetPicture(), selectedIds));
				m_state.ClearSelection();
			}
			break;
		case ToolbarAction::SetFillMode:
			m_state.colorMode = EditorState::ColorMode::Fill;
			break;
		case ToolbarAction::SetStrokeMode:
			m_state.colorMode = EditorState::ColorMode::Stroke;
			break;
		default:
			break;
		}
	}

	void ApplyColorToSelection(const gfx::Color& c)
	{
		if (!m_state.HasValidSelection(m_document.GetPicture()))
			return;

		std::vector<std::string> ids(m_state.GetSelectedIds().begin(),
			m_state.GetSelectedIds().end());

		shapes::ColorTarget target = (m_state.colorMode == EditorState::ColorMode::Fill)
			? shapes::ColorTarget::Fill
			: shapes::ColorTarget::Stroke;

		bool changed = false;
		for (const auto& id : ids)
		{
			const auto& f = m_document.GetPicture().GetShape(id);
			gfx::Color current = (target == shapes::ColorTarget::Fill)
				? f.GetFillColor()
				: f.GetStrokeColor();
			if (current != c)
			{
				changed = true;
				break;
			}
		}
		if (!changed)
			return;

		m_document.GetHistory().Execute(
			std::make_unique<ColorCommand>(m_document.GetPicture(), ids, c, target));
	}

	Document& m_document;
	EditorState& m_state;
	EditorView& m_view;
	sf::RenderWindow& m_window;
	Toolbar& m_toolbar;
	ShapeListView& m_listView;
	ShapeCreateController& m_create;
	ShapeTransformController& m_transform;
	ShortcutController& m_shortcuts;
	TimelineView& m_timeline;
};

#endif //L3_INPUTROUTER_H