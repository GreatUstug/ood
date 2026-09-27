#ifndef MOCKQUACKBEHAVIOR_H
#define MOCKQUACKBEHAVIOR_H

#include "Duck/Quack/IQuackBehavior.h"
#include <gmock/gmock.h>

class MockQuackBehavior : public IQuackBehavior
{
public:
	MOCK_METHOD(void, Quack, (), (override));
};

#endif