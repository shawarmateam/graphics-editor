cmake_minimum_required(VERSION 3.10)
project(graphics-editor)

# flags
set(CMAKE_CXX_STANDART 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# src
file(GLOB_RECURSE SRCS src/*.c)
file(GLOB_RECURSE HEADERS include/*)

# compile
add_executable(main ${SRCS})


# include packages

## SDL
find_package(SDL2 REQUIRED)
target_link_libraries(main SDL2)

## SDL_ttf
find_package(SDL2_ttf REQUIRED)
target_link_libraries(main SDL2_ttf)

# headers
include_directories(${CMAKE_SOURCE_DIR}/include)



file(COPY "${CMAKE_SOURCE_DIR}/assets" DESTINATION "${CMAKE_BINARY_DIR}")
