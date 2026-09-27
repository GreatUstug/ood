//
// Created by maxim on 11.09.2026.
//

#ifndef DUCKSFUNC_DANCE_H
#define DUCKSFUNC_DANCE_H
#include <functional>
#include <iostream>

namespace Dance
{
inline std::function<void()> Waltz()
{
	return []() { std::cout << "I'm dancing waltz!\n"; };
}

inline std::function<void()> Minuet()
{
	return []() { std::cout << "I'm dancing minuet!\n"; };
}

inline std::function<void()> NoWay()
{
	return []() {};
}
}

#endif //DUCKSFUNC_DANCE_H