//
// Created by maxim on 18.10.2026.
//

#ifndef L3_WINDOWCONTROLLER_H
#define L3_WINDOWCONTROLLER_H
#include "EditorController.h"
#include "Commands/CommandHistory.h"
#include "Model/EditorState.h"
#include "Model/Picture.h"
#include "View/EditorView.h"
#include "View/ShapeListView.h"
#include "View/Toolbar.h"
#include "View/gfx/SFMLCanvas.h"

class EditorWindowController
{
	constexpr static unsigned CANVAS_W = 800;
	constexpr static unsigned CANVAS_H = 600;
	constexpr static unsigned PANEL_W = 200;
	constexpr static std::string APP_NAME = "Editor: File ";
	constexpr static std::string DEFAULT_FILE_NAME = "Unsaved";

public:
	explicit EditorWindowController(sf::Font& font, FileDialog& dialog,
		sf::Vector2i pos = { 0, 0 }, const std::string& fileName = DEFAULT_FILE_NAME)
		: m_window(sf::VideoMode(sf::Vector2u(CANVAS_W + PANEL_W, CANVAS_H)), APP_NAME + fileName)
		  , m_canvas(m_window)
		  , m_font(font)
		  , m_view(m_picture, m_state, m_canvas, m_window, m_font, m_toolbar)
		  , m_listView(m_picture, m_state, m_window, m_font,
			  (float)CANVAS_W, 0.f, (float)PANEL_W, (float)CANVAS_H)
		  , m_controller(m_picture, m_state, m_commandHistory, m_view, m_window, m_toolbar,
			  m_listView, dialog, CANVAS_W, CANVAS_H)
	{
		m_window.setPosition(pos);
	}

	bool IsOpen()
	{
		return m_window.isOpen();
	}

	void CloseWindow()
	{
		m_window.close();
	}

	void HandleEvents()
	{
		while (const auto event = m_window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				m_window.close();
				return;
			}
			m_controller.HandleEvent(*event);
		}
	}

	void Render()
	{
		m_window.clear();
		m_view.Render();
		m_listView.Render();
		m_window.display();
	}

	AppCommand TakePendingCommand()
	{
		return m_controller.TakePendingCommand();
	}

	void ReplaceDocument(shapes::Picture&& p)
	{
		m_picture.ReplaceWith(std::move(p));
		m_state.ClearSelection();
	}

private:
	sf::RenderWindow m_window;
	SFMLCanvas m_canvas;
	sf::Font& m_font;

	shapes::Picture m_picture;
	EditorState m_state;
	CommandHistory m_commandHistory;
	Toolbar m_toolbar;

	EditorView m_view;
	ShapeListView m_listView;
	EditorController m_controller;
};
#endif //L3_WINDOWCONTROLLER_H