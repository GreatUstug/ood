#ifndef DUCKS_IFLYBEHAVIOR_H
#define DUCKS_IFLYBEHAVIOR_H

struct IFlyBehavior
{
    virtual ~IFlyBehavior(){};
    virtual void Fly() = 0;
	virtual bool CanFly() const = 0;
};

#endif //DUCKS_IFLYBEHAVIOR_H