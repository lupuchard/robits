cmake_minimum_required(VERSION 3.6)
project(Town)

add_subdirectory(Deps/squirrel-3.1 Deps/squirrel-3.1)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -O0 -g -fsanitize=address -std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -O3 -std=c++17")

file(GLOB SOURCE_FILES *.cpp *.h Region/*cpp Region/*.h)
file(GLOB DEP_FILES Deps/*.cpp Deps/*.h)

add_executable(Town ${SOURCE_FILES} ${DEP_FILES} Game.cpp Game.h Deps/Rando.cpp Deps/Rando.h IRenderer.h ScriptManager.cpp ScriptManager.h Region/Substance.cpp Region/Substance.h)
target_link_libraries(Town sfml-graphics sfml-window sfml-system)
