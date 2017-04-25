cmake_minimum_required(VERSION 3.7)
project(Octee)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp
        include/octree.h
        include/point.h
        src/diffusion.cpp
        include/diffusion.h
        include/octree_iterator.h)

set(SFML_FLAGS "-lsfml-graphics -lsfml-window -lsfml-system")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SFML_FLAGS}")


add_executable(Octee ${SOURCE_FILES})

target_link_libraries(Octee ${SFML_FLAGS})