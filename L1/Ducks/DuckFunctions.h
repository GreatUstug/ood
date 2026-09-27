#ifndef DUCKS_DUCKFUNCTIONS_H
#define DUCKS_DUCKFUNCTIONS_H

#include "Duck/Duck.h"

void DrawDuck(Duck const& duck)
{
    duck.Display();
}

void PlayWithDuck(Duck& duck)
{
    DrawDuck(duck);
    duck.Quack();
    duck.Fly();
    duck.Dance();
    std::cout << std::endl;
}

#endif