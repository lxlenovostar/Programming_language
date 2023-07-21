cmake_minimum_required(VERSION 3.14)
project(Proj7)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

add_executable(jackcompiler main.cc jackanalyzer.cc jackanalyzer.h jacktokenizer.cc jacktokenizer.h compilationengine.cc compilationengine.h utils.cc utils.h)