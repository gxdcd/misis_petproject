#
# Configure files, assets, and resources
#

if(APP_ASSETS_DIR STREQUAL "")
    message(FATAL_ERROR "APP_ASSETS_DIR is not set")
endif()

function(copy_asset_file filename)
    # We will use fontawesome for the icons in the GUI
    # https://github.com/beakerbrowser/beakerbrowser.com/blob/master/fonts/fontawesome-webfont.ttf
    configure_file(
        "${APP_ASSETS_DIR}/${filename}"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fontawesome-webfont.ttf"
        COPYONLY)
endfunction()

function(copy_asset_files folder glob_expression)
    set(glob_dir "${APP_ASSETS_DIR}/${folder}")
    file(GLOB_RECURSE globbed_files "${glob_dir}/${glob_expression}")

    foreach(file ${globbed_files})
        get_filename_component(filename ${file} NAME)
        file(RELATIVE_PATH sub_dir "${glob_dir}" "${file}")
        get_filename_component(target_dir ${sub_dir} DIRECTORY)
        configure_file(
            ${file}
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target_dir}/${filename}"
            COPYONLY)
    endforeach()
endfunction()