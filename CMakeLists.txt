cmake_minimum_required(VERSION 2.8)

# Project name
project(battleship)

# qiBuild CMake framework
find_package(qibuild)

# List of source files
set(_srcs
    battleship.cpp
    battleship.h
    board.cpp
    board.h
    enemyboard.cpp
    enemyboard.h
    global.cpp
    global.h
    main.cpp)

# plugin shared library
qi_create_lib(battleship SHARED ${_srcs} SUBFOLDER naoqi)

qi_use_lib(battleship ALCOMMON ALPROXIES BOOST)
