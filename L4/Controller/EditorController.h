#ifndef L3_EDITCONTROLLER_H
#define L3_EDITCONTROLLER_H

#include "InputRouter.h"
#include "DocumentController.h"
#include "ShapeCreateController.h"
#include "ShapeTransformController.h"
#include "ShortcutController.h"

#include "Model/Document.h"
#include "Model/EditorState.h"

#include "View/EditorView.h"
#include "View/FileDialog.h"
#include "View/ShapeListView.h"
#include "View/Toolbar.h"

#include "SFML/Graphics/RenderWindow.hpp"

class EditorController
{
public:
	EditorController(std::shared_ptr<Document> document,
		EditorState& state,
		EditorView& view,
		sf::RenderWindow& window,
		Toolbar& toolbar,
		ShapeListView& listView,
		FileDialog& fileDialog,
		int canvasWidth,
		int canvasHeight,
		TimelineView& timelineView)
		: m_fileDialog(fileDialog)
		  , m_create(*document, state, canvasWidth, canvasHeight)
		  , m_transform(*document, state, canvasWidth, canvasHeight)
		  , m_documentCtrl(*document, state, fileDialog)
		  , m_shortcuts(*document, state, m_documentCtrl, m_create)
		  , m_router(*document, state, view, window, toolbar, listView,
			  m_create, m_transform, m_shortcuts, timelineView)
	{
	}

	void HandleEvent(const sf::Event& event)
	{
		AppState cmd = m_router.HandleEvent(event);
		if (cmd != AppState::None)
			m_pendingCommand = cmd;
	}

	AppState TakePendingCommand()
	{
		AppState c = m_pendingCommand;
		m_pendingCommand = AppState::None;
		return c;
	}

private:
	FileDialog& m_fileDialog;
	ShapeCreateController m_create;
	ShapeTransformController m_transform;
	DocumentController m_documentCtrl;
	ShortcutController m_shortcuts;
	InputRouter m_router;
	AppState m_pendingCommand = AppState::None;
};

#endif