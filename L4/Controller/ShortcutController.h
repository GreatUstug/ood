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
	ShortcutController(Document& document,
					   EditorState& state,
					   DocumentController& documentCtrl,
					   ShapeCreateController& create)
		: m_document(document)
		, m_state(state)
		, m_documentCtrl(documentCtrl)
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
			m_documentCtrl.Save();
			return AppState::None;
		}
		if (key.control && key.code == sf::Keyboard::Key::O)
		{
			return AppState::OpenInNewWindow;
		}
		if (key.control && key.code == sf::Keyboard::Key::N)
		{
			return AppState::NewWindow;
		}
		if (key.control && key.code == sf::Keyboard::Key::D)
		{
			return AppState::DuplicateWindow;
		}

		if (key.code == sf::Keyboard::Key::Delete)
		{
			if (m_state.HasValidSelection(m_document.GetPicture()))
			{
				std::vector<std::string> selectedIds(m_state.GetSelectedIds().begin(), m_state.GetSelectedIds().end());
				m_document.GetHistory().Execute(std::make_unique<DeleteCommand>(m_document.GetPicture(), selectedIds));
				m_state.ClearSelection();
			}
		}
		if (key.control && key.code == sf::Keyboard::Key::Z && !key.shift)
		{
			std::vector<std::string> affected = m_document.GetHistory().Undo();
			ApplySelection(affected);
			return AppState::None;
		}
		if (key.control && (key.code == sf::Keyboard::Key::Y ||
			(key.code == sf::Keyboard::Key::Z && key.shift)))
		{
			auto affected = m_document.GetHistory().Redo();
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
			if (m_document.GetPicture().HasShape(id)) valid.insert(id);
		m_state.SetSelectedIds(valid);
	}

	Document& m_document;
	EditorState&          m_state;
	DocumentController&   m_documentCtrl;
	ShapeCreateController& m_create;
};

#endif //L3_SHORTCUTCONTROLLER_H