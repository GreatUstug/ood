//
// Created by maxim on 06.09.2026.
//

#ifndef DUCKS_IQUAKBEHAVIOR_H
#define DUCKS_IQUAKBEHAVIOR_H

struct IQuackBehavior
{
    virtual ~IQuackBehavior(){};
    virtual void Quack() = 0;
};

#endif //DUCKS_IQUAKBEHAVIOR_H