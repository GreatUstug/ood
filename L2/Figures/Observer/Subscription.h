#ifndef FIGURES_SUBSCRIPTION_H
#define FIGURES_SUBSCRIPTION_H
#pragma once

template <typename TSubject, typename TObserver> class Subscription
{
public:
	Subscription() = default;

	Subscription(TSubject* subject, TObserver* observer)
		: m_subject(subject)
		  , m_observer(observer)
	{
		if (m_subject)
		{
			m_subject->RegisterSubscription(this);
		}
	}

	~Subscription()
	{
		Disconnect();
	}

	Subscription(const Subscription&) = delete;

	Subscription& operator=(const Subscription&) = delete;

	Subscription(Subscription&& other)
	{
		m_subject = other.m_subject;
		m_observer = other.m_observer;
		m_alive = other.m_alive;
		m_active = other.m_active;

		if (m_alive && m_subject && m_active)
		{
			m_subject->UnregisterSubscription(&other);
			m_subject->RegisterSubscription(this);
		}

		other.m_subject = nullptr;
		other.m_observer = nullptr;
		other.m_alive = false;
		other.m_active = false;
	};

	void Disconnect()
	{
		if (!m_active)
			return;
		if (m_alive && m_subject)
		{
			m_subject->UnsubscribeObserver(m_observer);
			m_subject->UnregisterSubscription(this);
		}
		m_active = false;
		m_subject = nullptr;
		m_observer = nullptr;
	}

	bool IsActive() const { return m_active; }
	TObserver* GetObserver() const { return m_observer; }

	void MarkSubjectDead()
	{
		m_alive = false;
	}

private:
	TSubject* m_subject = nullptr;
	TObserver* m_observer = nullptr;
	bool m_alive = true;
	bool m_active = true;
};

#endif //FIGURES_SUBSCRIPTION_H