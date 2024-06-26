cmake_minimum_required(VERSION 3.28)

function(configure_lib this_lib)
    add_lib_target(${this_lib})

    # Sources
    target_sources(${this_lib}
        PUBLIC FILE_SET ${this_lib}_modules TYPE CXX_MODULES
        FILES
        model.ixx
        model.sync.cxx
        model.matcher.cxx
    )

    # Dependencies
    use_logging_package(${this_lib})
    target_link_libraries(${this_lib} PRIVATE utilities)
endfunction()

configure_lib(model)