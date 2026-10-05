//
// Created by maxim on 04.10.2026.
//

#ifndef L3_EDITCONTROLLER_H
#define L3_EDITCONTROLLER_H
#include "IO/LoadService.h"
#include "IO/SaveService.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Cursor.hpp"
#include "Shapes/Picture.h"
#include "State/DrawSelectionFrame.h"
#include "State/EditorState.h"
#include "../IO/portable-file-dialogs.h"
#include "IO/DocumentIO.h"
#include "Shapes/ResizePolicy.h"
#include "View/EditorView.h"
#include "View/FileDialog.h"
#include "View/Toolbar.h"

#include <fstream>
#include <iostream>
#include <random>

namespace shapes {
class Picture;}class EditorController
{
	public:
	EditorController(shapes::Picture& picture,
					 EditorState& state,
					 EditorView& view,
					 sf::RenderWindow& window,
					 Toolbar& toolbar,
					 int canvasWidth = 800,
					 int canvasHeight = 600)
		: m_picture(picture)
		, m_state(state)
		, m_view(view)
		, m_window(window)
		, m_toolbar(toolbar)
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
		if (HandleToolbarClick(pos)) return;
		if (HandleResizeHandleClick(pos)) return;
		HandleShapeClick(pos);
	}
	bool HandleToolbarClick(const sf::Vector2f& pos) {
		auto action = m_toolbar.HitTest(pos.x, pos.y);
		if (action == ToolbarAction::None) return false;
		HandleToolbarAction(action);
		return true;
	}
	bool HandleResizeHandleClick(const sf::Vector2f& pos) {
		if (!m_state.HasValidSelection(m_picture)) return false;
		auto bounds = m_picture.GetShape(m_state.selectedId).GetBounds();
		auto h = HitTestHandle(bounds, pos.x, pos.y);
		if (h == Handle::None) return false;

		m_state.activeHandle = h;
		m_state.resizeStartBounds = bounds;
		m_state.resizeStartMouseX = pos.x;
		m_state.resizeStartMouseY = pos.y;
		m_state.isDragging = false;
		return true;
	}

	void HandleShapeClick(const sf::Vector2f& pos) {
		std::string hit = m_picture.HitTest(pos.x, pos.y);
		if (hit == m_state.selectedId && m_state.HasValidSelection(m_picture)) {
			auto bounds = m_picture.GetShape(hit).GetBounds();
			m_state.isDragging = true;
			m_state.dragOffsetX = pos.x - bounds.x;
			m_state.dragOffsetY = pos.y - bounds.y;
		} else {
			m_state.selectedId = hit;
			m_state.isDragging = false;
		}
	}

	void OnMouseMoved(const sf::Vector2i& pixel)
	{
		sf::Vector2f pos = m_window.mapPixelToCoords(pixel);
		if (!m_state.isDragging && !m_state.IsResizing())
		{
			m_view.UpdateCursor(pos);
		}
		if (m_state.HasValidSelection(m_picture))
		{
			if (m_state.IsResizing())
			{
				Resize(pos);
			}
			if (m_state.isDragging)
			{
				Drag(pos);
			}
		}
	}
	void Resize(const sf::Vector2f& position)
	{
		if (!m_state.HasValidSelection(m_picture)) return;
		double dx = position.x - m_state.resizeStartMouseX;
		double dy = position.y - m_state.resizeStartMouseY;
		auto b = shapes::ResizeAndClamp(m_state.resizeStartBounds, m_state.activeHandle, dx, dy, m_canvasWidth, m_canvasHeight);
		m_picture.SetShapeBounds(m_state.selectedId, b);
	}
	void Drag(const sf::Vector2f& position)
	{
		auto bounds = m_picture.GetShape(m_state.selectedId).GetBounds();
		shapes::Bounds desired{
			position.x - m_state.dragOffsetX,
			position.y - m_state.dragOffsetY,
			bounds.width,
			bounds.height
		};

		auto target = shapes::ClampToCanvas(desired, m_canvasWidth, m_canvasHeight);

		double dx = target.x - bounds.x;
		double dy = target.y - bounds.y;
		if (dx != 0.0 || dy != 0.0)
		{
			m_picture.MoveShape(m_state.selectedId, dx, dy);
		}
	}
	void OnMouseReleased()
	{
		m_state.isDragging = false;
		m_state.activeHandle = Handle::None;
	}
	void OnKeyPressed(const sf::Event::KeyPressed& key)
	{
		if (key.code == sf::Keyboard::Key::Delete && m_state.HasValidSelection(m_picture))
		{
			m_picture.DeleteShape(m_state.selectedId);
			m_state.ClearSelection();
		}
		else if (key.control && key.code == sf::Keyboard::Key::S) {
			SaveDocument();
		}
		else if (key.control && key.code == sf::Keyboard::Key::O) {
			LoadDocument();
		}
		else if (key.control && key.code == sf::Keyboard::Key::N) {
			NewDocument();
		}
	}

	void HandleToolbarAction(ToolbarAction action) {
		switch (action) {
		case ToolbarAction::AddRectangle: AddShapeAtCenter("rectangle"); break;
		case ToolbarAction::AddEllipse:   AddShapeAtCenter("ellipse");   break;
		case ToolbarAction::AddTriangle:  AddShapeAtCenter("triangle");  break;
		case ToolbarAction::Delete:
			if (m_state.HasValidSelection(m_picture)) {
				m_picture.DeleteShape(m_state.selectedId);
				m_state.ClearSelection();
			}
			break;
		default: break;
		}
	}

	void AddShapeAtCenter(const std::string& type) {
		std::string id = MakeId(type);

		double cx = m_canvasWidth / 2.0;
		double cy = m_canvasHeight / 2.0;

		std::unique_ptr<shapes::IShapeGeometry> geo;

		if (type == "rectangle") {
			geo = std::make_unique<shapes::Rectangle>(cx - 60, cy - 40, 120, 80);
		} else if (type == "ellipse") {
			geo = std::make_unique<shapes::Ellipse>(cx, cy, 60, 40);
		} else if (type == "triangle") {
			geo = std::make_unique<shapes::Triangle>(
				cx, cy + 40, cx + 60, cy - 40, cx - 60, cy - 40);
		}

		m_picture.AddShape(std::make_unique<shapes::IFigure>(
			id, "#c678ff", std::move(geo)));

		m_state.selectedId = id;
	}

	void SaveDocument() {
		auto path = m_fileDialog.AskSavePath();
		if (!path) return;
		try
		{
			DocumentIO::Save(m_picture, *path);
		} catch (const std::exception& e)
		{
			m_fileDialog.ShowError(e.what());
		}
	}

	void LoadDocument() {
		auto path = m_fileDialog.AskOpenPath();
		if (!path) return;
		try {
			auto newPicture = DocumentIO::Load(*path);
			m_picture.ReplaceWith(std::move(newPicture));
			m_state.ClearSelection();
		} catch (const std::exception& e) {
			m_fileDialog.ShowError(e.what());
		}
	}

	void NewDocument() {
		shapes::Picture empty;
		m_picture.ReplaceWith(std::move(empty));
		m_state.ClearSelection();
	}
	std::string MakeId(const std::string& prefix) {
		static std::mt19937 rng{std::random_device{}()};
		return prefix + std::to_string(rng());
	}
	shapes::Picture&  m_picture;
	EditorState&      m_state;
	EditorView&       m_view;
	sf::RenderWindow& m_window;
	Toolbar& m_toolbar;
	FileDialog m_fileDialog;
	int m_canvasWidth;
	int m_canvasHeight;
};

#endif //L3_EDITCONTROLLER_H
