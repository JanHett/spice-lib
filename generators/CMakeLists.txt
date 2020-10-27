cmake_minimum_required(VERSION 3.16)

set(target ${PROJECT_NAME}-generators)
message(STATUS "Configuring ${target}")

#############################
# Dependency setup
#############################

set(EXTERNAL_DIR "${PROJECT_SOURCE_DIR}/external/")

#~~~~~~~~~~~~~~~~~~~~~~~~~~~#
# Halide
#~~~~~~~~~~~~~~~~~~~~~~~~~~~#

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/Halide/CMakeLists.txt")
    # we couldn't find Halide
    message("Unable to find Halide. Attempting to pull.")

    # we have a submodule setup for Halide, so we clone it
    execute_process(COMMAND git submodule update --init -- external/Halide
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif()

add_subdirectory(../external/Halide Halide)

#############################
# Target setup
#############################

set(CMAKE_CXX_STANDARD 11)  # or newer
set(CMAKE_CXX_STANDARD_REQUIRED YES)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~#
# Halide generator executable
#~~~~~~~~~~~~~~~~~~~~~~~~~~~#

#
# Custom generators
#

file(GLOB custom_generator_sources
    *.cpp
    **/*.cpp
)

message(STATUS
    "Compiling custom Halide generators from ${custom_generator_sources}")

add_executable(spice_lib_generators ${custom_generator_sources})
target_link_libraries(spice_lib_generators PRIVATE Halide::Generator)

#
# Halide apps
#

# set(HALIDE_APPS_ROOT ../external/Halide/apps/)

# set(halide_apps_sources
#     # FFT
#     ${HALIDE_APPS_ROOT}fft/fft_generator.cpp
#     ${HALIDE_APPS_ROOT}fft/fft.cpp
# )

# message(STATUS "Compiling Halide apps generators from ${halide_apps_sources}")

# add_executable(halide_apps_generators ${halide_apps_sources})
# target_include_directories(halide_apps_generators PRIVATE
#     ${HALIDE_APPS_ROOT}fft
#     )

# target_link_libraries(halide_apps_generators PRIVATE Halide::Generator)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~#
# Halide libraries
#~~~~~~~~~~~~~~~~~~~~~~~~~~~#

#
# HALIDE APPS
#

# FFT

# add_halide_library(fft_2d_forward_halide FROM halide_apps_generators
#     GENERATOR fft
#     PARAMS direction=samples_to_frequency size0=1 size1=1)

# add_halide_library(fft_2d_reverse_halide FROM halide_apps_generators
#     GENERATOR fft
#     PARAMS direction=frequency_to_samples size0=1 size1=1)

#
# Custom generators
#

add_halide_library(adaptive_threshold_halide FROM spice_lib_generators)
add_halide_library(add_buffers_halide FROM spice_lib_generators
    USE_RUNTIME adaptive_threshold_halide.runtime)
add_halide_library(subtract_buffers_halide FROM spice_lib_generators
    USE_RUNTIME adaptive_threshold_halide.runtime)
add_halide_library(multiply_buffers_halide FROM spice_lib_generators
    USE_RUNTIME adaptive_threshold_halide.runtime)
add_halide_library(divide_buffers_halide FROM spice_lib_generators
    USE_RUNTIME adaptive_threshold_halide.runtime)
