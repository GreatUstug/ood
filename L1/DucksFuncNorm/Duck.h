#ifndef DUCK_H
#define DUCK_H

#include "FlyBehavior.h"
#include "QuackBehavior.h"

#include <cassert>
#include <iostream>
#include <memory>

#include "DanceBehavior.h"

#include <functional>

using Behavior = std::function<void()>;
using FlyBehavior = std::function<int()>;

class Duck
{
public:
    Duck(
    	FlyBehavior fly,
    	Behavior quack,
    	Behavior dance,
    	Behavior display
    	)
		: m_flyBehavior(fly)
		, m_quackBehavior(std::move(quack))
		, m_danceBehavior(std::move(dance))
		, m_displayBehavior(std::move(display))
    {
        assert(m_quackBehavior);
        assert(m_danceBehavior);
       SetFlyBehavior(fly);
    }

    void Quack() const
    {
        m_quackBehavior();
    }

    void Swim() const
    {
        std::cout << "I'm swimming" << std::endl;
    }

    void Fly()
    {
    	int flightCount = m_flyBehavior();
    	CheckAndQuack(flightCount);
    }

    void Dance() const
    {
        m_danceBehavior();
    }

    void SetFlyBehavior(FlyBehavior flyBehavior)
    {
        assert(flyBehavior);
        m_flyBehavior = std::move(flyBehavior);
    }

    void Display() const
    {
	    m_displayBehavior();
    };
    ~Duck() = default;

private:
    void CheckAndQuack(int flightCount) const
    {
        if (flightCount != 0 && flightCount % FLIGHT_DIVIDER == 0) {
            m_quackBehavior();
        }
    }
	Behavior m_displayBehavior;
    FlyBehavior m_flyBehavior;
    Behavior m_quackBehavior;
    Behavior m_danceBehavior;
    const int FLIGHT_DIVIDER = 2;
};

#endif