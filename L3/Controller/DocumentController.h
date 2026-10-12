//
// Created by maxim on 12.10.2026.
//

#ifndef L3_DOCUMENTCONTROLLER_H
#define L3_DOCUMENTCONTROLLER_H

#include "IO/DocumentIO.h"
#include "Model/Picture.h"
#include "Model/EditorState.h"
#include "View/FileDialog.h"

#include <exception>

class DocumentController
{
public:
	DocumentController(shapes::Picture& picture,
					   EditorState& state,
					   FileDialog& dialog)
		: m_picture(picture)
		, m_state(state)
		, m_dialog(dialog)
	{
	}

	void Save()
	{
		auto path = m_dialog.AskSavePath();
		if (!path)
			return;
		try
		{
			DocumentIO::Save(m_picture, *path);
		}
		catch (const std::exception& e)
		{
			m_dialog.ShowError(e.what());
		}
	}

	void Load()
	{
		auto path = m_dialog.AskOpenPath();
		if (!path)
			return;
		try
		{
			auto newPicture = DocumentIO::Load(*path);
			m_picture.ReplaceWith(std::move(newPicture));
			m_state.ClearSelection();
		}
		catch (const std::exception& e)
		{
			m_dialog.ShowError(e.what());
		}
	}

	void New()
	{
		shapes::Picture empty;
		m_picture.ReplaceWith(std::move(empty));
		m_state.ClearSelection();
	}

private:
	shapes::Picture& m_picture;
	EditorState&     m_state;
	FileDialog&      m_dialog;
};

#endif //L3_DOCUMENTCONTROLLER_H