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
#add_subdirectory("${VENDOR_DIR}/drogon" bin_drogon)
#add_subdirectory("${VENDOR_DIR}/indicators" bin_indicators)
#add_subdirectory("${VENDOR_DIR}/rapidjson" bin_rapidjson) # No exports of targets
add_subdirectory("${VENDOR_DIR}/sqlpp11" bin_sqlpp11)
add_subdirectory("${VENDOR_DIR}/structopt" bin_structopt)
add_subdirectory("${VENDOR_DIR}/fmt" bin_fmt)
#add_subdirectory("${VENDOR_DIR}/catch2" bin_catch2)
#add_subdirectory("${VENDOR_DIR}/spdlog" bin_spdlog) # No exports of targets

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
    "${VENDOR_DIR}/catch2/include/**/*.hpp"
    "${VENDOR_DIR}/spdlog/include/**/*.hpp"
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

# Read licenses into variables
function(create_git_submodule_information submodule_name submodule_directory submodule_license_file)
    message(STATUS " - submodule_name:         ${submodule_name}")
    message(STATUS " - submodule_directory:    ${submodule_directory}")
    message(STATUS " - submodule_license_file: ${submodule_license_file}")

    # Get GIT URL
    execute_process(COMMAND git config --get remote.origin.url
    WORKING_DIRECTORY "${submodule_directory}"
    OUTPUT_VARIABLE GIT_URL)
    string(REPLACE "\n" "" GIT_URL "${GIT_URL}")
    set("GIT_URL_${submodule_name}" "${GIT_URL}" PARENT_SCOPE)

    execute_process(COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY "${submodule_directory}"
    OUTPUT_VARIABLE GIT_HASH)
    string(REPLACE "\n" "" GIT_HASH "${GIT_HASH}")
    set("GIT_HASH_${submodule_name}" "${GIT_HASH}" PARENT_SCOPE)

    file(READ "${submodule_directory}/${submodule_license_file}" LICENSE)
    string(REPLACE "\n" "\\n" LICENSE "${LICENSE}")
    string(REPLACE "\"" "\\\"" LICENSE "${LICENSE}")
    set("LICENSE_${submodule_name}" "${LICENSE}" PARENT_SCOPE)
endfunction()

create_git_submodule_information("RAPIDJSON" "${VENDOR_DIR}/rapidjson" "license.txt")
create_git_submodule_information("STRUCTOPT" "${VENDOR_DIR}/structopt" "LICENSE")
create_git_submodule_information("SQLPP11" "${VENDOR_DIR}/sqlpp11" "LICENSE")
create_git_submodule_information("SPDLOG" "${VENDOR_DIR}/spdlog" "LICENSE")
create_git_submodule_information("INDICATORS" "${VENDOR_DIR}/indicators" "LICENSE")
create_git_submodule_information("FMT" "${VENDOR_DIR}/fmt" "LICENSE.rst")
create_git_submodule_information("DROGON" "${VENDOR_DIR}/drogon" "LICENSE")
create_git_submodule_information("CATCH2" "${VENDOR_DIR}/catch2" "LICENSE.txt")

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
    PRIVATE
    "${VENDOR_DIR}/rapidjson/include"
)

# Include the include directory of spdlog
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    "${VENDOR_DIR}/spdlog/include"
)

# Link structopt
target_link_libraries(${PROJECT_NAME} PRIVATE structopt::structopt)

# Link sqlpp11
target_link_libraries(${PROJECT_NAME} PRIVATE sqlpp11::sqlpp11)

# Link fmt
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

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
