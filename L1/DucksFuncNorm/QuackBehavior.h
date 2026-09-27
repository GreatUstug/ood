//
// Created by maxim on 11.09.2026.
//

#ifndef DUCKSFUNC_QUACK_H
#define DUCKSFUNC_QUACK_H
#include <functional>
#include <iostream>

namespace Quack
{
inline std::function<void()> Normal()
{
	return []() { std::cout << "Quack Quack!!!\n"; };
}

inline std::function<void()> Squeak()
{
	return []() { std::cout << "Squeek!!!\n"; };
}

inline std::function<void()> Mute()
{
	return []() {};
}
}

#endif //DUCKSFUNC_QUACK_H