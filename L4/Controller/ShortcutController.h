//
// Created by maxim on 12.10.2026.
//

#ifndef L3_SHORTCUTCONTROLLER_H
#define L3_SHORTCUTCONTROLLER_H

#include "AppCommand.h"
#include "Commands/DeleteCommand.h"
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
					   CommandHistory& commandHistory,
					   DocumentController& document,
					   ShapeCreateController& create)
		: m_picture(picture)
		, m_state(state)
		, m_commandHistory(commandHistory)
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
				std::vector<std::string> selectedIds(m_state.GetSelectedIds().begin(), m_state.GetSelectedIds().end());
				m_commandHistory.Execute(std::make_unique<DeleteCommand>(m_picture, selectedIds));
				m_state.ClearSelection();
			}
		}
		if (key.control && key.code == sf::Keyboard::Key::Z && !key.shift)
		{
			m_commandHistory.Undo();
			PruneSelection();
			return AppCommand::None;
		}
		if (key.control && (key.code == sf::Keyboard::Key::Y ||
			(key.code == sf::Keyboard::Key::Z && key.shift)))
		{
			m_commandHistory.Redo();
			PruneSelection();
			return AppCommand::None;
		}
		return AppCommand::None;
	}

private:
	void PruneSelection() const
	{
		std::set<std::string> valid;
		for (const auto& id : m_state.GetSelectedIds())
			if (m_picture.HasShape(id)) valid.insert(id);
		if (valid != m_state.GetSelectedIds())
			m_state.SetSelectedIds(valid);
	}

	shapes::Picture&      m_picture;
	EditorState&          m_state;
	CommandHistory&       m_commandHistory;
	DocumentController&   m_document;
	ShapeCreateController& m_create;
};

#endif //L3_SHORTCUTCONTROLLER_H