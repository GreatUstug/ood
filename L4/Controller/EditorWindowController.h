//
// Created by maxim on 18.10.2026.
//

#ifndef L3_WINDOWCONTROLLER_H
#define L3_WINDOWCONTROLLER_H
#include "EditorController.h"
#include "Model/Document.h"
#include "Model/EditorState.h"
#include "Model/Picture.h"
#include "View/EditorView.h"
#include "View/ShapeListView.h"
#include "View/TimelineView.h"
#include "View/Toolbar.h"
#include "View/gfx/SFMLCanvas.h"

class EditorWindowController
{
	constexpr static unsigned CANVAS_W = 800;
	constexpr static unsigned CANVAS_H = 600;
	constexpr static unsigned PANEL_W = 200;
	constexpr static unsigned TIMELINE_H = 150;
	constexpr static std::string APP_NAME = "Editor: File ";
	constexpr static std::string DEFAULT_FILE_NAME = "Unsaved";

public:
	explicit EditorWindowController(sf::Font& font, FileDialog& dialog, std::shared_ptr<Document> document,
		sf::Vector2i pos = { 0, 0 }, const std::string& fileName = DEFAULT_FILE_NAME)
		: m_document(std::move(document))
		  , m_window(sf::VideoMode(sf::Vector2u(CANVAS_W + PANEL_W, CANVAS_H + TIMELINE_H)), APP_NAME + fileName)
		  , m_canvas(m_window)
		  , m_font(font)
		  , m_view(m_document->GetPicture(), m_state, m_canvas, m_window, m_font, m_toolbar)
		  , m_listView(m_document->GetPicture(), m_state, m_window, m_font,
			  (float)CANVAS_W, 0.f, (float)PANEL_W, (float)CANVAS_H)
		  , m_timeline(m_document->GetPicture(), m_state, m_window, m_font, 0.f, (float)CANVAS_H, (float)(CANVAS_W + PANEL_W), (float)TIMELINE_H)
		  , m_controller(m_document, m_state, m_view, m_window, m_toolbar,
			  m_listView, dialog, CANVAS_W, CANVAS_H, m_timeline)
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
		m_timeline.Render();
		m_window.display();
	}

	WindowAction TakePendingCommand()
	{
		return m_controller.TakePendingCommand();
	}

	void ReplaceDocument(shapes::Picture&& p)
	{
		m_document->GetPicture().ReplaceWith(std::move(p));
		m_state.ClearSelection();
	}

	std::shared_ptr<Document> GetDocument() const { return m_document; }

private:
	std::shared_ptr<Document> m_document;
	sf::RenderWindow m_window;
	gfx::SFMLCanvas m_canvas;
	sf::Font& m_font;

	EditorState m_state;
	Toolbar m_toolbar;

	EditorView m_view;
	ShapeListView m_listView;
	TimelineView m_timeline;
	EditorController m_controller;
};
#endif //L3_WINDOWCONTROLLER_H