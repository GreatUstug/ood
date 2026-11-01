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
		ShortcutController& shortcuts)
		: m_document(document)
		  , m_state(state)
		  , m_view(view)
		  , m_window(window)
		  , m_toolbar(toolbar)
		  , m_listView(listView)
		  , m_create(create)
		  , m_transform(transform)
		  , m_shortcuts(shortcuts)
	{
	}

	AppState HandleEvent(const sf::Event& event)
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
		return AppState::None;
	}

private:
	void OnMousePressed(const sf::Vector2i& pixel)
	{
		sf::Vector2f pos = m_window.mapPixelToCoords(pixel);

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
		if (m_state.mode == EditorState::Mode::Creating)
		{
			m_create.Commit();
			return;
		}
		m_transform.Release();
	}

	bool HandleToolbarClick(const sf::Vector2f& pos)
	{
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
		case ToolbarAction::SetColorFill:
		{
			if (!m_state.HasValidSelection(m_document.GetPicture())) break;
			std::vector<std::string> ids(m_state.GetSelectedIds().begin(),
										 m_state.GetSelectedIds().end());
			gfx::Color target = btn.color.value();

			bool changed = false;
			for (const auto& id : ids)
				if (m_document.GetPicture().GetShape(id).GetFillColor() != target)
				{ changed = true; break; }
			if (!changed) break;

			m_document.GetHistory().Execute(
				std::make_unique<ColorCommand>(m_document.GetPicture(), ids, target));
			break;
		}
		default:
			break;
		}
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
};

#endif //L3_INPUTROUTER_H