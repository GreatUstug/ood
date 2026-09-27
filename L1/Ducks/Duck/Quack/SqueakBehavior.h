//
// Created by maxim on 06.09.2026.
//

#ifndef DUCKS_SQUEAKBEHAVIOR_H
#define DUCKS_SQUEAKBEHAVIOR_H

#include "IQuackBehavior.h"
#include <iostream>

class SqueakBehavior : public IQuackBehavior
{
public:
    void Quack() override
    {
        std::cout << "Squeek!!!" << std::endl;
    }
};

#endif //DUCKS_SQUEAKBEHAVIOR_H