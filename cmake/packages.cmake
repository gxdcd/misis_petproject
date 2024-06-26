#
# Configure package dependencies
#

#
# Vulkan/GL/GLES/EGL/GLX/WGL Loader-Generator
#
# https://github.com/Dav1dde/glad
#
# The MIT License (MIT)
#
function(use_glad_package target)
    find_package(glad CONFIG REQUIRED)
    target_link_libraries(${target} PRIVATE glad::glad)
endfunction()

#
# GLFW is an Open Source, multi-platform library for OpenGL
#
# https://www.glfw.org/
#
# zlib License
#
function(use_glfw3_package target)
    target_compile_definitions(${target} PRIVATE GLFW_EXPOSE_NATIVE_WIN32)
    find_package(glfw3 CONFIG REQUIRED)
    target_link_libraries(${target} PRIVATE glfw)
endfunction()

#
# Dear ImGui is a bloat-free graphical user interface library for C++
#
# https://github.com/ocornut/imgui
#
# The MIT License (MIT)
#
# See also:
# https://pixtur.github.io/mkdocs-for-imgui/site/api-imgui/ImGui--Dear-ImGui-end-user/
# https://github.com/ocornut/imgui/wiki/Useful-Extensions
# https://github.com/ocornut/imgui/wiki/Useful-Extensions#cness
#
function(use_imgui_package target)
    find_package(imgui CONFIG REQUIRED)
    target_link_libraries(${target} PRIVATE imgui::imgui)
endfunction()

#
# Very fast, header-only/compiled, C++ logging library.
#
# https://github.com/gabime/spdlog
#
# The MIT License (MIT)
#
function(use_logging_package target)
    find_package(spdlog CONFIG REQUIRED)

    # spdlog::spdlog_header_only has problems with using inside the module
    # target_link_libraries(${target} PRIVATE spdlog::spdlog_header_only)
    target_link_libraries(${target} PRIVATE spdlog::spdlog)

    # Because spdlog is hidden inside a module, all it's includes are
    # hidden inside the module and are not visible automagically.
    #
    # Due to this, we often can see the following error message(s):
    #
    # call to consteval function
    # " fmt::v10::basic_format_string<Char, Args...>::basic_format_string(const S &s)
    #
    #
    # [with Char=char, Args=<std::string &>, S=char [10], <unnamed>=0]"
    # did not produce a valid constant expressionC/C++(3133)
    #
    # The code actually compiles, but the IDE intellisense gives false positives.
    #
    # To fix this, we need to include necessary headers included by spdlog indirectly,
    # by including the fmt library in case SPDLOG_USE_STD_FORMAT is defined.
    #
    # We can't get here information about the spdlog compile definitions,
    # so we need to define them manually. Normally, we would just use the
    # following snippet in every source file, but it is not what we want.
    #
    #
    # ifdef SPDLOG_USE_STD_FORMAT
    # include <format>
    # else
    # include <fmt/format.h>
    # endif

    # vcpkg delivers a version of spdlog that does not use std::format.
    # Surely it is to support older compilers, but ATM we are using C++23.
    # Know this we include fmt/format.h to get rid of the false positives.
    target_compile_definitions(${target} PRIVATE MANUAL_SPDLOG_USE_STD_FORMAT_REFER_TO_CMAKE_LISTS=0)

    # Add forced include to get rid of the false positives
    target_compile_options(${target} PRIVATE /FI${APP_INCLUDE_DIR}/spdlog_impl.hxx) # no other way to avoid false positives

    #
    # Note: Possibly we should build the spdlog library from
    # source to set the compile definitions as we need them.
    #
    # In case we will get the errors as consequence of
    # what we did, we must consider building the spdlog library
    #
endfunction()

#
# JSON for Modern C++
#
# https://github.com/nlohmann/json
#
# The MIT License (MIT)
#
function(use_nlohmann_json_package target)
    find_package(nlohmann_json CONFIG REQUIRED)
    target_link_libraries(${target} PRIVATE nlohmann_json::nlohmann_json)
endfunction()