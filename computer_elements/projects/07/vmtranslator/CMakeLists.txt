cmake_minimum_required(VERSION 3.14)
project(Proj7)

set(CMAKE_CXX_STANDARD 20)

add_executable(vmtranslator main.cc parser.cc parser.h codewriter.cc codewriter.h utils.h)