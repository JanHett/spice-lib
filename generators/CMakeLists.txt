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

if(UNIX AND NOT APPLE)
    # Linux build fails if we are linking aginst linuxbrew LLVM and don't use
    # the SHARED_LLVM
    set(Halide_SHARED_LLVM ON)

    set(TARGET_WEBASSEMBLY OFF)
    set(TARGET_METAL OFF)
    set(TARGET_D3D12COMPUTE OFF)
endif()

if(USE_CONAN)
    include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
endif()

add_subdirectory(${EXTERNAL_DIR}/Halide Halide)

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

# GPU features available per platform
set(GPU_FEATURES cuda opencl)

if(APPLE)
    list(PREPEND GPU_FEATURES metal)
elseif(WIN32)
    list(PREPEND GPU_FEATURES d3d12compute)
endif()

# Segmentation
add_halide_library(adaptive_threshold_halide FROM spice_lib_generators)

# Buffer arithmetic
add_halide_library(add_buffers_halide FROM spice_lib_generators
    USE_RUNTIME adaptive_threshold_halide.runtime)
add_halide_library(subtract_buffers_halide FROM spice_lib_generators
    USE_RUNTIME adaptive_threshold_halide.runtime)
add_halide_library(multiply_buffers_halide FROM spice_lib_generators
    USE_RUNTIME adaptive_threshold_halide.runtime)
add_halide_library(divide_buffers_halide FROM spice_lib_generators
    USE_RUNTIME adaptive_threshold_halide.runtime)

add_halide_library(add_buffer_scalar_halide FROM spice_lib_generators
    USE_RUNTIME adaptive_threshold_halide.runtime)
add_halide_library(subtract_buffer_scalar_halide FROM spice_lib_generators
    USE_RUNTIME adaptive_threshold_halide.runtime)
add_halide_library(multiply_buffer_scalar_halide FROM spice_lib_generators
    USE_RUNTIME adaptive_threshold_halide.runtime)
add_halide_library(divide_buffer_scalar_halide FROM spice_lib_generators
    USE_RUNTIME adaptive_threshold_halide.runtime)

# Buffer arithmetic
# add_halide_library(add_buffers_halide FROM spice_lib_generators
#     USE_RUNTIME adaptive_threshold_halide.runtime)
# add_halide_library(subtract_buffers_halide FROM spice_lib_generators
#     USE_RUNTIME adaptive_threshold_halide.runtime)
add_halide_library(multiply_complex_buffers_halide FROM spice_lib_generators
    FEATURES ${GPU_FEATURES}
    USE_RUNTIME adaptive_threshold_halide.runtime)
# add_halide_library(divide_buffers_halide FROM spice_lib_generators
#     USE_RUNTIME adaptive_threshold_halide.runtime)

# Convolution
add_halide_library(spatial_convolution_halide FROM spice_lib_generators
    USE_RUNTIME adaptive_threshold_halide.runtime)
