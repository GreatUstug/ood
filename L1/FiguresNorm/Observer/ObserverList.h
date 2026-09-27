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
		if (std::find(m_observers.begin(), m_observers.end(), observer) != m_observers.end())
			return;
		m_observers.push_back(observer);
	}
	void RemoveObserver(TObserver* observer)
	{
		if (!observer) return;
		m_observers.erase(
			std::remove(m_observers.begin(), m_observers.end(), observer),
			m_observers.end());
	}

	template <typename Fn>
	void Notify(Fn&& fn) {
		auto copy = m_observers;
		for (auto* obs : copy) {
			fn(obs);
		}
	}

	std::size_t Size() const { return m_observers.size(); }
private:
	std::vector<TObserver*> m_observers;
};
#endif //FIGURES_OBSERVERLIST_H