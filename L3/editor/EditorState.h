//
// Created by maxim on 04.10.2026.
//

#ifndef L3_EDITORSTATE_H
#define L3_EDITORSTATE_H
#pragma once
#include "Shapes/Bounds.h"


#include <string>

class EditorState {
public:
	std::string selectedId;
	bool   isDragging = false;
	double dragOffsetX = 0;
	double dragOffsetY = 0;
	enum class Handle { None, NW, N, NE, E, SE, S, SW, W };
	Handle activeHandle = Handle::None;
	shapes::Bounds resizeStartBounds;
	double resizeStartMouseX = 0;
	double resizeStartMouseY = 0;
	bool IsResizing() const { return activeHandle != Handle::None; }
	bool HasSelection() const { return !selectedId.empty(); }
	void ClearSelection() { selectedId.clear(); }
};

#endif //L3_EDITORSTATE_H
