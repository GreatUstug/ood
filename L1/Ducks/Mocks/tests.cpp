#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../Duck/Duck.h"
#include "../Mocks/MockFlyBehavior.h"
#include "../Mocks/MockQuackBehavior.h"
#include "../Mocks/MockDanceBehavior.h"

using ::testing::StrictMock;
using ::testing::Return;

namespace {

class TestDuck : public Duck
{
public:
    using Duck::Duck;
    void Display() const override {}
};

}

TEST(DuckDanceTest, DelegatesToStrategy)
{
    auto dance = std::make_unique<StrictMock<MockDanceBehavior>>();
    auto* dancePtr = dance.get();

    TestDuck duck(
        std::make_unique<StrictMock<MockFlyBehavior>>(),
        std::make_unique<StrictMock<MockQuackBehavior>>(),
        std::move(dance));

    EXPECT_CALL(*dancePtr, Dance()).Times(1);
    duck.Dance();
}

TEST(DuckDanceTest, DelegatesEveryCall)
{
    auto dance = std::make_unique<StrictMock<MockDanceBehavior>>();
    auto* dancePtr = dance.get();

    TestDuck duck(
        std::make_unique<StrictMock<MockFlyBehavior>>(),
        std::make_unique<StrictMock<MockQuackBehavior>>(),
        std::move(dance));

    EXPECT_CALL(*dancePtr, Dance()).Times(3);
    duck.Dance();
    duck.Dance();
    duck.Dance();
}

class DuckFlyQuackTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        auto fly   = std::make_unique<StrictMock<MockFlyBehavior>>();
        auto quack = std::make_unique<StrictMock<MockQuackBehavior>>();
        auto dance = std::make_unique<StrictMock<MockDanceBehavior>>();

        m_fly   = fly.get();
        m_quack = quack.get();

        m_duck = std::make_unique<TestDuck>(
            std::move(fly), std::move(quack), std::move(dance));
    }

    StrictMock<MockFlyBehavior>*   m_fly{};
    StrictMock<MockQuackBehavior>* m_quack{};
    std::unique_ptr<TestDuck>      m_duck;
};

TEST_F(DuckFlyQuackTest, QuacksAfterEverySecondFlight)
{
    EXPECT_CALL(*m_fly, CanFly()).WillRepeatedly(Return(true));
    EXPECT_CALL(*m_fly, Fly()).Times(4);
    EXPECT_CALL(*m_quack, Quack()).Times(2);

    m_duck->Fly();
    m_duck->Fly();
    m_duck->Fly();
    m_duck->Fly();
}

TEST_F(DuckFlyQuackTest, QuacksOnlyAfterEvenFlights)
{
    EXPECT_CALL(*m_fly, CanFly()).WillRepeatedly(Return(true));
    EXPECT_CALL(*m_fly, Fly()).Times(3);
    EXPECT_CALL(*m_quack, Quack()).Times(1);

    m_duck->Fly();
    m_duck->Fly();
    m_duck->Fly();
}

TEST_F(DuckFlyQuackTest, NonFlyingDuckNeverQuacks)
{
	EXPECT_CALL(*m_fly, CanFly()).WillRepeatedly(Return(false));
	EXPECT_CALL(*m_fly, Fly()).Times(3);
	EXPECT_CALL(*m_quack, Quack()).Times(0);

	m_duck->Fly();
	m_duck->Fly();
	m_duck->Fly();
}