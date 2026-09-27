//
// Created by maxim on 06.09.2026.
//

#ifndef DUCKS_IDANCEBEHAVIOR_H
#define DUCKS_IDANCEBEHAVIOR_H

struct IDanceBehavior
{
	// TODO: Про виртуальный деструктор вспомнить
    virtual ~IDanceBehavior() = default;
    virtual void Dance() = 0;
};

#endif //DUCKS_IDANCEBEHAVIOR_H