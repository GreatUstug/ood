//
// Created by maxim on 11.09.2026.
//

#ifndef DUCKSFUNC_DUCKCONSTRUCTBEHAVIOR_H
#define DUCKSFUNC_DUCKCONSTRUCTBEHAVIOR_H
#include "DanceBehavior.h"
#include "FlyBehavior.h"
#include "QuackBehavior.h"
#include "Duck.h"

namespace DuckConstruct
{
inline Duck Mallard()
{
	return Duck(Fly::WithWings(),
		 Quack::Normal(),
		 Dance::Waltz(),
		 [](){std::cout << "I'm mallard duck" << std::endl;}
		);
}
inline Duck Model()
{
	return Duck(Fly::NoWay(),
		Quack::Normal(),
		Dance::NoWay(),
		[](){std::cout << "I'm model duck" << std::endl;}
		);
}
inline Duck Decoy()
{
	return Duck(Fly::NoWay(), Quack::Mute(), Dance::NoWay(),
		[](){std::cout << "I'm decoy duck" << std::endl;});
}
inline Duck Redhead()
{
	return Duck(Fly::WithWings(), Quack::Normal(), Dance::Minuet(),
		[](){std::cout << "I'm redhead duck" << std::endl;}
		);
}
inline Duck Rubber()
{
	return Duck(Fly::NoWay(), Quack::Squeak(), ::Dance::Minuet(),
		[](){std::cout << "I'm rubber duck" << std::endl;}
		);
}
}

#endif //DUCKSFUNC_DUCKCONSTRUCTBEHAVIOR_H