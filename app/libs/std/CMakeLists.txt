function(configure_std_lib_old_way this_lib)
    file(GENERATE OUTPUT std.cxx CONTENT "")
    add_library(${this_lib} STATIC EXCLUDE_FROM_ALL std.cxx)
    put_target_to_libs_folder(${this_lib})
    add_clean_target(${this_lib})
    target_include_directories(${this_lib} PRIVATE "${APP_INCLUDE_DIR}")
    target_precompile_headers(${this_lib} PRIVATE "${APP_INCLUDE_DIR}/std.hxx")
endfunction()

# Create precompiled header target to reuse
function(configure_std_lib this_lib)
    add_library(${this_lib} STATIC EXCLUDE_FROM_ALL)

    put_target_to_libs_folder(${this_lib})
    add_clean_target(${this_lib})

    target_include_directories(${this_lib} PUBLIC "${APP_INCLUDE_DIR}")
    target_precompile_headers(${this_lib} PUBLIC "${APP_INCLUDE_DIR}/std.hxx")

    target_sources(${this_lib}
        PUBLIC FILE_SET ${this_lib}_lib_modules TYPE CXX_MODULES
        FILES
        std.cxx
        PUBLIC FILE_SET ${this_lib}_lib_headers
        TYPE HEADERS
        BASE_DIRS "${APP_INCLUDE_DIR}"
        FILES "${APP_INCLUDE_DIR}/std.hxx"
    )
endfunction()

configure_std_lib(std)

# https://gitlab.kitware.com/cmake/cmake/-/issues/24865
# The command-line generators like Ninja may need to be
# updated to put the PCH-derived force-include flag earlier.
function(reuse_precompiled_headers target)
    target_precompile_headers(${target} REUSE_FROM std)
endfunction()