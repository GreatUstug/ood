//
// Created by maxim on 12.10.2026.
//

#ifndef L3_SHORTCUTCONTROLLER_H
#define L3_SHORTCUTCONTROLLER_H

#include "AppState.h"
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

	AppState OnKeyPressed(const sf::Event::KeyPressed& key)
	{
		if (key.code == sf::Keyboard::Key::Escape &&
			m_state.mode == EditorState::Mode::Creating)
		{
			m_create.Cancel();
			return AppState::None;
		}

		if (key.control && key.code == sf::Keyboard::Key::S)
		{
			m_document.Save();
			return AppState::None;
		}
		if (key.control && key.code == sf::Keyboard::Key::O)
		{
			return AppState::OpenInNewWindow;
		}
		if (key.control && key.code == sf::Keyboard::Key::N)
		{
			if (key.shift)
			{
				return AppState::DuplicateWindow;
			}
			return AppState::NewWindow;
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
			std::vector<std::string> affected = m_commandHistory.Undo();
			ApplySelection(affected);
			return AppState::None;
		}
		if (key.control && (key.code == sf::Keyboard::Key::Y ||
			(key.code == sf::Keyboard::Key::Z && key.shift)))
		{
			auto affected = m_commandHistory.Redo();
			ApplySelection(affected);
			return AppState::None;
		}
		return AppState::None;
	}

private:
	void ApplySelection(const std::vector<std::string>& ids) const
	{
		std::set<std::string> valid;
		for (const auto& id : ids)
			if (m_picture.HasShape(id)) valid.insert(id);
		m_state.SetSelectedIds(valid);
	}

	shapes::Picture&      m_picture;
	EditorState&          m_state;
	CommandHistory&       m_commandHistory;
	DocumentController&   m_document;
	ShapeCreateController& m_create;
};

#endif //L3_SHORTCUTCONTROLLER_H