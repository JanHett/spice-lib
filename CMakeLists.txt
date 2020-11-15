# changes To-Do (https://www.youtube.com/watch?v=bsXLMQ6WgIk):

# remove file(GLOB)

# set up target for lib: `add_library`

# set up target for tests: `add_executable`

# set version number

# remove explicit compile flags, and required version...
# set them on targets:
# --> target_compile_features(<target> PUBLIC ... INTERFACE ... PRIVATE ...)

# specify PUBLIC/INTERFACE/PRIVATE on target_link_libraries

# remove -Wall etc from *requirements* - only use it for building tests

# add install commands and export library interface (38:00 +)

# add alias spice::spice

# CTEST! ctest -S build.cmake <== this is where CI is run

# remove -Werror

# target properties for static analysis (especially CMAK_CXX_CLANG_TIDY)


################################################################################

cmake_minimum_required(VERSION 3.16)

project("spice-lib"
             DESCRIPTION "An image processing library."
             HOMEPAGE_URL "https://github.com/janhett/spice-lib")

set(PROJECT_VERSION 0.1.0)

include(GNUInstallDirs)

if(APPLE)
    # Fix linking on 10.14+. See https://stackoverflow.com/questions/54068035
    LINK_DIRECTORIES(/usr/local/lib)
endif()

#############################
# Dependency setup
#############################

set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/")

# Include cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#~~~~~~~~~~~~~~~~~~~~~~~~~~~#
# OpenImageIO
#~~~~~~~~~~~~~~~~~~~~~~~~~~~#

find_package(OpenImageIO REQUIRED)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~#
# Google Test
#~~~~~~~~~~~~~~~~~~~~~~~~~~~#

# set(GTEST_ROOT "/usr/local/include/gtest" CACHE PATH "Path to googletest")
# find_package(GTest REQUIRED)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~#
# Guideline Support Library
#~~~~~~~~~~~~~~~~~~~~~~~~~~~#

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/gsl/include/")
    # we couldn't find the header files for GSL or they don't exist
    message("Unable to find Guideline Support Library. Attempting to pull.")

    # we have a submodule setup for gsl, so we clone it
    execute_process(COMMAND git submodule update --init -- external/gsl
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()
add_subdirectory(external/gsl)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~#
# FFTW
#~~~~~~~~~~~~~~~~~~~~~~~~~~~#

set(FFTW_ROOT_DIR "${EXTERNAL_DIR}fftw/")
# set(FFTW_DIR "${FFTW_ROOT_DIR}/lib/cmake/Halide/")

if(NOT EXISTS "${FFTW_ROOT_DIR}")
    # we couldn't find the directory for FFTW
    message("Unable to find FFTW. Attempting to pull pre-built binaries.")

    if(UNIX)
      # curl FFTW build
      set(FFTW_MACOS_BUILD_URL http://fftw.org/fftw-3.3.8.tar.gz)
      file(DOWNLOAD ${FFTW_MACOS_BUILD_URL} "${EXTERNAL_DIR}__fftw_temp.tar.gz"
           TIMEOUT 60)
      # Ensure the target directory exists
      execute_process(COMMAND mkdir -p ${FFTW_ROOT_DIR}
                      WORKING_DIRECTORY "${CMAKE_PROJECT_SOURCE_DIR}")
      # Unpack the FFTW build
      execute_process(COMMAND tar -xzf __fftw_temp.tar.gz -C fftw --strip-components 1
                      WORKING_DIRECTORY "${EXTERNAL_DIR}")
      # Remove the archive
      execute_process(COMMAND rm -f __fftw_temp.tar.gz
                      WORKING_DIRECTORY "${EXTERNAL_DIR}")
    else()
      message("No instructions to pull FFTW for the current OS")
    endif()

    # TODO: replicate this for other OSes (read: Windows)
endif()

add_subdirectory(${FFTW_ROOT_DIR})

# find threads so we can link to them and enable fftw's parallel routines
# find_package(Threads) # TODO: make this work

#~~~~~~~~~~~~~~~~~~~~~~~~~~~#
# Custom Halide generators
#~~~~~~~~~~~~~~~~~~~~~~~~~~~#

add_subdirectory(generators)

#############################
# Libary target setup
#############################

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(
  ${PROJECT_NAME}
  INTERFACE $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

target_include_directories(${PROJECT_NAME} INTERFACE ${OIIO_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
    INTERFACE
        GSL
        OpenImageIO
        # fftw3_threads # TODO: make this work
        fftw3f
        # Threads::Threads

        # Halide-generated code
        ## Custom Halide functions
        ### Thresholds
        adaptive_threshold_halide

        ### Buffer arithmetic
        add_buffers_halide
        subtract_buffers_halide
        multiply_buffers_halide
        divide_buffers_halide

        ### Complex buffer arithmetic
        # add_complex_buffers_halide
        # subtract_complex_buffers_halide
        multiply_complex_buffers_halide
        # divide_complex_buffers_halide

        ### Convolution
        spatial_convolution_halide

        ## Halide apps
        # fft_2d_forward_halide
        # fft_2d_reverse_halide
        )

#############################
# Installation setup
#############################

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION
    ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

# install(EXPORT ${PROJECT_NAME}_Targets
#         FILE ${PROJECT_NAME}Targets.cmake
#         NAMESPACE ${PROJECT_NAME}::
#         DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

# TODO have source ".../include/spice"?
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include DESTINATION include)

#############################
# Testing setup
#############################

option(ENABLE_TESTS "Build unit tests" ON)

if(ENABLE_TESTS)
  add_subdirectory(tests)
endif()

#############################
# Benchmarking setup
#############################

option(ENABLE_BENCHMARKS "Build benchmarks" ON)

if(ENABLE_BENCHMARKS)
  add_subdirectory(benchmark)
endif()

#############################
# Documentation setup
#############################

option(ENABLE_DOCS "Build documentation" ON)

if(ENABLE_DOCS)
  add_subdirectory(doc)
endif()
