#ifndef DUCK_H
#define DUCK_H

#include "Fly/IFlyBehavior.h"
#include "Quack/IQuackBehavior.h"

#include <cassert>
#include <iostream>
#include <memory>

#include "Dance/IDanceBehavior.h"

class Duck
{
public:
    Duck(std::unique_ptr<IFlyBehavior>&& flyBehavior,
        std::unique_ptr<IQuackBehavior>&& quackBehavior,
        std::unique_ptr<IDanceBehavior>&& danceBehavior)
        : m_quackBehavior(std::move(quackBehavior)),
          m_danceBehavior(std::move(danceBehavior))
    {
        assert(m_quackBehavior);
        assert(m_danceBehavior);
    	// TODO: почему ассерты и полчему сеттер именно в таком виде
        SetFlyBehavior(std::move(flyBehavior));
    }

    void Quack() const
    {
        m_quackBehavior->Quack();
    }

    void Swim()
    {
        std::cout << "I'm swimming" << std::endl;
    }

    void Fly()
    {
    	if (m_flyBehavior->CanFly() == true) {
    		m_flightCount++;
    	}
        m_flyBehavior->Fly();
    	PrintFlyCount();
    	CheckAndQuack();
    }

    void Dance()
    {
        m_danceBehavior->Dance();
    }

    void SetFlyBehavior(std::unique_ptr<IFlyBehavior>&& flyBehavior)
    {
        assert(flyBehavior);
        m_flyBehavior = std::move(flyBehavior);
    }

    virtual void Display() const = 0;
    virtual ~Duck() = default;

private:
	void PrintFlyCount() const
	{
		std::cout << "Fly count: " << m_flightCount << std::endl;
	}
    void CheckAndQuack() const
    {
        if ( m_flightCount % FLIGHT_DIVIDER == 0) {
            m_quackBehavior->Quack();
        }
    }
    std::unique_ptr<IFlyBehavior> m_flyBehavior;
    std::unique_ptr<IQuackBehavior> m_quackBehavior;
    std::unique_ptr<IDanceBehavior> m_danceBehavior;
    static constexpr int FLIGHT_DIVIDER = 2;
	int m_flightCount = 0;
};

#endif