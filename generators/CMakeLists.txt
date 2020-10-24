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

# set(Halide_ROOT_DIR "${EXTERNAL_DIR}/Halide/")
# set(Halide_DIR "${Halide_ROOT_DIR}/lib/cmake/Halide/")

# message(STATUS "Installing Halide in ${Halide_ROOT_DIR}")

# if(NOT EXISTS "${Halide_ROOT_DIR}")
#     # we couldn't find the header files for GSL or they don't exist
#     message("Unable to find Halide. Attempting to pull pre-built binaries.")

#     if(APPLE)
#       # curl Halide build
#       set(HALIDE_MACOS_BUILD_URL https://github.com/halide/Halide/releases/download/v10.0.0/Halide-10.0.0-x86-64-osx-db901f7f7084025abc3cbb9d17b0f2d3f1745900.tar.gz)
#       file(DOWNLOAD ${HALIDE_MACOS_BUILD_URL} "${EXTERNAL_DIR}__halide_temp.tar.gz"
#            TIMEOUT 60)
#       # Ensure the target directory exists
#       execute_process(COMMAND mkdir -p ${Halide_ROOT_DIR}
#                       WORKING_DIRECTORY "${CMAKE_PROJECT_SOURCE_DIR}")
#       # Unpack the Halide build
#       execute_process(COMMAND tar -xzf __halide_temp.tar.gz -C Halide --strip-components 1
#                       WORKING_DIRECTORY "${EXTERNAL_DIR}")
#       # Remove the archive
#       execute_process(COMMAND rm -f __halide_temp.tar.gz
#                       WORKING_DIRECTORY "${EXTERNAL_DIR}")
#     endif()

#     # TODO: replicate this for other OSes
# endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/Halide/")
    # we couldn't find the header files for Halide or they don't exist
    message("Unable to find Halide. Attempting to pull.")

    # we have a submodule setup for halide, so we clone it
    execute_process(COMMAND git submodule update --init -- external/Halide
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif()

add_subdirectory(../external/Halide Halide)

# find_package(Halide REQUIRED)

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

message(STATUS "Compiling custom Halide generators from ${custom_generator_sources}")

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
