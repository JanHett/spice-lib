# 
# External dependencies
# 

# find_package(${PROJECT_NAME} REQUIRED HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../")
# add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../")

# 
# Executable name and options
# 

# Target name
set(target ${PROJECT_NAME}-benchmark)
message(STATUS "Configuring ${target}")


# 
# Sources
# 

file(GLOB sources
    *.cpp
    **/*.cpp
)

# 
# Create executable
# 

# Build executable
add_executable(${target}
    ${sources}
)

# Create namespaced alias
add_executable(${PROJECT_NAME}::${target} ALIAS ${target})


# 
# Project options
# 

# set_target_properties(${target}
#     PROPERTIES
#     ${DEFAULT_PROJECT_OPTIONS}
#     FOLDER "${IDE_FOLDER}"
# )


# 
# Include directories
# 

target_include_directories(${target}
    PRIVATE
    # ${DEFAULT_INCLUDE_DIRECTORIES}
    ${PROJECT_BINARY_DIR}/external
    ${PROJECT_BINARY_DIR}/include
    ${PROJECT_BINARY_DIR}/tools
)


# 
# Libraries
#

# Pull benchmark library if it isn't there

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/benchmark/CMakeLists.txt")
    # we couldn't find the header files for Google benchmark or they don't exist
    message("Unable to find benchmark. Attempting to pull.")

    # we have a submodule setup for halide, so we clone it
    execute_process(COMMAND git submodule update --init -- external/benchmark
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif()

set(BENCHMARK_ENABLE_GTEST_TESTS OFF)

add_subdirectory(../external/benchmark benchmark)

target_link_libraries(${target}
    PRIVATE
    # ${DEFAULT_LIBRARIES}
    ${PROJECT_NAME}::${PROJECT_NAME}
    benchmark::benchmark
)


# 
# Compile definitions
# 

# target_compile_definitions(${target}
#     PRIVATE
#     ${DEFAULT_COMPILE_DEFINITIONS}
# )


# 
# Compile options
# 

target_compile_options(${target}
    PRIVATE
    -Wall
    -Wextra
    -pedantic
    -g
    -O3
    # -O1
    # -fno-omit-frame-pointer
    # -fsanitize=address
)

# target_link_options(${target}
#     PRIVATE
#     -fsanitize=address
# )


# 
# Linker options
# 

target_link_libraries(${target}
    PRIVATE
    # ${DEFAULT_LINKER_OPTIONS}
    ${PROJECT_NAME}
)