#ifndef DUCKS_MUTEQUACKBEHAVIOR_H
#define DUCKS_MUTEQUACKBEHAVIOR_H

#include "IQuackBehavior.h"

class MuteQuackBehavior : public IQuackBehavior
{
public:
    void Quack() override {}
};

#endif //DUCKS_MUTEQUACKBEHAVIOR_H