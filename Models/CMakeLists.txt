cmake_minimum_required(VERSION 3.16)
project(Models LANGUAGES CXX)


# --- Packages ---
find_package(Qt5 REQUIRED COMPONENTS Core)


# --- Sources ---
set(HEADERS
    car.h
    game.h
    gameimpl.h
    neural.h
    track.h
)

set(SOURCES
    car.cpp
    game.cpp
    gameimpl.cpp
    neural.cpp
    track.cpp
)


# --- Target ---
add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})


# --- Link ---
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core)
target_link_libraries(${PROJECT_NAME} PRIVATE Common)
