# 3.11+ for FetchContent, 3.21+ for C17
cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

# generate compilation db
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set     (PROJECT_NAME poNES)
set     (DEVICE_LIB_NAME ${PROJECT_NAME}_devlib)

project (${PROJECT_NAME} VERSION 0.1 LANGUAGES C)
set     (CMAKE_BUILD_TYPE Debug)
set     (CMAKE_C_STANDARD 17)

add_compile_options(-Wall -Wextra -pedantic)

# get raylib
set(RAYLIB_VERSION 4.2.0)
find_package(raylib ${RAYLIB_VERSION} QUIET)
if (NOT raylib_FOUND) # download raylib if not present
  include(FetchContent)
  FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # is raylib downloaded yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()

set(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} raylib)
if (APPLE)
    set(PROJECT_LIBRARIES ${PROJECT_LIBRARIES}
        "-framework IOKit"
        "-framework Cocoa"
        "-framework OpenGL")
endif()

# target files
file(GLOB_RECURSE PROJECT_SOURCES
    src/*.c
    src/*.h
)
set(PROJECT_INCLUDE_DIRS
    src/
    src/cart/
    src/mapper/
)

# platform
add_executable              (${PROJECT_NAME} ${PROJECT_SOURCES})

target_include_directories  (${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDE_DIRS})
target_link_libraries       (${PROJECT_NAME} PUBLIC ${PROJECT_LIBRARIES})
target_compile_definitions  (${PROJECT_NAME} PUBLIC ${PROJECT_COMPILE_DEFINITIONS})
target_compile_options      (${PROJECT_NAME} PUBLIC ${PROJECT_COMPILE_OPTIONS})
set_target_properties       (${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE C)

if (APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        XCODE_GENERATE_SCHEME TRUE
        XCODE_SCHEME_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif()

add_custom_target(run COMMAND ${PROJECT_NAME})

