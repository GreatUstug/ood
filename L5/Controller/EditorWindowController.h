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
		sf::Vector2f baseSize(
			static_cast<float>(CANVAS_W + PANEL_W),
			static_cast<float>(CANVAS_H + TIMELINE_H));
		m_camera = sf::View(baseSize / 2.f, baseSize);
		m_window.setView(m_camera);
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

			if (const auto* resized = event->getIf<sf::Event::Resized>())
			{
				float baseW = static_cast<float>(CANVAS_W + PANEL_W);
				float baseH = static_cast<float>(CANVAS_H + TIMELINE_H);

				float winW = static_cast<float>(resized->size.x);
				float winH = static_cast<float>(resized->size.y);

				float sizeX = baseW / winW;
				float sizeY = baseH / winH;

				if (sizeX > 1.f || sizeY > 1.f) {
					float scale = std::min(1.f / sizeX, 1.f / sizeY);
					sizeX *= scale;
					sizeY *= scale;
				}

				float posX = (1.f - sizeX) / 2.f;
				float posY = (1.f - sizeY) / 2.f;

				m_camera.setViewport(sf::FloatRect({posX, posY}, {sizeX, sizeY}));

				m_window.setView(m_camera);
			}

			if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>())
			{
				bool ctrl = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
					sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
				if (ctrl)
				{
					sf::Vector2f worldPos = m_window.mapPixelToCoords(wheel->position, m_camera);
					if (!m_timeline.HitTest(worldPos.x, worldPos.y))
					{
						sf::Vector2f mousePos(
							static_cast<float>(wheel->position.x),
							static_cast<float>(wheel->position.y));
						float factor = (wheel->delta > 0) ? 1.1f : 1.0f / 1.1f;
						ZoomAt(factor, mousePos);
						return;
					}
				}
			}

			if (const auto* key = event->getIf<sf::Event::KeyPressed>())
			{
				bool ctrl = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
					sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
				if (ctrl && (key->code == sf::Keyboard::Key::Equal || key->code == sf::Keyboard::Key::Add))
				{
					sf::Vector2f center(m_window.getSize().x / 2.f, m_window.getSize().y / 2.f);
					ZoomAt(1.2f, center);
				}
				if (ctrl && (key->code == sf::Keyboard::Key::Hyphen || key->code == sf::Keyboard::Key::Subtract))
				{
					sf::Vector2f center(m_window.getSize().x / 2.f, m_window.getSize().y / 2.f);
					ZoomAt(1.0f / 1.2f, center);
				}
				if (ctrl && key->code == sf::Keyboard::Key::Num0)
				{
					ResetZoom();
				}
			}

			m_controller.HandleEvent(*event);
		}
	}

	void Render()
	{
		m_window.clear(sf::Color::Transparent);
		m_window.setView(m_camera);
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

	void ZoomAt(float factor, const sf::Vector2f& screenPos)
	{
		float newZoom = std::clamp(m_zoom * factor, MIN_ZOOM, MAX_ZOOM);
		if (std::abs(newZoom - m_zoom) < 1e-6f)
			return;

		sf::Vector2f worldBefore = m_window.mapPixelToCoords(
			sf::Vector2i(screenPos.x, screenPos.y), m_camera);

		m_zoom = newZoom;

		sf::Vector2f baseSize(
			static_cast<float>(CANVAS_W + PANEL_W),
			static_cast<float>(CANVAS_H + TIMELINE_H));
		m_camera.setSize(baseSize / m_zoom);

		sf::Vector2f worldAfter = m_window.mapPixelToCoords(
			sf::Vector2i(screenPos.x, screenPos.y), m_camera);
		m_camera.move(worldBefore - worldAfter);

		m_window.setView(m_camera);
	}

private:
	float m_zoom = 1.0f;
	const float MIN_ZOOM = 0.1f;
	const float MAX_ZOOM = 10.0f;
	std::shared_ptr<Document> m_document;
	sf::RenderWindow m_window;
	gfx::SFMLCanvas m_canvas;
	sf::Font& m_font;

	sf::View m_camera;

	EditorState m_state;
	Toolbar m_toolbar;

	EditorView m_view;
	ShapeListView m_listView;
	TimelineView m_timeline;
	EditorController m_controller;

	void ResetZoom()
	{
		m_zoom = 1.0f;
		sf::Vector2f baseSize(
			static_cast<float>(CANVAS_W + PANEL_W),
			static_cast<float>(CANVAS_H + TIMELINE_H));
		m_camera.setSize(baseSize);
		m_camera.setCenter(baseSize / 2.f);
		m_window.setView(m_camera);
	}
};
#endif //L3_WINDOWCONTROLLER_H