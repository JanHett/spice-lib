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

set(Halide_ROOT_DIR "${EXTERNAL_DIR}/Halide/")
set(Halide_DIR "${Halide_ROOT_DIR}/lib/cmake/Halide/")

message(STATUS "Installing Halide in ${Halide_ROOT_DIR}")

if(NOT EXISTS "${Halide_ROOT_DIR}")
    # we couldn't find the header files for GSL or they don't exist
    message("Unable to find Halide. Attempting to pull pre-built binaries.")

    if(APPLE)
      # curl Halide build
      set(HALIDE_MACOS_BUILD_URL https://github.com/halide/Halide/releases/download/v10.0.0/Halide-10.0.0-x86-64-osx-db901f7f7084025abc3cbb9d17b0f2d3f1745900.tar.gz)
      file(DOWNLOAD ${HALIDE_MACOS_BUILD_URL} "${EXTERNAL_DIR}__halide_temp.tar.gz"
           TIMEOUT 60)
      # Ensure the target directory exists
      execute_process(COMMAND mkdir -p ${Halide_ROOT_DIR}
                      WORKING_DIRECTORY "${CMAKE_PROJECT_SOURCE_DIR}")
      # Unpack the Halide build
      execute_process(COMMAND tar -xzf __halide_temp.tar.gz -C Halide --strip-components 1
                      WORKING_DIRECTORY "${EXTERNAL_DIR}")
      # Remove the archive
      execute_process(COMMAND rm -f __halide_temp.tar.gz
                      WORKING_DIRECTORY "${EXTERNAL_DIR}")
    endif()

    # TODO: replicate this for other OSes
endif()

find_package(Halide REQUIRED)

#############################
# Target setup
#############################

#~~~~~~~~~~~~~~~~~~~~~~~~~~~#
# Halide generator executable
#~~~~~~~~~~~~~~~~~~~~~~~~~~~#

file(GLOB sources
    *.cpp
    **/*.cpp
)

message(STATUS "Compiling from ${sources}")

set(CMAKE_CXX_STANDARD 11)  # or newer
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_executable(spice_lib_generators ${sources})
target_link_libraries(spice_lib_generators PRIVATE Halide::Generator)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~#
# Halide libraries
#~~~~~~~~~~~~~~~~~~~~~~~~~~~#

add_halide_library(adaptive_threshold_generator FROM spice_lib_generators)