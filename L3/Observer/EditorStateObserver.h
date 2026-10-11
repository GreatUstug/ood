//
// Created by maxim on 11.10.2026.
//

#ifndef L3_EDITORSTATEOBSERVER_H
#define L3_EDITORSTATEOBSERVER_H

#include <string>

class EditorStateObserver {
public:
	virtual ~EditorStateObserver() = default;
	virtual void OnSelectionChanged(const std::string& selectedId) = 0;
};

#endif //L3_EDITORSTATEOBSERVER_H
