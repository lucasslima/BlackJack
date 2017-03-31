cmake_minimum_required(VERSION 3.6)
project(BlackJack)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-static-libgcc -static-libstdc++")        ## Optimize

set(SOURCE_FILES
        BlackJack.cpp
        BlackJack.h
        cards.h
        players.h )

set(SERVER_FILES
        blackjack_server.cpp
        )
set(CLIENT_FILES
        blackajck_client.cpp
        )

add_executable(BlackJack ${SOURCE_FILES})
add_executable(server ${SERVER_FILES})
TARGET_LINK_LIBRARIES(server -lboost_system)
add_executable(client ${CLIENT_FILES})
TARGET_LINK_LIBRARIES(client -lboost_system -lpthread)
