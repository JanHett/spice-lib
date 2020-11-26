message(STATUS "Pulling in ConanSetup")

function(conan_install)
    message(STATUS "Running conan_install")
    include(PlatformInfo)

    set(SPICE_CONAN_PROFILE_TEMPLATE "${PROJECT_SOURCE_DIR}/conan_profile.in")
    set(SPICE_CONAN_PROFILE "${CMAKE_BINARY_DIR}/conan_profile")

    configure_file(${SPICE_CONAN_PROFILE_TEMPLATE} ${SPICE_CONAN_PROFILE}
        @ONLY)

    if (NOT EXISTS ${SPICE_CONAN_PROFILE})
        message(FATAL_ERROR "Conan profile was not configured. Aborting configuration.")
    endif()

    set(CONAN_INSTALL_COMMAND conan install ${PROJECT_SOURCE_DIR} --profile ${SPICE_CONAN_PROFILE} --build=missing)
    message(STATUS "Executing `${CONAN_INSTALL_COMMAND}`")
    execute_process(COMMAND ${CONAN_INSTALL_COMMAND}
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
                    RESULT_VARIABLE CONAN_RESULT
                    )

    message(NOTICE "conan install exited with result `${CONAN_RESULT}`")

    if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake" AND
        EXISTS "${CMAKE_BINARY_DIR}/conan_paths.cmake")
        include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
        include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
        conan_basic_setup(TARGETS)
    else()
        message(NOTICE "Could not find conanbuildinfo.cmake or conan_paths.cmake")
        message(NOTICE "conan_profile:")
        execute_process(COMMAND cat ${SPICE_CONAN_PROFILE})
        message(FATAL_ERROR "Conan setup failed. Aborting configuration.")
    endif()
    message(STATUS "conan_install finished")
endfunction()

set (SPICE_DEPENDENCY_CONAN_TARGETS
    CONAN_PKG::openimageio
    CONAN_PKG::fftw
    )
