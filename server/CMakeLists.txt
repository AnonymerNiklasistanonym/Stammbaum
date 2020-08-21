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
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMOD_RESULT
        )
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(
                FATAL_ERROR
                "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules"
            )
        endif()
    endif()
endif()

# Check if thread library can be found
find_package(Threads REQUIRED)

# Include vendor libraries
add_subdirectory("${VENDOR_DIR}/drogon" bin_drogon)
#add_subdirectory("${VENDOR_DIR}/indicators" bin_indicators)
#add_subdirectory("${VENDOR_DIR}/rapidjson" bin_rapidjson) # No exports of targets
add_subdirectory("${VENDOR_DIR}/sqlpp11" bin_sqlpp11)
# set(DATE_INCLUDE_DIR "${VENDOR_DIR}/date/include")
add_subdirectory("${VENDOR_DIR}/sqlpp11_connector_mysql" bin_sqlpp11_connector_mysql)
add_subdirectory("${VENDOR_DIR}/structopt" bin_structopt)
add_subdirectory("${VENDOR_DIR}/fmt" bin_fmt)
#add_subdirectory("${VENDOR_DIR}/catch2" bin_catch2)
#add_subdirectory("${VENDOR_DIR}/spdlog" bin_spdlog) # No exports of targets

# Set links to local source/header files
set(INCLUDE_FILES_DIR "${PROJECT_DIR}/include")
set(SOURCE_FILES_DIR "${PROJECT_DIR}/src")
set(MAIN_SOURCE_FILE "${SOURCE_FILES_DIR}/main.cpp")
set(CONFIG_SOURCE_FILE "${SOURCE_FILES_DIR}/config.in.hpp")

# Drogon source files
aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)

# Drogon view compilations
file(GLOB_RECURSE SCP_LIST ${CMAKE_CURRENT_SOURCE_DIR}/views/*.csp)
foreach(cspFile ${SCP_LIST})
    message(STATUS "cspFile:" ${cspFile})
    get_filename_component(classname ${cspFile} NAME_WE)
    message(STATUS "view classname:" ${classname})
    ADD_CUSTOM_COMMAND(OUTPUT ${classname}.h ${classname}.cc
            COMMAND drogon_ctl
            ARGS create view ${cspFile}
            DEPENDS ${cspFile}
            VERBATIM )
    set(VIEWSRC ${VIEWSRC} ${classname}.cc)
endforeach()

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

# Read git submodule information into variables
function(create_git_submodule_information)
    set(options)
    set(oneValueArgs ID DIR LICENSE_FILE_PATH)
    set(multiValueArgs)
    cmake_parse_arguments(
        SUBMODULE_INFO
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    # Get git submodule repository URL
    # gersemi: off
    execute_process(
        COMMAND git config --get remote.origin.url
        WORKING_DIRECTORY "${SUBMODULE_INFO_DIR}"
        OUTPUT_VARIABLE GIT_URL
    )
    # gersemi: on
    string(REPLACE "\n" "" GIT_URL "${GIT_URL}")
    set("GIT_URL_${SUBMODULE_INFO_ID}" "${GIT_URL}" PARENT_SCOPE)
    # Get git submodule git HASH
    # gersemi: off
    execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY "${SUBMODULE_INFO_DIR}"
        OUTPUT_VARIABLE GIT_HASH
    )
    # gersemi: on
    string(REPLACE "\n" "" GIT_HASH "${GIT_HASH}")
    set("GIT_HASH_${SUBMODULE_INFO_ID}" "${GIT_HASH}" PARENT_SCOPE)
    # Get git submodule license
    file(
        READ "${SUBMODULE_INFO_DIR}/${SUBMODULE_INFO_LICENSE_FILE_PATH}" LICENSE
    )
    string(REPLACE "\n" "\\n" LICENSE "${LICENSE}")
    string(REPLACE "\"" "\\\"" LICENSE "${LICENSE}")
    set("LICENSE_${SUBMODULE_INFO_ID}" "${LICENSE}" PARENT_SCOPE)
endfunction()
create_git_submodule_information(
    ID "RAPIDJSON"
    DIR "${VENDOR_DIR}/rapidjson"
    LICENSE_FILE_PATH "license.txt"
)
create_git_submodule_information(
    ID "STRUCTOPT"
    DIR "${VENDOR_DIR}/structopt"
    LICENSE_FILE_PATH "LICENSE"
)
create_git_submodule_information(
    ID "SQLPP11"
    DIR "${VENDOR_DIR}/sqlpp11"
    LICENSE_FILE_PATH "LICENSE"
)
create_git_submodule_information(
    ID "SQLPP11_CONNECTOR_MYSQL"
    DIR "${VENDOR_DIR}/sqlpp11_connector_mysql"
    LICENSE_FILE_PATH "LICENSE"
)
create_git_submodule_information(
    ID "SPDLOG"
    DIR "${VENDOR_DIR}/spdlog"
    LICENSE_FILE_PATH "LICENSE"
)
create_git_submodule_information(
    ID "INDICATORS"
    DIR "${VENDOR_DIR}/indicators"
    LICENSE_FILE_PATH "LICENSE"
)
create_git_submodule_information(
    ID "FMT"
    DIR "${VENDOR_DIR}/fmt"
    LICENSE_FILE_PATH "LICENSE.rst"
)
create_git_submodule_information(
    ID "DROGON"
    DIR "${VENDOR_DIR}/drogon"
    LICENSE_FILE_PATH "LICENSE"
)
create_git_submodule_information(
    ID "CATCH2"
    DIR "${VENDOR_DIR}/catch2"
    LICENSE_FILE_PATH "LICENSE.txt"
)

# Create a configure file
set(VERSION "1.0.0")
configure_file("${CONFIG_SOURCE_FILE}" "${PROJECT_BINARY_DIR}/config.hpp")

# Create executable with all provided sources
# (include headers for better IDE support)
add_executable(
    ${PROJECT_NAME}
    "${MAIN_SOURCE_FILE}"
    ${ALL_SOURCE_FILES}
    ${ALL_HEADER_FILES}
    ${SRC_DIR} ${CTL_SRC} ${FILTER_SRC} ${VIEWSRC} ${PLUGIN_SRC} ${MODEL_SRC}
)

# Include the include directories of all modules and the local headers
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${INCLUDE_FILES_DIR}
    ${SOURCE_FILES_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/models
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

# Include the include directory of sqlpp11 connector
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    "${VENDOR_DIR}/sqlpp11_connector_mysql/include"
)

# Link structopt
target_link_libraries(${PROJECT_NAME} PRIVATE structopt::structopt)

# Link sqlpp11
target_link_libraries(${PROJECT_NAME} PRIVATE sqlpp11::sqlpp11)

# Link sqlpp11 connector mysql
target_link_libraries(${PROJECT_NAME} PRIVATE sqlpp-mysql)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${PROJECT_NAME} PRIVATE "${MYSQL_LIBRARIES}")

# Link fmt
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

# Link drogon
target_link_libraries(${PROJECT_NAME} PRIVATE drogon)

# Set library source files compilation flags for different compilers
target_compile_options(
    ${PROJECT_NAME}
    PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:
    -pedantic
    -Wall
    #-Werror
    -Wextra
    -Wformat
    >
    $<$<CXX_COMPILER_ID:MSCV>:
    /W4
    /Wall
    /WX
    >
)

# Set C++ version for the project
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

# Set Debug post fix to differentiate a debug and release executable
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
