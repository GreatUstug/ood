#ifndef DUCKS_FLYNOWAY_H
#define DUCKS_FLYNOWAY_H

#include "IFlyBehavior.h"

class FlyNoWay : public IFlyBehavior
{
public:
    void Fly() override {}
	bool CanFly() const override
    {
	    return false;
    }
};

#endif //DUCKS_FLYNOWAY_H