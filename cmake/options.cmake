pragma_once()

# We want to use Unicode interface to the Windows API's
add_compile_definitions(UNICODE _UNICODE)

# Prevent confusion min/max macros vs std::min/std::max
add_compile_definitions(NOMINMAX)

# Exclude rarely used Windows API's
# https://stackoverflow.com/questions/11040133/what-does-defining-win32-lean-and-mean-exclude-exactly
add_compile_definitions(WIN32_LEAN_AND_MEAN)

include_directories(${APP_INCLUDE_DIRS})
set(force_app_include utility_macros.h author_warning.h)

# https://learn.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically?view=msvc-170
# https://learn.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-by-category?view=msvc-170
if(MSVC)
    add_compile_options(/utf-8) # https://github.com/ocornut/imgui/issues/4866#issuecomment-1006539446
    add_compile_options(/MP) # multi threaded
    add_compile_options(/W4) # more strict warnings
    add_compile_options(/w44800) # warn about integral to bool conversion

    # Assist SEH error output
    add_link_options(/DEBUG) # Generate Debug Info

    if(${DEBUG_BUILD})
        add_compile_options(/Zi) # Program Database (/Zi)
    endif()

    foreach(header ${force_app_include})
        add_compile_options(/FI${APP_INCLUDE_DIR}/${header})
    endforeach()
else()
    message(FATAL_ERROR "Unsupported compiler - not sure how to set the flags")
endif()