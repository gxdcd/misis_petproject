if(APP_INCLUDE_DIR STREQUAL "")
    message(FATAL_ERROR "APP_INCLUDE_DIR is not set")
endif()

#
# Target folders
#
function(put_target_to_exe_folder target)
    set_property(TARGET ${target} PROPERTY FOLDER "Exe")
endfunction()

function(put_target_to_extern_libs_folder target)
    set_property(TARGET ${target} PROPERTY FOLDER "Deps")
endfunction()

function(put_target_to_libs_folder target)
    set_property(TARGET ${target} PROPERTY FOLDER "Libs")
endfunction()

function(put_target_to_tests_folder target)
    set_property(TARGET ${target} PROPERTY FOLDER "Tests")
endfunction()

function(put_target_to_utilities_folder target)
    set_property(TARGET ${target} PROPERTY FOLDER "Utilities")
endfunction()

#
# Exe targets
#

# Add a executable target
# Args:
#   target: The name of the target
# Options:
#   ALL: Include the test executable in global ALL target (opposite of EXCLUDE_FROM_ALL)
#   WIN32: Use WIN32 subsystem
#   REUSE_PCH: Reuse precompiled headers defined by the project (this possibly will break forced includes)
#   ADD_CLEAN: Add a clean target for the test
function(add_exe_target target)
    set(options ALL WIN32 REUSE_PCH ADD_CLEAN)
    set(oneValueArgs FOLDER)
    set(multipleValuesArgs SOURCES LIBS)
    cmake_parse_arguments(PARSE_ARGV 1
        arg "${options}" "${oneValueArgs}" "${multipleValuesArgs}")

    if(NOT arg_ALL)
        set(options EXCLUDE_FROM_ALL)
    endif()

    if(arg_WIN32)
        set(subsystem WIN32)
    endif()

    add_executable(${target} ${subsystem} ${options} ${arg_SOURCES})

    if(arg_REUSE_PCH)
        reuse_precompiled_headers(${target})
    endif()

    if(arg_FOLDER)
        set_property(TARGET ${target} PROPERTY FOLDER ${arg_FOLDER})
    endif()

    if(arg_ADD_CLEAN)
        add_clean_target(${target})
    endif()

    if(arg_LIBS)
        target_link_libraries(${target} PRIVATE ${arg_LIBS})
    endif()
endfunction()

#
# Clean targets
#

# https://discourse.cmake.org/t/ability-to-clean-individual-targets/2980/19
function(add_clean_target target)
    set(clean_target clean.${target})
    set(cleanup_script ${target}.clean.bat)

    # CAUTION: this command sequence provides the means
    # to clean the target without rebuilding it.
    # If you try to refactor this code, please, consider
    # the possibility of breaking this logic.
    #
    # For instance, it seems like a simple solution to
    # just add a custom target that cleans the target
    # TARGET_FILE:${target} and TARGET_OBJECTS:${target}.
    # It will work once, but next time you try to use this
    # clean target, it will first build the target and only
    # then clean it. The following implementation avoids
    # this by splitting this process into two steps.

    set(target_output "$<SHELL_PATH:$<TARGET_FILE:${target}>>")
    set(target_objects "$<JOIN:$<SHELL_PATH:$<TARGET_OBJECTS:${target}>>,\" \">")
    set(delete "del /Q /S")
    set(command "${delete} \"${target_output}\" \"${target_objects}\"")

    add_custom_command(
        TARGET ${target}
        POST_BUILD
        COMMENT "Writing `${target}` cleanup script"
        COMMAND cmd /c > ${cleanup_script} echo ${command}
        DEPENDS_EXPLICIT_ONLY
        COMMAND_EXPAND_LISTS
    )

    set(error "Cleanup script ${cleanup_script} not found. Try to [re]build `${target}` target")
    set(command "echo off & if exist ${cleanup_script} (call ${cleanup_script}) else (echo ${error})")
    add_custom_target(${clean_target}
        COMMENT "Cleaning target `${target}`"
        COMMAND cmd /c ${command}
    )

    put_target_to_utilities_folder(${clean_target})
endfunction()

function(add_lib_target target)
    add_library(${target} STATIC EXCLUDE_FROM_ALL)
    reuse_precompiled_headers(${target})
    add_clean_target(${target})
    put_target_to_libs_folder(${target})
endfunction()