#ifndef L3_EDITORSTATE_H
#define L3_EDITORSTATE_H

#include "Handle.h"
#include "Bounds.h"
#include "Observer/ObserverList.h"
#include "Observer/EditorStateObserver.h"

#include <set>
#include <string>

struct EditorState
{
	enum class Mode { Idle, Dragging, Resizing, Creating };

	void Subscribe(EditorStateObserver* obs) { m_observers.AddObserver(obs); }
	void Unsubscribe(EditorStateObserver* obs) { m_observers.RemoveObserver(obs); }


	[[nodiscard]] const std::set<std::string>& GetSelectedIds() const { return m_selectedIds; }
	[[nodiscard]] const std::string& GetPrimarySelectedId() const { return m_primarySelectedId; }
	[[nodiscard]] bool IsSelected(const std::string& id) const { return m_selectedIds.contains(id); }
	[[nodiscard]] bool HasAnySelection() const { return !m_selectedIds.empty(); }

	void SetSelectedIds(const std::set<std::string>& selectedIds)
	{
		if (selectedIds == m_selectedIds) { return; }
		m_selectedIds = selectedIds;
		if (!m_selectedIds.contains(m_primarySelectedId))
			m_primarySelectedId = m_selectedIds.empty() ? std::string{} : *m_selectedIds.begin();
		NotifySelectionChanged();
	}

	void SetPrimarySelectedId(const std::string& id)
	{
		if (m_primarySelectedId == id)
			return;
		m_primarySelectedId = id;
		NotifySelectionChanged();
	}

	void ClearSelection()
	{
		if (m_selectedIds.empty() && m_primarySelectedId.empty())
			return;
		m_selectedIds.clear();
		m_primarySelectedId.clear();
		NotifySelectionChanged();
	}

	[[nodiscard]] bool HasValidSelection(const shapes::Picture& picture) const
	{
		for (const auto& id : m_selectedIds)
		{
			if (picture.HasShape(id))
				return true;
		}
		return false;
	}

	void ToggleSelected(const std::string& id)
	{
		if (m_selectedIds.count(id))
		{
			m_selectedIds.erase(id);
			if (m_primarySelectedId == id)
				m_primarySelectedId = m_selectedIds.empty() ? std::string{} : *m_selectedIds.begin();
		}
		else
		{
			m_selectedIds.insert(id);
			m_primarySelectedId = id;
		}
		NotifySelectionChanged();
	}

	Mode mode = Mode::Idle;

	double dragAnchorX = 0;
	double dragAnchorY = 0;
	std::map<std::string, shapes::Bounds> dragStartBounds;
	double lastAppliedDx = 0;
	double lastAppliedDy = 0;

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

private:
	void NotifySelectionChanged()
	{
		m_observers.Notify([this](EditorStateObserver* o) {
			o->OnSelectionChanged(m_selectedIds);
		});
	}

	std::string m_primarySelectedId;
	std::set<std::string> m_selectedIds;
	ObserverList<EditorStateObserver> m_observers;
};

#endif