//
// Created by maxim on 04.10.2026.
//

#ifndef L3_EDITORSTATE_H
#define L3_EDITORSTATE_H
#pragma once
#include <string>

class EditorState {
public:
	std::string selectedId;
	bool   isDragging = false;
	double dragOffsetX = 0;
	double dragOffsetY = 0;
	bool HasSelection() const { return !selectedId.empty(); }
	void ClearSelection() { selectedId.clear(); }
};

#endif //L3_EDITORSTATE_H
