cmake_minimum_required(VERSION 3.8)
project(rolling_sky)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp util.h Ball.cpp Ball.h const.h Box.cpp Box.h myImage.cpp myImage.h ObjReader.cpp ObjReader.h
        util.cpp SkyBox.cpp SkyBox.h firework.cpp firework.h particle.cpp particle.h snow.cpp snow.h)
#add_subdirectory("Maths")
file(GLOB MahtsLib Maths/*.cpp)
file(GLOB GLeeLib GLee/*.c)
add_executable(rolling_sky ${SOURCE_FILES} ${MahtsLib} ${GLeeLib})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} -lm ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})