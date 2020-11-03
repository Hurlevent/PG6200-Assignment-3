cmake_minimum_required(VERSION 3.7)

project(Bunny VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(SOURCE_FILES
  main.cpp
  GameManager.cpp
  Model.cpp
  ShadowFBO.cpp
  VirtualTrackball.cpp
  )

add_executable(Bunny ${SOURCE_FILES})

target_compile_features(Bunny PUBLIC cxx_std_17)


# TODO: Use find_package() for cross platform support

target_link_libraries(Bunny "-lSDL2" "-lGLEW" "-lassimp" "-lGLU" "-lGL" "-lIL" "-lILU")
