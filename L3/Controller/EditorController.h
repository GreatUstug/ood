//
// Created by maxim on 04.10.2026.
//

#ifndef L3_EDITCONTROLLER_H
#define L3_EDITCONTROLLER_H
#include "IO/LoadService.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Cursor.hpp"
#include "Model/Picture.h"
#include "../View/DrawSelectionFrame.h"
#include "../Model/EditorState.h"
#include "../IO/portable-file-dialogs.h"
#include "IO/DocumentIO.h"
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
	      ,m_listView(listView)
		  , m_state(state)
		  , m_view(view)
		  , m_window(window)
		  , m_toolbar(toolbar)
		  , m_canvasWidth(canvasWidth)
		  , m_canvasHeight(canvasHeight)
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

		if (m_listView.HitTest(pos.x, pos.y)) {
			m_listView.HandleClick(pos.x, pos.y);
			return;
		}
		if (m_state.mode == EditorState::Mode::Creating)
		{
			BeginCreateGesture(pos);
			return;
		}
		if (HandleToolbarClick(pos))
			return;
		if (HandleResizeHandlerClick(pos))
			return;
		HandleShapeClick(pos);
	}

	void BeginCreateGesture(const sf::Vector2f& pos)
	{
		m_state.createStartX = pos.x;
		m_state.createStartY = pos.y;
		m_state.createCurrentX = pos.x;
		m_state.createCurrentY = pos.y;
		m_state.hasCreateStart = true;
	}

	bool HandleToolbarClick(const sf::Vector2f& pos)
	{
		auto action = m_toolbar.HitTest(pos.x, pos.y);
		if (action == ToolbarAction::None)
			return false;
		HandleToolbarAction(action);
		return true;
	}

	bool HandleResizeHandlerClick(const sf::Vector2f& pos)
	{
		if (!m_state.HasValidSelection(m_picture))
			return false;
		auto bounds = m_picture.GetShape(m_state.GetSelectedId()).GetBounds();
		auto h = GetActualHitTestHandle(bounds, pos.x, pos.y);
		if (h == Handle::None)
			return false;

		m_state.activeHandle = h;
		m_state.resizeStartBounds = bounds;
		m_state.resizeStartMouseX = pos.x;
		m_state.resizeStartMouseY = pos.y;
		m_state.mode = EditorState::Mode::Resizing;
		return true;
	}

	void HandleShapeClick(const sf::Vector2f& pos)
	{
		std::string hit = m_picture.HitTest(pos.x, pos.y);
		if ((hit == m_state.GetSelectedId()) && m_state.HasValidSelection(m_picture))
		{
			auto bounds = m_picture.GetShape(hit).GetBounds();
			m_state.dragOffsetX = pos.x - bounds.x;
			m_state.dragOffsetY = pos.y - bounds.y;
			m_state.mode = EditorState::Mode::Dragging;
		}
		else
		{
			m_state.SetSelectedId(hit);
			m_state.mode = EditorState::Mode::Idle;
		}
	}

	void OnMouseMoved(const sf::Vector2i& pixel)
	{
		sf::Vector2f pos = m_window.mapPixelToCoords(pixel);
		switch (m_state.mode)
		{
		case EditorState::Mode::Dragging:
			if (m_state.HasValidSelection(m_picture))
				Drag(pos);
			break;
		case EditorState::Mode::Resizing:
			if (m_state.HasValidSelection(m_picture))
				Resize(pos);
			break;
		case EditorState::Mode::Creating:
			if (m_state.hasCreateStart)
			{
				m_state.createCurrentX = pos.x;
				m_state.createCurrentY = pos.y;
			}
			break;
		default:
			m_view.UpdateCursor(pos);
		}

	}

	void Resize(const sf::Vector2f& position)
	{
		if (!m_state.HasValidSelection(m_picture))
			return;
		double dx = position.x - m_state.resizeStartMouseX;
		double dy = position.y - m_state.resizeStartMouseY;
		auto b = shapes::ResizeAndClamp(m_state.resizeStartBounds, m_state.activeHandle, dx, dy, m_canvasWidth, m_canvasHeight);
		m_picture.SetShapeBounds(m_state.GetSelectedId(), b);
	}

	void Drag(const sf::Vector2f& position)
	{
		auto bounds = m_picture.GetShape(m_state.GetSelectedId()).GetBounds();
		shapes::Bounds desired{
			position.x - m_state.dragOffsetX,
			position.y - m_state.dragOffsetY,
			bounds.width,
			bounds.height
		};

		auto target = shapes::ClampToCanvasByShift(desired, m_canvasWidth, m_canvasHeight);

		double dx = target.x - bounds.x;
		double dy = target.y - bounds.y;
		if (dx != 0.0 || dy != 0.0)
		{
			m_picture.MoveShape(m_state.GetSelectedId(), dx, dy);
		}
	}

	void OnMouseReleased()
	{
		if (m_state.mode == EditorState::Mode::Creating)
		{
			if (m_state.hasCreateStart)
				CommitCreating();
			return;
		}
		m_state.activeHandle = Handle::None;
		m_state.mode = EditorState::Mode::Idle;
	}

	void CommitCreating()
	{
		shapes::Bounds b = shapes::BoundsFromPoints(
			m_state.createStartX, m_state.createStartY,
			m_state.createCurrentX, m_state.createCurrentY);

		if (b.width < shapes::MIN_SHAPE_SIZE)
			b.width = shapes::MIN_SHAPE_SIZE;
		if (b.height < shapes::MIN_SHAPE_SIZE)
			b.height = shapes::MIN_SHAPE_SIZE;

		b = shapes::ClampToCanvas(b, m_canvasWidth, m_canvasHeight);

		if (b.width < 1.0 || b.height < 1.0)
		{
			m_state.CancelCreating();
			return;
		}

		std::string id = shapes::MakeId(m_state.creatingType);
		auto figure = shapes::MakeShape(m_state.creatingType, id, b);
		if (!figure)
		{
			m_state.CancelCreating();
			return;
		}

		m_picture.AddShape(std::move(figure));
		m_state.SetSelectedId(id);
		m_state.CancelCreating();
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
			SaveDocument();
		}
		if (key.control && key.code == sf::Keyboard::Key::O)
		{
			LoadDocument();
		}
		if (key.control && key.code == sf::Keyboard::Key::N)
		{
			NewDocument();
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

	void SaveDocument()
	{
		auto path = m_fileDialog.AskSavePath();
		if (!path)
			return;
		try
		{
			DocumentIO::Save(m_picture, *path);
		}
		catch (const std::exception& e)
		{
			m_fileDialog.ShowError(e.what());
		}
	}

	void LoadDocument()
	{
		auto path = m_fileDialog.AskOpenPath();
		if (!path)
			return;
		try
		{
			auto newPicture = DocumentIO::Load(*path);
			m_picture.ReplaceWith(std::move(newPicture));
			m_state.ClearSelection();
		}
		catch (const std::exception& e)
		{
			m_fileDialog.ShowError(e.what());
		}
	}

	void NewDocument()
	{
		shapes::Picture empty;
		m_picture.ReplaceWith(std::move(empty));
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
	int m_canvasWidth;
	int m_canvasHeight;
};

#endif //L3_EDITCONTROLLER_H