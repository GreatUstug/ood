cmake_minimum_required(VERSION 4.1)
project(Ducks)

set(CMAKE_CXX_STANDARD 23)

add_executable(Ducks main.cpp
        DuckFunctions.h
        Duck/DecoyDuck.h
        Duck/Duck.h
        Duck/MallarDuck.h
        Duck/ModelDuck.h
        Duck/RedheadDuck.h
        Duck/RubberDuck.h
        Duck/Fly/FlyNoWay.h
        Duck/Fly/FlyWithWings.h
        Duck/Fly/IFlyBehavior.h
        Duck/Quack/IQuackBehavior.h
        Duck/Quack/MuteQuackBehavior.h
        Duck/Quack/QuackBehavior.h
        Duck/Quack/SqueakBehavior.h
        Duck/Dance/IDanceBehavior.h
        Duck/Dance/DanceWaltz.h
        Duck/Dance/DanceMinuet.h
        Duck/Dance/DanceNoWay.h
        Mocks/MockFlyBehavior.h
)

