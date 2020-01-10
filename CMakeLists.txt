cmake_minimum_required(VERSION 3.10)

project(Zcoreboard VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE SOURCE "src/*.cpp" "src/*.hpp")
add_executable(Zcoreboard "${SOURCE}")

target_link_libraries(Zcoreboard
		PUBLIC restbed
		PUBLIC jsoncpp)


