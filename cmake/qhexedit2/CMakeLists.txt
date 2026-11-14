cmake_minimum_required(VERSION 3.16)

# Small wrapper used by ExternalProject to build qhexedit2, which only ships a
# qmake project upstream.
project(qhexedit2-build LANGUAGES CXX)

set(Qt "Qt5" CACHE STRING "Qt package to use (Qt5 or Qt6)")
if(NOT Qt MATCHES "^Qt[56]$")
    message(FATAL_ERROR "Qt must be Qt5 or Qt6 (got '${Qt}')")
endif()

if(Qt STREQUAL "Qt6")
    set(CMAKE_CXX_STANDARD 17)
else()
    set(CMAKE_CXX_STANDARD 11)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)

find_package(${Qt} REQUIRED COMPONENTS Core Gui Widgets)

if(NOT QHEXEDIT2_SOURCE_DIR)
    message(FATAL_ERROR "QHEXEDIT2_SOURCE_DIR must point at a qhexedit2 checkout")
endif()

set(QHEXEDIT2_SRC "${QHEXEDIT2_SOURCE_DIR}/src")
if(NOT EXISTS "${QHEXEDIT2_SRC}/qhexedit.cpp")
    message(FATAL_ERROR "qhexedit2 sources not found in ${QHEXEDIT2_SRC}")
endif()

add_library(qhexedit STATIC
    "${QHEXEDIT2_SRC}/chunks.cpp"
    "${QHEXEDIT2_SRC}/commands.cpp"
    "${QHEXEDIT2_SRC}/qhexedit.cpp"
    "${QHEXEDIT2_SRC}/chunks.h"
    "${QHEXEDIT2_SRC}/commands.h"
    "${QHEXEDIT2_SRC}/qhexedit.h"
)

target_include_directories(qhexedit PUBLIC "${QHEXEDIT2_SRC}")
target_link_libraries(qhexedit PUBLIC ${Qt}::Core ${Qt}::Gui ${Qt}::Widgets)

install(TARGETS qhexedit
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
