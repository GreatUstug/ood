#ifndef L3_EDITCONTROLLER_H
#define L3_EDITCONTROLLER_H

#include "InputRouter.h"
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
					 EditorView& view,
					 sf::RenderWindow& window,
					 Toolbar& toolbar,
					 ShapeListView& listView,
					 int canvasWidth,
					 int canvasHeight)
		: m_fileDialog()
		, m_create(picture, state, canvasWidth, canvasHeight)
		, m_transform(picture, state, canvasWidth, canvasHeight)
		, m_document(picture, state, m_fileDialog)
		, m_shortcuts(picture, state, m_document, m_create)
		, m_router(picture, state, view, window, toolbar, listView,
				   m_create, m_transform, m_shortcuts)
	{
	}

	void HandleEvent(const sf::Event& event)
	{
		m_router.HandleEvent(event);
	}

private:
	FileDialog                 m_fileDialog;
	ShapeCreateController      m_create;
	ShapeTransformController   m_transform;
	DocumentController         m_document;
	ShortcutController         m_shortcuts;
	InputRouter                m_router;
};

#endif