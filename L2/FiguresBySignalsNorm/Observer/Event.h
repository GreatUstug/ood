#ifndef FIGURES_EVENT_H
#define FIGURES_EVENT_H
#pragma once

#include "ObserverListById.h"
#include "SubscriptionById.h"

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

template <typename Signature>
class Event;

template <typename... Args>
class Event<void(Args...)> {
public:
	using Handler = std::function<void(Args...)>;

	std::unique_ptr<SubscriptionById<Event>> Subscribe(Handler handler) {
		const auto id = m_handlers.AddObserver(std::move(handler));
		return std::make_unique<SubscriptionById<Event>>(this, id);
	}

	void UnsubscribeObserver(std::size_t id) {
		m_handlers.RemoveObserver(id);
	}

	void RegisterSubscription(SubscriptionById<Event>* sub) {
		m_subscriptions.push_back(sub);
	}

	void UnregisterSubscription(SubscriptionById<Event>* sub) {
		m_subscriptions.erase(
			std::remove(m_subscriptions.begin(), m_subscriptions.end(), sub),
			m_subscriptions.end());
	}

	void Notify(Args... args) {
		m_handlers.Notify([&](Handler& h) {
			h(args...);
		});
	}

	~Event() {
		for (auto* sub : m_subscriptions) {
			sub->MarkSubjectDead();
		}
	}

private:
	ObserverListById<Handler>                       m_handlers;
	std::vector<SubscriptionById<Event>*>           m_subscriptions;
};

#endif //FIGURES_EVENT_H