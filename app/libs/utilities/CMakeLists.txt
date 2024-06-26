cmake_minimum_required(VERSION 3.28)

function(configure_lib this_lib)
    add_lib_target(${this_lib})

    # Sources
    target_sources(${this_lib}
        PUBLIC FILE_SET ${this_lib}_modules TYPE CXX_MODULES
        FILES

        # uses 3rd party
        logging.ixx # uses spdlog
        config.ixx # uses ImGui
        fonts.ixx # uses ImGui

        # std
        stopwatch.ixx
        threadloop.ixx
        utils.ixx

        # std/os
        os.ixx
        win.ixx
        seh.ixx
        utf.ixx

        PUBLIC FILE_SET ${this_lib}_lib_headers TYPE HEADERS
        FILES
        seh.h
    )

    # Dependencies
    use_glfw3_package(${this_lib})
    use_imgui_package(${this_lib})
    use_nlohmann_json_package(${this_lib})
    use_logging_package(${this_lib})
endfunction()

configure_lib(utilities)