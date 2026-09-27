#include <gtest/gtest.h>

#include "Duck.h"
#include "FlyBehavior.h"
#include "QuackBehavior.h"
#include "DanceBehavior.h"

class TestDuck : public Duck
{
public:
    using Duck::Duck;
};

struct DuckWithCounters
{
    Duck duck;
    int* flyCalls;
    int* quackCalls;
    int* danceCalls;
    int* displayCalls;
};


TEST(DuckDanceTest, DelegatesToStrategy)
{
    int danceCalls = 0;

    Duck duck(
        Fly::NoWay(),
        Quack::Normal(),
        [&danceCalls]() { ++danceCalls; },
        []() {}
    );

    duck.Dance();

    EXPECT_EQ(danceCalls, 1);
}

TEST(DuckDanceTest, DelegatesEveryCall)
{
    int danceCalls = 0;

    Duck duck(
        Fly::NoWay(),
        Quack::Mute(),
        [&danceCalls]() { ++danceCalls; },
        []() {}
    );

    duck.Dance();
    duck.Dance();
    duck.Dance();

    EXPECT_EQ(danceCalls, 3);
}



class DuckFlyQuackTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		m_flyCalls = 0;
		m_quackCalls = 0;
		m_danceCalls = 0;

		m_duck = std::make_unique<Duck>(
			[this]() -> int {
				++m_flyCalls;
				if (!m_canFly) return 0;
				return m_flyCalls;
			},
			[this]() { ++m_quackCalls; },
			[this]() { ++m_danceCalls; },
			[]() {}
		);
	};

	int m_flyCalls{};
	int m_quackCalls{};
	int m_danceCalls{};
	bool m_canFly = true;
	std::unique_ptr<Duck> m_duck;

};

TEST_F(DuckFlyQuackTest, QuacksAfterEverySecondFlight)
{
    m_flyCalls = 1;

    m_duck->Fly();
    m_duck->Fly();
    m_duck->Fly();
    m_duck->Fly();

    EXPECT_EQ(m_flyCalls, 5);
    EXPECT_EQ(m_quackCalls, 2);
}

TEST_F(DuckFlyQuackTest, QuacksOnlyAfterEvenFlights)
{
    m_flyCalls = 1;

    m_duck->Fly();
    m_duck->Fly();
    m_duck->Fly();

    EXPECT_EQ(m_flyCalls, 4);
    EXPECT_EQ(m_quackCalls, 2);
}

TEST_F(DuckFlyQuackTest, NonFlyingDuckNeverQuacks)
{
    m_flyCalls = 0;

    m_duck->Fly();
    m_duck->Fly();
    m_duck->Fly();

    EXPECT_EQ(m_flyCalls, 3);
    EXPECT_EQ(m_quackCalls, 1);
}