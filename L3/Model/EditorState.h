#ifndef L3_EDITORSTATE_H
#define L3_EDITORSTATE_H

#include "Handle.h"
#include "Bounds.h"
#include "Observer/ObserverList.h"
#include "Observer/EditorStateObserver.h"

#include <string>

struct EditorState {
	enum class Mode { Idle, Dragging, Resizing, Creating };

	void Subscribe(EditorStateObserver* obs)   { m_observers.AddObserver(obs); }
	void Unsubscribe(EditorStateObserver* obs) { m_observers.RemoveObserver(obs); }

	const std::string& GetSelectedId() const { return selectedId; }

	void SetSelectedId(const std::string& id) {
		if (selectedId == id) return;
		selectedId = id;
		NotifySelectionChanged();
	}

	void ClearSelection() { SetSelectedId(""); }

	bool HasValidSelection(const shapes::Picture& picture) const {
		return !selectedId.empty() && picture.HasShape(selectedId);
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
	bool   hasCreateStart = false;

	void CancelCreating() {
		mode = Mode::Idle;
		creatingType.clear();
		hasCreateStart = false;
	}

private:
	void NotifySelectionChanged() {
		m_observers.Notify([this](EditorStateObserver* o) {
			o->OnSelectionChanged(selectedId);
		});
	}

	std::string selectedId;
	ObserverList<EditorStateObserver> m_observers;
};

#endif