//
// Created by maxim on 06.09.2026.
//

#ifndef DUCKS_DANCEMINUET_H
#define DUCKS_DANCEMINUET_H

class DanceMinuet : public IDanceBehavior {
public:
    void Dance() override
    {
        std::cout << "I'm dancing minuet!" << '\n';
    }
};

#endif //DUCKS_DANCEMINUET_H