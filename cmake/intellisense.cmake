pragma_once()

# Temporary workaround for MSVC intellisense support for C++20 modules
# to avoid "could not find module file for module ... C/C++(3275)" errors
# in Visual Studio Code.
# Very possible that this workaround will be removed in the future
# because someday MSVC and VS Code will support C++20 modules properly.
#
# Usage:
# - put `support_intellisense(target)` into your CMakeLists.txt
# - configure and build the project
# - configure project again
#
# Notes:
# - This function requires MSVC compiler
# - The function will create symlinks to the .ifc files in the binary directory of the target project
# - Some time the errors will be shown in VS Code, but the project will be built successfully
# - After the cleaning the project you will possibly get the `ninja` error like
# `smth... needed by... missing and no known rule to make it` - configure project again, build and configure again
# - Also you could spot an error `could not open output file '...ifc'` - build again or regonfigure
# the project and build again - the reason why this happens is VS Code extension is holding this file open
function(support_intellisense target)
    message("Adding intellisense support fix to `${target}` target")

    if(NOT TARGET ${target})
        message(FATAL_ERROR "Cannot find target \"${target}\"!!!")
    endif()

    get_target_property(target_binary_dir ${target} BINARY_DIR)

    file(GLOB_RECURSE ifc_files
        ${target_binary_dir}/*.ifc)

    if(NOT ifc_files)
        message(STATUS "Target `${target}` has not built yet. Build this target first and reconfigure project to assist intellisense.")
        return()
    endif()

    foreach(ifc_file ${ifc_files})
        get_filename_component(ifc_directory ${ifc_file} DIRECTORY)

        if(ifc_directory STREQUAL ${target_binary_dir})
            continue()
        endif()

        # if(ifc_directory MATCHES ${target})
        #     get_filename_component(ifc_symlink_file ${ifc_file} NAME)
        #     file(CREATE_LINK ${ifc_file} ${CMAKE_BINARY_DIR}/ifc/${ifc_symlink_file} SYMBOLIC)
        # endif()
    endforeach()
endfunction()

# Create a directory for the `.ifc` symlink files and add this
# directory to the compiler search as a part of the workaround
function(prepare_intellisense)
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/ifc/")
    add_compile_options(/ifcSearchDir "${CMAKE_BINARY_DIR}/ifc/")
endfunction()

# This is an ugly hack to kill cpptools-srv.exe when the project
# is going to be rebuilt. We need this because `cpptools-srv.exe`
# locks the `.ifc` files and the build fails.
function(unlock_sources_for_build target)
    set(kill_target utility.kill.cpptools-srv)

    if(NOT TARGET ${kill_target})
        add_custom_target(
            ${kill_target}
            COMMAND cmd /c "start /B taskkill /f /im cpptools-srv.exe 2>nul"
            VERBATIM
        )
        put_target_to_utilities_folder(${kill_target})
    endif()

    add_dependencies(${target} ${kill_target})
endfunction()

function(support_intellisense_and_unlock target)
    support_intellisense(${target})
    unlock_sources_for_build(${target})
endfunction()

    # We need this to let VS Code intelisense work properly with
    # C++20 modules. This, hopefully, is a temporary workaround
function(support_intellisense_and_unlock_all)
    cmtools_get_all_targets(all_targets)

    foreach(target ${all_targets})
        get_target_property(target_type ${target} TYPE)

        if(target_type MATCHES "LIBRARY")
            # Possible types are UTILITY, STATIC_LIBRARY, MODULE_LIBRARY,
            # SHARED_LIBRARY, OBJECT_LIBRARY, INTERFACE_LIBRARY, EXECUTABLE
            support_intellisense_and_unlock(${target})
        endif()
    endforeach()
endfunction()

if(MSVC)
    prepare_intellisense()
else()
    message(FATAL_ERROR "Unsupported compiler - not sure how to set the flags")
endif()