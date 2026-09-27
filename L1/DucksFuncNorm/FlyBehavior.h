//
// Created by maxim on 11.09.2026.
//

#ifndef DUCKSFUNC_FLY_H
#define DUCKSFUNC_FLY_H
#include <functional>
#include <iostream>

namespace Fly
{
inline std::function<int()> WithWings()
{
	//std::function - при копировании сохранится ли у нас переменная захвата с таким же значением
	return [count = 0]() mutable -> int{
		++count;
		std::cout << "The flight number is апрар " << count << '\n';
		std::cout << "I'm flying with wings!!\n";
		return count;
	};
}

inline std::function<int()> NoWay()
{
	return []() -> int 
	{
		return 0;
	};
}
}

#endif //DUCKSFUNC_FLY_H