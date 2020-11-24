function(conan_install)
    include(CompilerInfo)

    set(SPICE_CONAN_PROFILE_TEMPLATE "${CMAKE_PROJECT_SOURCE_DIR}/conan_profile.in")
    set(SPICE_CONAN_PROFILE "${CMAKE_BINARY_DIR}/conan_profile")

    configure_file(${SPICE_CONAN_PROFILE_TEMPLATE} ${SPICE_CONAN_PROFILE}
        @ONLY)

    execute_process(COMMAND conan install ${CMAKE_PROJECT_SOURCE_DIR} --profile ${SPICE_CONAN_PROFILE} --build=missing
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

    if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
        include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
        conan_basic_setup(TARGETS)
    elseif()
        message(NOTICE "Could not find conanbuildinfo.cmake")
    endif()
endfunction()

set (SPICE_DEPENDENCY_CONAN_TARGETS
    CONAN_PKG::openimageio
    CONAN_PKG::fftw
    )
