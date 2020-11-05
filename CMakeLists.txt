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

target_compile_definitions(Bunny PUBLIC _DEBUG)

find_package(SDL2 REQUIRED)
find_package(ASSIMP REQUIRED)
find_package(GLEW REQUIRED)
find_package(DevIL REQUIRED)
find_package(OpenGL REQUIRED)

if(SDL2_FOUND)
  message("Found SDL2")
endif()

if(ASSIMP_FOUND)
  message("Found assimp")
endif()

if(GLEW_FOUND)
  message("Found GLEW")
endif()

if(DevIL_FOUND)
  message("Found DevIL")
endif()

if(OPENGL_FOUND)
  message("Found GL")
endif()

if(OPENGL_GLU_FOUND)
  message("Found GLU")
endif()

target_link_libraries(Bunny SDL2 GLEW assimp OpenGL::GL OpenGL::GLU ${IL_LIBRARIES} ${ILU_LIBRARIES})

#target_link_libraries(Bunny "-lSDL2" "-lGLEW" "-lassimp" "-lGLU" "-lGL" "-lIL" "-lILU")
