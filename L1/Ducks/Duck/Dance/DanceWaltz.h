//
// Created by maxim on 06.09.2026.
//

#ifndef DUCKS_DANCEWALTZ_H
#define DUCKS_DANCEWALTZ_H
#include <iostream>

#include "IDanceBehavior.h"

class DanceWaltz : public IDanceBehavior {
    public:
        void Dance() override
        {
            std::cout << "I'm dancing waltz!" << '\n';
        }
};

#endif //DUCKS_DANCEWALTZ_H