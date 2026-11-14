cmake_minimum_required(VERSION 3.16)

project(GLTF-Tool VERSION 0.1.0 LANGUAGES CXX)

# Qt package: Qt5 (default) or Qt6.
set(Qt "Qt5" CACHE STRING "Qt package to use (Qt5 or Qt6)")
set_property(CACHE Qt PROPERTY STRINGS Qt5 Qt6)
if(NOT Qt MATCHES "^Qt[56]$")
    message(FATAL_ERROR "Qt must be Qt5 or Qt6 (got '${Qt}')")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(Qt STREQUAL "Qt6")
    set(CMAKE_CXX_STANDARD 17)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(${Qt} REQUIRED COMPONENTS Core Gui Widgets)

message(STATUS "Building GLTF-Tool with ${Qt}")

include(ExternalProject)
find_package(Git REQUIRED)

set(GLTF_TOOL_DEPS_DIR "${CMAKE_BINARY_DIR}/_deps")
file(MAKE_DIRECTORY "${GLTF_TOOL_DEPS_DIR}")

# Pinned dependency revisions (previously tracked as Git submodules).
set(TINYGLTF_REPOSITORY "https://github.com/syoyo/tinygltf.git")
set(TINYGLTF_TAG "eaa25a307af92722c987fd37b2b09947732f6341")
set(QHEXEDIT2_REPOSITORY "https://github.com/fferri/qhexedit2.git")
set(QHEXEDIT2_TAG "acc907ddbbba1ef0ed175481942c4342d9bb2e0f")

set(TINYGLTF_SOURCE_DIR "${GLTF_TOOL_DEPS_DIR}/tinygltf")
set(QHEXEDIT2_SOURCE_DIR "${GLTF_TOOL_DEPS_DIR}/qhexedit2")
set(QHEXEDIT2_BINARY_DIR "${GLTF_TOOL_DEPS_DIR}/qhexedit2-build")
set(QHEXEDIT2_INSTALL_DIR "${GLTF_TOOL_DEPS_DIR}/qhexedit2-install")
set(QHEXEDIT2_LIBRARY
    "${QHEXEDIT2_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}qhexedit${CMAKE_STATIC_LIBRARY_SUFFIX}")

# tinygltf is header-only. Fetch the pinned commit; tinygltf.cpp in this
# project provides the implementation. The commit is no longer reachable from
# any branch in the upstream repository, so fetch it by SHA (GitHub allows
# this) through the download script below.
ExternalProject_Add(tinygltf_dep
    GIT_REPOSITORY "${TINYGLTF_REPOSITORY}"
    GIT_TAG "${TINYGLTF_TAG}"
    GIT_SHALLOW FALSE
    SOURCE_DIR "${TINYGLTF_SOURCE_DIR}"
    DOWNLOAD_COMMAND
        "${CMAKE_COMMAND}"
        "-DGIT_EXECUTABLE=${GIT_EXECUTABLE}"
        "-DSOURCE_DIR=<SOURCE_DIR>"
        "-DREPOSITORY=${TINYGLTF_REPOSITORY}"
        "-DTAG=${TINYGLTF_TAG}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchGitCommit.cmake"
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    TEST_COMMAND ""
)

