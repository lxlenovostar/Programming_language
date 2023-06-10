cmake_minimum_required(VERSION 3.14)
project(Proj6)

set(CMAKE_CXX_STANDARD 17)

add_executable(assembler main.cc code.h parser.cc parser.h symboltable.cc symboltable.h)