cmake_minimum_required(VERSION 3.16)
project(Common LANGUAGES CXX)


# --- Packages ---
find_package(Qt5 REQUIRED COMPONENTS Core)


# --- Sources ---
set(HEADERS
    defines.h
    structs.h
)

set(SOURCES
)


# --- Target ---
add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})


# --- Include ---
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})


# --- Link ---
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core)
