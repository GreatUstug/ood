#ifndef DUCKS_FLYWITHWINGS_H
#define DUCKS_FLYWITHWINGS_H

#include "IFlyBehavior.h"
#include <iostream>

class FlyWithWings : public IFlyBehavior
{
public:
    void Fly() override
    {
        std::cout << "I'm flying with wings!!" << std::endl;
    }
	bool CanFly() const override
    {
	    return false;
    }
};

#endif //DUCKS_FLYWITHWINGS_H