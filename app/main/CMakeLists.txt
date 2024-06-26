cmake_minimum_required(VERSION 3.28)

#
# The problem:
# Why are my tests in a static library not getting registered?
# https://github.com/doctest/doctest/blob/master/doc/markdown/faq.md#why-are-my-tests-in-a-static-library-not-getting-registered
#
# With current setup using C++20 modules, things go not that bad.
#
# When STATIC library type is used, doctest test runner can not
# pick up the tests inside library and it's modules containing only
# templates. But when libraries, more precisely, their modules, contain
# real code, they show up their tests, even when built as STATIC.
#
# When we use OBJECT library type, test runner can pick up the tests
# inside the library and executes them, even if there are only templates.
#
# But the real problem with OBJECT library is coping with the linker mess.
# So, we are going to use STATIC library type for now as the default,
# and leave OBJECT library type as a fallback in case all will be broken
# or insufficient for the testing.
#
function(configure_lib this_lib)
    add_lib_target(${this_lib})

    # Sources
    target_sources(${this_lib}
        PUBLIC FILE_SET ${this_lib}_modules TYPE CXX_MODULES
        FILES
        main.ixx
        main.consts.cxx
        main.concepts.cxx
        main.initialization.cxx
        main.core.cxx
        main.core.imgui.cxx
        main.start.cxx
    )

    # Dependencies
    use_glad_package(${this_lib})
    use_glfw3_package(${this_lib})
    use_imgui_package(${this_lib})
    use_logging_package(${this_lib})

    target_link_libraries(${this_lib} PRIVATE imgui_ext)
    target_link_libraries(${this_lib} PRIVATE utilities)
endfunction()

configure_lib(main)