#
# CMakehelpers for different configuration tasks
#

macro(pragma_once)
    get_filename_component(list_file ${CMAKE_CURRENT_LIST_FILE} NAME_WE)

    if(__${list_file}_MODULE_INCLUDED)
        get_filename_component(list_file ${CMAKE_CURRENT_LIST_FILE} NAME)
        # message WARNING gives a call stack trace
        message(TRACE "pragma_once: Skipping including \"${list_file}\" ${CMAKE_CURRENT_LIST_FILE}")
        return()
    endif()

    set(__${list_file}_MODULE_INCLUDED TRUE)
endmacro()

function(adjust_build_type_output_name target)
    set_target_properties(${target}
        PROPERTIES
        OUTPUT_NAME "${target}"
        OUTPUT_NAME_DEBUG "${target}.d"
    )
endfunction()

macro(set_preferable_output_dirs)
    # Set the output directories
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endmacro()

macro(cmtools_get_directory_properties_recursive properties dir property_name)
    get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)

    foreach(subdir ${subdirectories})
        cmtools_get_directory_properties_recursive(${properties} ${subdir} ${property_name})
    endforeach()

    get_property(current_properties DIRECTORY ${dir} PROPERTY ${property_name})
    list(APPEND ${properties} ${current_properties})
endmacro()

function(cmtools_get_all_targets var)
    set(targets)
    cmtools_get_directory_properties_recursive(targets ${CMAKE_SOURCE_DIR} BUILDSYSTEM_TARGETS)
    set(${var} ${targets} PARENT_SCOPE)
endfunction()