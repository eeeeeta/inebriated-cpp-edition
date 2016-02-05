cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project("inebriated C++ edition")

add_executable(markov rand.cpp
                markov.cpp
                save.cpp
		        main.cpp)
target_compile_features(markov PRIVATE cxx_range_for)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Werror -g")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
