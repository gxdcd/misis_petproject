cmake_minimum_required(VERSION 3.28)

function(configure_lib this_lib)
    add_lib_target(${this_lib})

    # Sources
    target_sources(${this_lib}
        PUBLIC FILE_SET ${this_lib}_lib_modules TYPE CXX_MODULES
        FILES
        themes.ixx
        imgui.ixx
    )

    # Dependencies
    use_glfw3_package(${this_lib})
    use_imgui_package(${this_lib})
endfunction()

configure_lib(imgui_ext)