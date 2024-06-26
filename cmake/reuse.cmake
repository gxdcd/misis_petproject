
if(APP_SOURCE_DIR STREQUAL "")
    message(FATAL_ERROR "APP_SOURCE_DIR is not set")
endif()

function(use_app_include_directories target)
    set(options PRIVATE PUBLIC)
    set(multipleValuesArgs)
    set(oneValueArgs)
    cmake_parse_arguments(PARSE_ARGV 1
        arg "${options}" "${oneValueArgs}" "${multipleValuesArgs}")

    if(arg_PUBLIC)
        set(visibility PUBLIC)
    else()
        set(visibility PRIVATE)
    endif()

    target_include_directories(${target} ${visibility} ${APP_INCLUDE_DIRS})
endfunction()

function(reuse_include_directories source target visibility)
    get_target_property(source_include_dirs ${source} INCLUDE_DIRECTORIES)

    if(NOT source_include_dirs MATCHES "NOTFOUND")
        target_include_directories(${target} ${visibility} ${source_include_dirs})
    endif()
endfunction()