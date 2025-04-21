cmake_minimum_required(VERSION 3.16)
project(genetic_qt VERSION 0.1 LANGUAGES CXX)


# --- Compiler options ---
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# --- Subdirectories ---
add_subdirectory(Common)
add_subdirectory(GeneticQt)
add_subdirectory(Models)
add_subdirectory(Views)
