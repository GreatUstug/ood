//
// Created by maxim on 04.10.2026.
//

#ifndef L3_EDITCONTROLLER_H
#define L3_EDITCONTROLLER_H
#include "DocumentController.h"
#include "ShapeCreateController.h"
#include "ShapeTransformController.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Cursor.hpp"
#include "Model/Picture.h"
#include "../View/DrawSelectionFrame.h"
#include "../Model/EditorState.h"
#include "../IO/portable-file-dialogs.h"
#include "Model/ResizePolicy.h"
#include "Model/MakeShapeUtils.h"
#include "View/EditorView.h"
#include "View/FileDialog.h"
#include "View/ShapeListView.h"
#include "View/Toolbar.h"

#include <fstream>
#include <random>

class EditorController
{
public:
	EditorController(shapes::Picture& picture,
		EditorState& state,
		EditorView& view,
		sf::RenderWindow& window,
		Toolbar& toolbar,
		ShapeListView& listView,
		int canvasWidth,
		int canvasHeight)
		: m_picture(picture)
		  , m_listView(listView)
		  , m_state(state)
		  , m_view(view)
		  , m_window(window)
		  , m_toolbar(toolbar)
		  , m_document(picture, state, m_fileDialog)
		  , m_transform(picture, state, canvasWidth, canvasHeight)
		  , m_create(picture, state, canvasWidth, canvasHeight)
	{
	}

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

		if (m_listView.HitTest(pos.x, pos.y))
		{
			m_listView.HandleClick(pos.x, pos.y);
			return;
		}
		if (m_state.mode == EditorState::Mode::Creating)
		{
			m_create.Begin(pos);
			return;
		}
		if (HandleToolbarClick(pos))
			return;
		if (m_transform.TryStartResize(pos))
			return;
		m_transform.HandleShapeClick(pos);
	}

	bool HandleToolbarClick(const sf::Vector2f& pos)
	{
		auto action = m_toolbar.HitTest(pos.x, pos.y);
		if (action == ToolbarAction::None)
			return false;
		HandleToolbarAction(action);
		return true;
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
			if (m_state.hasCreateStart)
				m_create.Commit();
			return;
		}
		m_transform.Release();
	}

	void OnKeyPressed(const sf::Event::KeyPressed& key)
	{
		if (key.code == sf::Keyboard::Key::Escape &&
			m_state.mode == EditorState::Mode::Creating)
		{
			m_state.CancelCreating();
			return;
		}
		if (key.code == sf::Keyboard::Key::Delete && m_state.HasValidSelection(m_picture))
		{
			m_picture.DeleteShape(m_state.GetSelectedId());
			m_state.ClearSelection();
		}
		if (key.control && key.code == sf::Keyboard::Key::S)
		{
			m_document.Save();
		}
		if (key.control && key.code == sf::Keyboard::Key::O)
		{
			m_document.Load();
		}
		if (key.control && key.code == sf::Keyboard::Key::N)
		{
			m_document.New();
		}
	}

	void HandleToolbarAction(ToolbarAction action)
	{
		switch (action)
		{
		case ToolbarAction::AddRectangle:
			StartCreating("rectangle");
			break;
		case ToolbarAction::AddEllipse:
			StartCreating("ellipse");
			break;
		case ToolbarAction::AddTriangle:
			StartCreating("triangle");
			break;
		case ToolbarAction::Delete:
			if (m_state.HasValidSelection(m_picture))
			{
				m_picture.DeleteShape(m_state.GetSelectedId());
				m_state.ClearSelection();
			}
			break;
		default:
			break;
		}
	}

	void StartCreating(const std::string& type)
	{
		m_state.mode = EditorState::Mode::Creating;
		m_state.creatingType = type;
		m_state.hasCreateStart = false;
		m_state.ClearSelection();
	}

	std::string MakeId(const std::string& prefix)
	{
		static std::mt19937 rng{ std::random_device{}() };
		return prefix + std::to_string(rng());
	}

	shapes::Picture& m_picture;
	ShapeListView& m_listView;
	EditorState& m_state;
	EditorView& m_view;
	sf::RenderWindow& m_window;
	Toolbar& m_toolbar;
	FileDialog m_fileDialog;
	ShapeCreateController m_create;
	DocumentController m_document;
	ShapeTransformController m_transform;
};

#endif //L3_EDITCONTROLLER_H