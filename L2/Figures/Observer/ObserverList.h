//
// Created by maxim on 21.09.2026.
//

#ifndef FIGURES_OBSERVERLIST_H
#define FIGURES_OBSERVERLIST_H
#include <algorithm>
#include <vector>

template <typename TObserver>
class ObserverList
{
public:
	void AddObserver(TObserver* observer)
	{
		if (!observer) return;
		for (const auto& entry : m_entries)
		{
			if (entry.observer == observer && entry.active)
			{
				return;
			}
		}
		m_entries.push_back({observer, m_generation, true});
	}
	void RemoveObserver(TObserver* observer)
	{
		if (!observer) return;
		for (auto& e : m_entries) {
			if (e.active && e.observer == observer) {
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
		for (std::size_t i = 0; i < snapshotSize; ++i)
		{
			Entry& entry = m_entries[i];
			if (!entry.active) continue;
			if (entry.generation >= snapshotGen) continue;
			fn(m_entries[i].observer);
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
		TObserver*  observer;
		std::size_t generation;
		bool        active;
	};
	std::vector<Entry> m_entries;
	size_t m_generation = 0;
};
#endif //FIGURES_OBSERVERLIST_H