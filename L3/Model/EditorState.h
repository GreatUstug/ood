//
// Created by maxim on 04.10.2026.
//

#ifndef L3_EDITORSTATE_H
#define L3_EDITORSTATE_H
#pragma once
#include "Handle.h"
#include "Model/Bounds.h"


#include <string>

struct EditorState
{
	enum class Mode
	{
		Idle,
		Dragging,
		Resizing,
		Creating
	};

	std::string selectedId;

	bool HasValidSelection(const shapes::Picture& picture) const
	{
		return !selectedId.empty() && picture.HasShape(selectedId);
	}

	void ClearSelection()
	{
		selectedId.clear();
	}

	Mode mode = Mode::Idle;

	double dragOffsetX = 0;
	double dragOffsetY = 0;

	Handle activeHandle = Handle::None;
	shapes::Bounds resizeStartBounds;
	double resizeStartMouseX = 0;
	double resizeStartMouseY = 0;

	std::string creatingType;
	double createStartX = 0;
	double createStartY = 0;
	double createCurrentX = 0;
	double createCurrentY = 0;
	bool hasCreateStart = false;

	void CancelCreating()
	{
		mode = Mode::Idle;
		creatingType.clear();
		hasCreateStart = false;
	}
};

#endif //L3_EDITORSTATE_H