//
// Created by maxim on 07.09.2026.
//

#ifndef DUCKS_MOCKFLYBEHAVIOR_H
#define DUCKS_MOCKFLYBEHAVIOR_H

#include "Duck/Fly/IFlyBehavior.h"
#include <gmock/gmock.h>

class MockFlyBehavior : public IFlyBehavior
{
public:
	MOCK_METHOD(void, Fly, (), (override));
	MOCK_METHOD(bool, CanFly, (), (const, override));
};

#endif //DUCKS_MOCKFLYBEHAVIOR_H