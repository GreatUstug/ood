#ifndef DUCKS_DUCKFUNCTIONS_H
#define DUCKS_DUCKFUNCTIONS_H

#include "Duck.h"

void PlayWithDuck(Duck& duck)
{
	duck.Display();
    duck.Quack();
    duck.Fly();
    duck.Dance();
    std::cout << std::endl;
}

#endif