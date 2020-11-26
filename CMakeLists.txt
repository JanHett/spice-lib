# changes To-Do (https://www.youtube.com/watch?v=bsXLMQ6WgIk):

# remove file(GLOB)

# set version number

# remove explicit compile flags, and required version...
# set them on targets:
# --> target_compile_features(<target> PUBLIC ... INTERFACE ... PRIVATE ...)

# remove -Wall etc from *requirements* - only use it for building tests

# add install commands and export library interface (38:00 +)

# add alias spice::spice

# CTEST! ctest -S build.cmake <== this is where CI is run

# target properties for static analysis (especially CMAKE_CXX_CLANG_TIDY)


################################################################################

cmake_minimum_required(VERSION 3.16)

project("spice-lib"
             DESCRIPTION "An image processing library."
             HOMEPAGE_URL "https://github.com/janhett/spice-lib")

set(PROJECT_VERSION 0.1.0)

include(GNUInstallDirs)

if (APPLE)
    # Fix linking on 10.14+. See https://stackoverflow.com/questions/54068035
    LINK_DIRECTORIES(/usr/local/lib)
endif()

#############################
# Dependency setup
#############################

set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/")

# Include cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(USE_CONAN "Use `conan` for dependency management" OFF)

if (USE_CONAN)
  include(ConanSetup)
  conan_install()
endif()

#~~~~~~~~~~~~~~~~~~~~~~~~~~~#
# OpenImageIO
#~~~~~~~~~~~~~~~~~~~~~~~~~~~#

if (NOT USE_CONAN)
  find_package(OpenImageIO REQUIRED)
endif()

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

# if we don't use conan, we need to add the include dir for OIIO
if (NOT USE_CONAN)
  target_include_directories(${PROJECT_NAME} INTERFACE ${OIIO_INCLUDE_DIRS})
endif()

# use conan-provided packages if we can, otherwise fall back to global installs
if (USE_CONAN)
  target_link_libraries(${PROJECT_NAME}
    INTERFACE
        CONAN_PKG::openimageio
        CONAN_PKG::fftw
  )
else()
  target_link_libraries(${PROJECT_NAME}
    INTERFACE
        OpenImageIO
        # fftw3_threads # TODO: make this work
        fftw3f
        # Threads::Threads
  )
endif()

# all the other dependencies...
target_link_libraries(${PROJECT_NAME}
    INTERFACE
        GSL

        # Halide-generated code
        ## Custom Halide functions
        ### Thresholds
        adaptive_threshold_halide

        ### Buffer x Buffer arithmetic
        add_buffers_halide
        subtract_buffers_halide
        multiply_buffers_halide
        divide_buffers_halide

        ### Buffer x Scalar arithmetic
        add_buffer_scalar_halide
        subtract_buffer_scalar_halide
        multiply_buffer_scalar_halide
        divide_buffer_scalar_halide

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

# TODO: fix CI and remove this/move it to a toolchain file!
if (UNIX AND NOT APPLE)
  target_compile_options(${PROJECT_NAME} INTERFACE -fno-rtti)
endif()

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
