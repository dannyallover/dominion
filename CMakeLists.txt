cmake_minimum_required(VERSION 2.6)

project(dominion)

set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "-Wall -Wno-trigraphs -Wpedantic -Wextra -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


include_directories(src
    cpp
    )

add_subdirectory(src
    cpp
    )

add_executable(dominion
    src/cpp/main.cpp
    #src/cpp/mainTest.cpp
    src/cpp/Card.cpp
    src/cpp/ActionCard.cpp
    src/cpp/GameState.cpp
    src/cpp/Pile.cpp
    src/cpp/Player.cpp
    src/cpp/RandUtils.cpp
    src/cpp/TreasureCard.cpp
    src/cpp/VictoryCard.cpp
    src/cpp/CardLookup.cpp
    )

target_link_libraries(dominion
    )

#install (TARGETS dominion DESTINATION bin)
