cmake_minimum_required(VERSION 3.28)

function(configure_lib this_lib)
    add_lib_target(${this_lib})

    # Sources
    target_sources(${this_lib}
        PUBLIC FILE_SET ${this_lib}_modules TYPE CXX_MODULES
        FILES
        widgets.ixx
        widgets.busy.cxx
        widgets.msgbox.cxx
        widgets.appbutton.cxx
        widgets.symbuttons.cxx
        widgets.styles.cxx
        widgets.textures.cxx
    )

    # Dependencies
    use_imgui_package(${this_lib})
    use_stb_image(${this_lib})
    use_glad_package(${this_lib})
    target_link_libraries(${this_lib} PRIVATE imgui_ext)
endfunction()

configure_lib(widgets)