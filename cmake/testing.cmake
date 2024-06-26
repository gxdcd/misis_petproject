# TLDR:
# https://cliutils.gitlab.io/modern-cmake/chapters/testing.html
# https://cmake.org/cmake/help/book/mastering-cmake/chapter/Testing%20With%20CMake%20and%20CTest.html

# https://github.com/doctest/doctest
# https://cliutils.gitlab.io/modern-cmake/chapters/testing/googletest.html
# https://cliutils.gitlab.io/modern-cmake/chapters/testing/catch.html

# We use `doctest` for unit testing as default
find_package(doctest CONFIG REQUIRED)
include(doctest)

# Remove noise from the output about this file is missing
file(GENERATE OUTPUT DartConfiguration.tcl CONTENT
    "Site:Localhost
    BuildName:${CMAKE_CXX_PLATFORM_ID}-${CMAKE_CXX_COMPILER_ID}")

# Used to create a test target name from a test name
function(get_test_target_name test_name test_target)
    set(${test_target} test.${test_name} PARENT_SCOPE)
endfunction()

# Add a CTest test
# Args:
# test_name: The name of the test
# Options:
# NO_BUILD: Do not build the test automatically with CTest
# ALL: Include the test executable in global ALL target (opposite of EXCLUDE_FROM_ALL)
# WIN32: Use WIN32 subsystem
# REUSE_PCH: Reuse precompiled headers defined by the project
# ADD_CLEAN: Add a clean target for the test
function(add_ctest test_name)
    set(options NO_BUILD ALL WIN32 REUSE_PCH ADD_CLEAN)
    set(oneValueArgs)
    set(multipleValuesArgs SOURCES LIBS)
    cmake_parse_arguments(PARSE_ARGV 1
        arg "${options}" "${oneValueArgs}" "${multipleValuesArgs}")

    get_test_target_name(${test_name} test_target)

    add_exe_target(
        ${test_target}
        ${ARGN}
        FOLDER Tests
    )

    if(arg_NO_BUILD)
        message("Adding test ${test_target}")
        add_test(
            NAME ${test_name}
            COMMAND $<TARGET_FILE:${test_target}>
        )
    else()
        message("Adding test ${test_target} with automatic build step")

        # CMake & CTest : make test doesn't build tests
        # [-] https://stackoverflow.com/questions/733475/cmake-ctest-make-test-doesnt-build-tests
        # Running only one single test with CMake + make
        # [-] https://stackoverflow.com/questions/54160415/running-only-one-single-test-with-cmake-make
        # Using CTest to Drive Complex Tests
        # [v] https://cmake.org/cmake/help/book/mastering-cmake/chapter/Testing%20With%20CMake%20and%20CTest.html#using-ctest-to-drive-complex-tests
        add_test(
            NAME ${test_name}
            COMMAND ${CMAKE_CTEST_COMMAND}
            --build-and-test "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}"
            --build-nocmake
            --build-noclean
            --build-generator ${CMAKE_GENERATOR}
            --build-makeprogram ${CMAKE_MAKE_PROGRAM}
            --build-target ${test_target}
            --test-command $<TARGET_FILE:${test_target}>
            #[=[
                [v] --build-and-test             = Configure, build and run a test.
                [v] --build-target               = Specify a specific target to build.
                [v] --build-nocmake              = Run the build without running cmake first.
                [ ] --build-run-dir              = Specify directory to run programs from.
                [ ] --build-two-config           = Run CMake twice
                [ ] --build-exe-dir              = Specify the directory for the executable.
                [v] --build-generator            = Specify the generator to use.
                [ ] --build-generator-platform   = Specify the generator-specific platform.
                [ ] --build-generator-toolset    = Specify the generator-specific toolset.
                [ ] --build-project              = Specify the name of the project to build.
                [v] --build-makeprogram          = Specify the make program to use.
                [v] --build-noclean              = Skip the make clean step.
                [ ] --build-config-sample        = A sample executable to use to determine the configuration
                [ ] --build-options              = Add extra options to the build step.
            ]=]
        )
    endif()
endfunction()

function(use_doctest target)
    set(options WITH_MAIN CONFIG_IMPLEMENT)
    cmake_parse_arguments(PARSE_ARGV 1
        arg "${options}" "${oneValueArgs}" "${multipleValuesArgs}")

    if(arg_WITH_MAIN)
        target_compile_definitions(${target} PRIVATE DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN)
        target_compile_definitions(${target} PRIVATE DOCTEST_SINGLE_HEADER)
    endif()

    if(arg_CONFIG_IMPLEMENT)
        target_compile_definitions(${target} PRIVATE DOCTEST_CONFIG_IMPLEMENT)
    endif()

    get_target_property(doctest_include_dirs doctest::doctest INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(${target} PUBLIC ${doctest_include_dirs})
    target_compile_options(${target} PUBLIC /FI${doctest_include_dirs}/doctest/doctest.h)
    target_link_libraries(${target} PUBLIC doctest::doctest)
endfunction()

# Add a `doctest` test
# Args:
#   test_name: The name of the test
# Options:
#   NO_BUILD: Do not build the test automatically with CTest
#   ALL: Include the test executable in global ALL target (opposite of EXCLUDE_FROM_ALL)
#   WIN32: Use WIN32 subsystem
#   REUSE_PCH: Reuse precompiled headers defined by the project
#   ADD_CLEAN: Add a clean target for the test
function(add_doctest test_name)
    add_ctest(
        ${test_name}
        ${ARGN}
    )
    get_test_target_name(${test_name} test_target)
    use_doctest(${test_target} WITH_MAIN)
endfunction()