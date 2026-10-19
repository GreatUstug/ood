#ifndef FIGURES_SUBSCRIPTIONBYID_H
#define FIGURES_SUBSCRIPTIONBYID_H
#pragma once

#include <cstddef>
#include <utility>

template <typename TSubject>
class SubscriptionById {
public:
    using Id = std::size_t;

    SubscriptionById() = default;

    SubscriptionById(TSubject* subject, Id id)
        : m_subject(subject), m_id(id)
    {
        if (m_subject) m_subject->RegisterSubscription(this);
    }

    ~SubscriptionById() { Disconnect(); }

    SubscriptionById(const SubscriptionById&) = delete;
    SubscriptionById& operator=(const SubscriptionById&) = delete;

    SubscriptionById(SubscriptionById&& other) noexcept {
        MoveFrom(std::move(other));
    }

    SubscriptionById& operator=(SubscriptionById&& other) noexcept {
        if (this != &other) {
            Disconnect();
            MoveFrom(std::move(other));
        }
        return *this;
    }

    void Disconnect() {
        if (!m_active) return;
        if (m_alive && m_subject) {
            m_subject->UnsubscribeObserver(m_id);
            m_subject->UnregisterSubscription(this);
        }
        m_active = false;
        m_subject = nullptr;
    }

    bool IsActive() const { return m_active; }

    void MarkSubjectDead() { m_alive = false; }

private:
    void MoveFrom(SubscriptionById&& other) {
        m_subject = other.m_subject;
        m_id      = other.m_id;
        m_alive   = other.m_alive;
        m_active  = other.m_active;

        if (m_alive && m_subject && m_active) {
            m_subject->UnregisterSubscription(&other);
            m_subject->RegisterSubscription(this);
        }

        other.m_subject = nullptr;
        other.m_alive   = false;
        other.m_active  = false;
    }

    TSubject* m_subject = nullptr;
    Id        m_id      = 0;
    bool      m_alive   = true;
    bool      m_active  = true;
};

#endif //FIGURES_SUBSCRIPTIONBYID_H