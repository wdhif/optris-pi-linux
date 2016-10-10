CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

PROJECT(IRIMAGEROPENGLEXAMPLE)

ADD_SUBDIRECTORY(lib/cpr)

SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

INCLUDE_DIRECTORIES(lib/libirimager ${CPR_INCLUDE_DIRS})
LINK_DIRECTORIES(/usr/lib)

LINK_LIBRARIES(irimager irimageprocessing ircore udev GL GLU glut pthread ${CPR_LIBRARIES})

ADD_EXECUTABLE(irimager_show Obvious2D.cpp irimagerShow.cpp)

