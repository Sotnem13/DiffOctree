cmake_minimum_required(VERSION 3.7)
project(Octee)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp
        octree.h
        point.h
        diffusion.cpp
        diffusion.h
        )

set(SFML_FLAGS "-lsfml-graphics -lsfml-window -lsfml-system")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SFML_FLAGS}")


add_executable(Octee ${SOURCE_FILES})

target_link_libraries(Octee ${SFML_FLAGS})