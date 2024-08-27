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

# libraries
include(FetchContent)

set(GLFW_VERSION 3.4)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY  https://github.com/glfw/glfw.git
    GIT_TAG         ${GLFW_VERSION}
)

FetchContent_MakeAvailable(glfw)
set(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} glfw)

# target files
file(GLOB_RECURSE PROJECT_SOURCES
    src/*.c
)
set(PROJECT_INCLUDE_DIRS
    src/
    src/platform/glad/include/
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