# qhexedit2 only ships a qmake project, so build its sources through a small
# CMake wrapper (cmake/qhexedit2/CMakeLists.txt) fetched by ExternalProject.
set(Qt_DIR_VAR "${Qt}_DIR")
set(Qt_PACKAGE_DIR "${${Qt_DIR_VAR}}")
set(QHEXEDIT2_CONFIGURE_COMMAND
    "${CMAKE_COMMAND}"
    -G "${CMAKE_GENERATOR}"
    -S "${CMAKE_CURRENT_SOURCE_DIR}/cmake/qhexedit2"
    -B "<BINARY_DIR>"
    "-DQt=${Qt}"
    "-DQHEXEDIT2_SOURCE_DIR=${QHEXEDIT2_SOURCE_DIR}"
    "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-D${Qt_DIR_VAR}=${Qt_PACKAGE_DIR}"
)
if(CMAKE_CXX_COMPILER)
    list(APPEND QHEXEDIT2_CONFIGURE_COMMAND "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
endif()
if(CMAKE_TOOLCHAIN_FILE)
    list(APPEND QHEXEDIT2_CONFIGURE_COMMAND "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
endif()
if(CMAKE_GENERATOR_PLATFORM)
    list(APPEND QHEXEDIT2_CONFIGURE_COMMAND -A "${CMAKE_GENERATOR_PLATFORM}")
endif()
if(CMAKE_GENERATOR_TOOLSET)
    list(APPEND QHEXEDIT2_CONFIGURE_COMMAND -T "${CMAKE_GENERATOR_TOOLSET}")
endif()
if(CMAKE_OSX_ARCHITECTURES)
    list(APPEND QHEXEDIT2_CONFIGURE_COMMAND "-DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}")
endif()
if(CMAKE_OSX_DEPLOYMENT_TARGET)
    list(APPEND QHEXEDIT2_CONFIGURE_COMMAND "-DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}")
endif()

ExternalProject_Add(qhexedit2_dep
    GIT_REPOSITORY "${QHEXEDIT2_REPOSITORY}"
    GIT_TAG "${QHEXEDIT2_TAG}"
    GIT_SHALLOW FALSE
    SOURCE_DIR "${QHEXEDIT2_SOURCE_DIR}"
    BINARY_DIR "${QHEXEDIT2_BINARY_DIR}"
    INSTALL_DIR "${QHEXEDIT2_INSTALL_DIR}"
    BUILD_BYPRODUCTS "${QHEXEDIT2_LIBRARY}"
    CONFIGURE_COMMAND ${QHEXEDIT2_CONFIGURE_COMMAND}
    BUILD_COMMAND "${CMAKE_COMMAND}" --build "<BINARY_DIR>" --config "<CONFIG>"
    INSTALL_COMMAND "${CMAKE_COMMAND}" --install "<BINARY_DIR>" --config "<CONFIG>"
    UPDATE_COMMAND ""
    TEST_COMMAND ""
)

set(APP_SOURCES
    abstracttreemodel.cpp
    gltfinfo.cpp
    gltfmodel.cpp
    main.cpp
    mainwindow.cpp
    page.cpp
    pageaccessor.cpp
    pageanimation.cpp
    pagebuffer.cpp
    pagebufferview.cpp
    pagecamera.cpp
    pageimage.cpp
    pagelight.cpp
    pagematerial.cpp
    pagemesh.cpp
    pagenode.cpp
    pagesampler.cpp
    pagescene.cpp
    pageskin.cpp
    pagetexture.cpp
    tinygltf.cpp
)

set(APP_HEADERS
    abstracttreemodel.h
    gltfinfo.h
    gltfmodel.h
    mainwindow.h
    page.h
    pageaccessor.h
    pageanimation.h
    pagebuffer.h
    pagebufferview.h
    pagecamera.h
    pageimage.h
    pagelight.h
    pagematerial.h
    pagemesh.h
    pagenode.h
    pagesampler.h
    pagescene.h
    pageskin.h
    pagetexture.h
)

set(APP_FORMS
    mainwindow.ui
    pageaccessor.ui
    pageanimation.ui
    pagebuffer.ui
    pagebufferview.ui
    pagecamera.ui
    pageimage.ui
    pagelight.ui
    pagematerial.ui
    pagemesh.ui
    pagenode.ui
    pagesampler.ui
    pagescene.ui
    pageskin.ui
    pagetexture.ui
)

add_executable(GLTF-Tool MACOSX_BUNDLE
    ${APP_SOURCES}
    ${APP_HEADERS}
    ${APP_FORMS}
)

target_include_directories(GLTF-Tool PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${TINYGLTF_SOURCE_DIR}"
    "${QHEXEDIT2_SOURCE_DIR}/src"
)

target_link_libraries(GLTF-Tool PRIVATE
    ${Qt}::Core
    ${Qt}::Gui
    ${Qt}::Widgets
    "${QHEXEDIT2_LIBRARY}"
)

target_compile_definitions(GLTF-Tool PRIVATE QT_DEPRECATED_WARNINGS)

set_target_properties(GLTF-Tool PROPERTIES
    MACOSX_BUNDLE_BUNDLE_NAME "GLTF-Tool"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.fferri.GLTF-Tool"
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
)

# Both dependencies must be fetched before compiling sources that include them.
add_dependencies(GLTF-Tool tinygltf_dep qhexedit2_dep)

install(TARGETS GLTF-Tool
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
