cmake_minimum_required(VERSION 3.1)
project(Projet_de_Fin_d_Etudes)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    Solver-2E-VRP/Solver-2E-VRP/Model/Client.cpp
    Solver-2E-VRP/Solver-2E-VRP/Model/Client.h
    Solver-2E-VRP/Solver-2E-VRP/Model/Depot.cpp
    Solver-2E-VRP/Solver-2E-VRP/Model/Depot.h
    Solver-2E-VRP/Solver-2E-VRP/Model/Node.cpp
    Solver-2E-VRP/Solver-2E-VRP/Model/Node.h
    Solver-2E-VRP/Solver-2E-VRP/Model/Problem.cpp
    Solver-2E-VRP/Solver-2E-VRP/Model/Problem.h
    Solver-2E-VRP/Solver-2E-VRP/Model/Satellite.cpp
    Solver-2E-VRP/Solver-2E-VRP/Model/Satellite.h
    Solver-2E-VRP/Solver-2E-VRP/Solver-2E-VRP.cpp
    Solver-2E-VRP/Solver-2E-VRP/stdafx.cpp
    Solver-2E-VRP/Solver-2E-VRP/stdafx.h
        Solver-2E-VRP/Solver-2E-VRP/targetver.h
        Solver-2E-VRP/Solver-2E-VRP/Model/Solution.cpp
        Solver-2E-VRP/Solver-2E-VRP/Model/Solution.h
        Solver-2E-VRP/Solver-2E-VRP/Config.cpp
        Solver-2E-VRP/Solver-2E-VRP/Config.h
        Solver-2E-VRP/Solver-2E-VRP/Config.cpp
        Solver-2E-VRP/Solver-2E-VRP/Config.h
        Solver-2E-VRP/Solver-2E-VRP/Utility.cpp
        Solver-2E-VRP/Solver-2E-VRP/Utility.h
        Solver-2E-VRP/Solver-2E-VRP/Solver/Heuristic.cpp
        Solver-2E-VRP/Solver-2E-VRP/Solver/Heuristic.h Solver-2E-VRP/Solver-2E-VRP/Solver/Insertion.cpp Solver-2E-VRP/Solver-2E-VRP/Solver/Insertion.h Solver-2E-VRP/Solver-2E-VRP/Solver/MoleJamesonHeuristic.cpp Solver-2E-VRP/Solver-2E-VRP/Solver/MoleJamesonHeuristic.h Solver-2E-VRP/Solver-2E-VRP/Solver/LSSolver.cpp Solver-2E-VRP/Solver-2E-VRP/Solver/LSSolver.h Solver-2E-VRP/Solver-2E-VRP/Solver/SDVRPSolver.cpp Solver-2E-VRP/Solver-2E-VRP/Solver/SDVRPSolver.h Solver-2E-VRP/Solver-2E-VRP/Solver/IDCH.cpp Solver-2E-VRP/Solver-2E-VRP/Solver/IDCH.h)

add_executable(Projet_de_Fin_d_Etudes ${SOURCE_FILES})