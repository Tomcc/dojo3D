add_subdirectory("dojo")

set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "no desc" FORCE)
set(BUILD_BULLET3 OFF CACHE BOOL "no desc" FORCE)
set(BUILD_CPU_DEMOS OFF CACHE BOOL "no desc" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "no desc" FORCE)
set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "no desc" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "no desc" FORCE)
set(USE_GLUT OFF CACHE BOOL "no desc" FORCE)
set(USE_GRAPHICAL_BENCHMARK OFF CACHE BOOL "no desc" FORCE)
set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "no desc" FORCE)
add_subdirectory("bulletphysics")

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dojo")

project("Dojo3D")

option(IWYU "IWYU" OFF)

include(dojo/AddDojoIncludes.cmake)
INCLUDE(dojo/MSVCSetup.cmake)
include(dojo/ClangGCCSetup.cmake)

include_directories("include/Dojo3D")
include_directories("bulletphysics/src")

if(WIN32)
    include_directories("OculusSDK/LibOVR/Include")
    add_definitions("-DENABLE_OVR")
endif()

set(COMPILE_DEFINITIONS_RELEASE "${COMPILE_DEFINITIONS_RELEASE} -DPUBLISH")

file(GLOB common_src    
	"include/Dojo3D/*.h"
    "src/*.cpp"
)

set(dojo_SRC ${common_src} ${platform_src})

add_library(Dojo3D ${dojo_SRC})

target_link_libraries(Dojo3D Dojo BulletCollision BulletDynamics)

if (IWYU)
    find_program(iwyu_path NAMES include-what-you-use iwyu)
    if(NOT iwyu_path)
      message(FATAL_ERROR "Could not find the program include-what-you-use")
    endif()

    set_property(TARGET Dojo3D PROPERTY CXX_INCLUDE_WHAT_YOU_USE "${iwyu_path}")
else()
    include(cotire)
    
    set_target_properties(Dojo3D PROPERTIES
        COTIRE_PREFIX_HEADER_IGNORE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src;${CMAKE_CURRENT_SOURCE_DIR}/include;"
        COTIRE_ADD_UNITY_BUILD FALSE)

    cotire(Dojo3D)
endif()
