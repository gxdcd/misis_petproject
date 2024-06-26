cmake_minimum_required(VERSION 3.28)

if(NOT DEBUG_BUILD)
    set(subsystem WIN32)
endif()

# In Debug mode, we want to see the console window
# WIN32 does not create the console window in Release mode
add_executable(${this_project} ${subsystem} EXCLUDE_FROM_ALL "main.cpp")

# Add suffix according to build type (may have version too)
adjust_build_type_output_name(${this_project})

# Misc
put_target_to_exe_folder(${this_project})
add_clean_target(${this_project})

# Configure package dependencies and external libs (submodules)
use_logging_package(${this_project}) # this must be `global` (why?)
use_imgui_file_dialog() # this should be `global` (target added as a subdirectory)

# Add subdirectories
foreach(subdirectory libs;main;model;widgets;views)
    add_subdirectory(${subdirectory})
endforeach()

# Add precompiled headers to the executable so that these
# headers including STL were available in every source file
reuse_precompiled_headers(${this_project})

# Link libraries directly used (or maybe used) by the executable
foreach(lib utilities;main;views)
    target_link_libraries(${this_project} PUBLIC ${lib})
endforeach()

#
# Configure files, assets, and resources (could use `install` also)
#

# We use fontawesome for some icons in the GUI
# https://github.com/beakerbrowser/beakerbrowser.com/blob/master/fonts/fontawesome-webfont.ttf
copy_asset_file(fontawesome-webfont.ttf)

# https://feathericons.com/
# https://pictogrammers.com/library/mdi/
# https://pictogrammers.com/docs/general/license/
copy_asset_files(icons *.png)