# Set minimum CMake version
cmake_minimum_required(VERSION 3.14)

# Set project name and the programming language
project("prototype-server" LANGUAGES CXX)

# Set path to project root
set(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(VENDOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../vendor")

# Include local static libraries
add_subdirectory("${PROJECT_DIR}/lib_server")
add_subdirectory("${PROJECT_DIR}/lib_database")

# Update git submodules automatically
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build of server" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

# Include vendor libraries
add_subdirectory("${VENDOR_DIR}/drogon" bin_drogon)
add_subdirectory("${VENDOR_DIR}/indicators" bin_indicators)
add_subdirectory("${VENDOR_DIR}/rapidjson" bin_rapidjson)
add_subdirectory("${VENDOR_DIR}/sqlpp11" bin_sqlpp11)
add_subdirectory("${VENDOR_DIR}/structopt" bin_structopt)

# Set links to local source/header files
set(INCLUDE_FILES_DIR "${PROJECT_DIR}/include")
set(SOURCE_FILES_DIR "${PROJECT_DIR}/src")
set(MAIN_SOURCE_FILE "${SOURCE_FILES_DIR}/main.cpp")
set(CONFIG_SOURCE_FILE "${SOURCE_FILES_DIR}/config.in.hpp")

# Collect all files (for IDEs)
file(
    GLOB_RECURSE ALL_SOURCE_FILES
    "${SOURCE_FILES_DIR}/*.cpp"
    "${SOURCE_FILES_DIR}/**/*.cpp"
)
file(
    GLOB_RECURSE ALL_HEADER_FILES
    "${INCLUDE_FILES_DIR}/*.hpp"
    "${INCLUDE_FILES_DIR}/**/*.hpp"
    "${VENDOR_DIR}/drogon/lib/inc/**/*.hpp"
    "${VENDOR_DIR}/indicators/include/**/*.hpp"
    "${VENDOR_DIR}/rapidjson/include/**/*.hpp"
    "${VENDOR_DIR}/sqlpp11/include/**/*.hpp"
    "${VENDOR_DIR}/structopt/include/**/*.hpp"
)

# Output all indexed source files
message(STATUS "Main: ${MAIN_SOURCE_FILE}")
message(STATUS "Sources:")
foreach(SOURCE_FILE ${ALL_SOURCE_FILES})
    message(STATUS " - ${SOURCE_FILE}")
endforeach()
message(STATUS "Headers:")
foreach(HEADER_FILE ${ALL_HEADER_FILES})
    message(STATUS " - ${HEADER_FILE}")
endforeach()

# Create a configure file
set(VERSION "1.0.0")
configure_file("${CONFIG_SOURCE_FILE}"
               "${PROJECT_BINARY_DIR}/config.hpp")

# Create executable with all provided sources
# (include headers for better IDE support)
add_executable(
    ${PROJECT_NAME}
    "${MAIN_SOURCE_FILE}"
    ${ALL_SOURCE_FILES}
    ${ALL_HEADER_FILES}
)

# Include the include directories of all modules and the local headers
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${INCLUDE_FILES_DIR}
    ${SOURCE_FILES_DIR}
)

# Include the configure file in the binary directory
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# Include the include directory of rapidjson
target_include_directories(
    ${PROJECT_NAME}
    SYSTEM
    INTERFACE
    "${VENDOR_DIR}/rapidjson/include"
)

# Link structopt
target_link_libraries(${PROJECT_NAME} PRIVATE structopt::structopt)

# Link sqlpp11
target_link_libraries(${PROJECT_NAME} PRIVATE sqlpp11::sqlpp11)

# Set library source files compilation flags for different compilers
target_compile_options(
    ${PROJECT_NAME}
    PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:
    -pedantic
    -Wall
    -Werror
    -Wextra
    -Wformat
    >
    $<$<CXX_COMPILER_ID:MSCV>:
    /W4
    /Wall
    /WX
    >
)

# Set C++ version for the local source files
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

# Set Debug post fix to differentiate a debug and release executable
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
