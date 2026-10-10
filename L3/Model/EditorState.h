//
// Created by maxim on 04.10.2026.
//

#ifndef L3_EDITORSTATE_H
#define L3_EDITORSTATE_H
#pragma once
#include "Handle.h"
#include "Model/Bounds.h"


#include <string>

struct EditorState {
	bool HasValidSelection(const shapes::Picture& picture) const {
		return !selectedId.empty() && picture.HasShape(selectedId);
	}
	bool IsResizing() const { return activeHandle != Handle::None; }
	void ClearSelection() { selectedId.clear(); }
	std::string selectedId;
	bool   isDragging = false;
	double dragOffsetX = 0;
	double dragOffsetY = 0;
	Handle activeHandle = Handle::None;
	shapes::Bounds resizeStartBounds;
	double resizeStartMouseX = 0;
	double resizeStartMouseY = 0;
};

#endif //L3_EDITORSTATE_H
