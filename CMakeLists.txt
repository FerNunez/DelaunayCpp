cmake_minimum_required(VERSION 3.16)
project(Stars)

# Set the C++ standard to C++11
set(CMAKE_CXX_STANDARD 11)

## ERROR CHECK
# Add compiler flags for warnings
add_compile_options(-Wall -Wextra)
# Treat warnings as errors
add_compile_options(-Werror)
# Add additional error checking options
add_compile_options(-fsanitize=address -fsanitize=undefined -fsanitize=leak)

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
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} -fsanitize=address -fsanitize=undefined -fsanitize=leak)
