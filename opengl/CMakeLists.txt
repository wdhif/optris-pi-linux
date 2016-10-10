CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

PROJECT(IRIMAGEROPENGLEXAMPLE)

SET(CMAKE_BUILD_TYPE Release)

INCLUDE_DIRECTORIES(/usr/include/libirimager)
LINK_DIRECTORIES(/usr/lib)

LINK_LIBRARIES(irimager irimageprocessing ircore udev GL GLU glut pthread)

ADD_EXECUTABLE(irimager_show Obvious2D.cpp irimagerShow.cpp)
ADD_EXECUTABLE(irimager_twin_show Obvious2D.cpp irimagerTwinShow.cpp)
