//
// Created by maxim on 12.10.2026.
//

#ifndef L3_SHORTCUTCONTROLLER_H
#define L3_SHORTCUTCONTROLLER_H

#include "AppCommand.h"
#include "Controller/DocumentController.h"
#include "Controller/ShapeCreateController.h"

#include "Model/Picture.h"
#include "Model/EditorState.h"

#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"

class ShortcutController
{
public:
	ShortcutController(shapes::Picture& picture,
					   EditorState& state,
					   DocumentController& document,
					   ShapeCreateController& create)
		: m_picture(picture)
		, m_state(state)
		, m_document(document)
		, m_create(create)
	{
	}

	AppCommand OnKeyPressed(const sf::Event::KeyPressed& key)
	{
		if (key.code == sf::Keyboard::Key::Escape &&
			m_state.mode == EditorState::Mode::Creating)
		{
			m_create.Cancel();
			return AppCommand::None;
		}

		if (key.control && key.code == sf::Keyboard::Key::S)
		{
			m_document.Save();
			return AppCommand::None;
		}
		if (key.control && key.code == sf::Keyboard::Key::O)
		{
			return AppCommand::OpenInNewWindow;
		}
		if (key.control && key.code == sf::Keyboard::Key::N)
		{
			return AppCommand::NewWindow;
		}

		if (key.code == sf::Keyboard::Key::Delete)
		{
			if (m_state.HasValidSelection(m_picture))
			{
				m_picture.DeleteShape(m_state.GetPrimarySelectedId());
				m_state.ClearSelection();
			}
		}
		return AppCommand::None;
	}

private:
	shapes::Picture&      m_picture;
	EditorState&          m_state;
	DocumentController&   m_document;
	ShapeCreateController& m_create;
};

#endif //L3_SHORTCUTCONTROLLER_H