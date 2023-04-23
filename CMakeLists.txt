cmake_minimum_required(VERSION 3.16)
project(Stars)

# Set the C++ standard to C++11
set(CMAKE_CXX_STANDARD 11)

# Add the source files
set(SOURCES
  main.cpp
)

# Add the include files
set(INCLUDES
  include/delaunay.h
  include/delaunay.cpp
  include/viewer.h

)

# Add the executable
add_executable(Stars ${SOURCES} ${INCLUDES})

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})
