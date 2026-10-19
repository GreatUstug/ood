#ifndef FIGURES_OBSERVERLISTBYID_H
#define FIGURES_OBSERVERLISTBYID_H
#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

template <typename TObserver>
class ObserverListById {
public:
	using Id = std::size_t;

	Id AddObserver(TObserver observer) {
		const Id id = m_nextId++;
		m_entries.push_back({id, std::move(observer), m_generation, true});
		return id;
	}

	void RemoveObserver(Id id) {
		for (auto& e : m_entries) {
			if (e.active && e.id == id) {
				e.active = false;
				return;
			}
		}
	}

	template <typename Fn>
	void Notify(Fn&& fn) {
		++m_generation;
		const std::size_t snapshotGen  = m_generation;
		const std::size_t snapshotSize = m_entries.size();

		for (std::size_t i = 0; i < snapshotSize; ++i) {
			Entry& entry = m_entries[i];
			if (!entry.active) continue;
			if (entry.generation >= snapshotGen) continue;
			fn(entry.observer);
		}

		m_entries.erase(
			std::remove_if(m_entries.begin(), m_entries.end(),
						   [](const Entry& e) { return !e.active; }),
			m_entries.end());
	}

	std::size_t CountActive() const {
		return static_cast<std::size_t>(std::count_if(
			m_entries.begin(), m_entries.end(),
			[](const Entry& e) { return e.active; }));
	}

private:
	struct Entry {
		Id          id;
		TObserver   observer;
		std::size_t generation;
		bool        active;
	};

	std::vector<Entry> m_entries;
	Id          m_nextId    = 0;
	std::size_t m_generation = 0;
};

#endif //FIGURES_OBSERVERLISTBYID_H