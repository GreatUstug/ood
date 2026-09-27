#ifndef MOCKDANCEBEHAVIOR_H
#define MOCKDANCEBEHAVIOR_H

#include "Duck/Dance/IDanceBehavior.h"
#include <gmock/gmock.h>

class MockDanceBehavior : public IDanceBehavior
{
public:
    MOCK_METHOD(void, Dance, (), (override));
};

#endif