cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project("inebriated C++ edition")

add_executable(markov markov.cpp)
target_compile_features(markov PRIVATE cxx_range_for)
