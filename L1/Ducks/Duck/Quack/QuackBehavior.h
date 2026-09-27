//
// Created by maxim on 06.09.2026.
//

#ifndef DUCKS_QUACKBEHAVIOR_H
#define DUCKS_QUACKBEHAVIOR_H

#include "IQuackBehavior.h"
#include <iostream>

class QuackBehavior : public IQuackBehavior
{
public:
    void Quack() override
    {
        std::cout << "Quack Quack!!!" << std::endl;
    }
};

#endif //DUCKS_QUACKBEHAVIOR_H