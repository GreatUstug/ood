#ifndef L3_EDITCONTROLLER_H
#define L3_EDITCONTROLLER_H

#include "InputRouter.h"
#include "Commands/CommandHistory.h"
#include "Controller/DocumentController.h"
#include "Controller/InputRouter.h"
#include "Controller/ShapeCreateController.h"
#include "Controller/ShapeTransformController.h"
#include "Controller/ShortcutController.h"

#include "Model/Picture.h"
#include "Model/EditorState.h"

#include "View/EditorView.h"
#include "View/FileDialog.h"
#include "View/ShapeListView.h"
#include "View/Toolbar.h"

#include "SFML/Graphics/RenderWindow.hpp"

class EditorController
{
public:
	EditorController(shapes::Picture& picture,
		EditorState& state,
		CommandHistory& commandHistory,
		EditorView& view,
		sf::RenderWindow& window,
		Toolbar& toolbar,
		ShapeListView& listView,
		FileDialog& fileDialog,
		int canvasWidth,
		int canvasHeight)
		: m_fileDialog(fileDialog)
			,m_commandHistory(commandHistory)
		  , m_create(picture, state, commandHistory, canvasWidth, canvasHeight)
		  , m_transform(picture, state, canvasWidth, canvasHeight)
		  , m_document(picture, state, m_fileDialog)
		  , m_shortcuts(picture, state, commandHistory, m_document, m_create)
		  , m_router(picture, state, view, commandHistory, window, toolbar, listView,
			  m_create, m_transform, m_shortcuts)
	{
	}

	void HandleEvent(const sf::Event& event)
	{
		AppCommand cmd = m_router.HandleEvent(event);
		if (cmd != AppCommand::None)
			m_pendingCommand = cmd;
	}

	AppCommand TakePendingCommand()
	{
		AppCommand c = m_pendingCommand;
		m_pendingCommand = AppCommand::None;
		return c;
	}

private:
	FileDialog& m_fileDialog;
	CommandHistory& m_commandHistory;
	ShapeCreateController m_create;
	ShapeTransformController m_transform;
	DocumentController m_document;
	ShortcutController m_shortcuts;
	InputRouter m_router;
	AppCommand m_pendingCommand;
};

#endif