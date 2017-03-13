cmake_minimum_required(VERSION 3.6)
project(BlackJack)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        BlackJack.cpp
        BlackJack.h
        cards.h
        players.h blackjack_server.cpp)

add_executable(BlackJack ${SOURCE_FILES})