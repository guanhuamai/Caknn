cmake_minimum_required(VERSION 3.6)
project(CaknnSR)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Utility.h Opf.h Result.h PartialMatrix.h SafeRegion.h DistElement.h BaseExpansion.h SLEExpansion.h Query.h IleMinExpansion.h LC.h MAD.h)

LINK_DIRECTORIES(/usr/local/lib)
add_executable(CaknnSR ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(CaknnSR libqdbm.a libxqdbm.a libqdbm.so)