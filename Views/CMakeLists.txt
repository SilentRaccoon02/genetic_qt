cmake_minimum_required(VERSION 3.16)
project(Views LANGUAGES CXX)


# --- Packages ---
find_package(Qt5 REQUIRED COMPONENTS Widgets)


# --- Sources ---
set(HEADERS
    controls.h
    gamefield.h
    mainwindow.h
)

set(SOURCES
    controls.cpp
    gamefield.cpp
    mainwindow.cpp
)

set (UI
    controls.ui
)


# --- Target ---
add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES} ${UI})


# --- Link ---
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Common)
