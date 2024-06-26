cmake_minimum_required(VERSION 3.28)

function(configure_lib this_lib)
    add_lib_target(${this_lib})

    # Sources
    target_sources(${this_lib}
        PUBLIC FILE_SET ${this_lib}_lib_modules TYPE CXX_MODULES
        FILES
        view.tiles.ixx
        view.tiles.state.cxx
        view.tiles.table.cxx
        view.tiles.controls.cxx
        view.tiles.status.cxx
        view.tiles.paths.cxx
        view.tiles.pdfbox.cxx
        view.tiles.pdf.cxx
    )

    # Dependencies
    use_imgui_package(${this_lib})
    use_imgui_file_dialog(${this_lib})
    use_logging_package(${this_lib})

    target_link_libraries(${this_lib} PRIVATE imgui_ext)
    target_link_libraries(${this_lib} PRIVATE model)
    target_link_libraries(${this_lib} PRIVATE widgets)
    target_link_libraries(${this_lib} PRIVATE utilities)

    find_package(PkgConfig)
    pkg_check_modules(POPPLER_CPP REQUIRED IMPORTED_TARGET poppler-cpp)
    target_link_libraries(${this_lib} PRIVATE PkgConfig::POPPLER_CPP)
endfunction()

configure_lib(views)

#[=[

PDFium alternative

Old PDFium tutorial (8 years)
https://github.com/documentcloud/pdfium
https://github.com/documentcloud/pdfium/blob/master/README.md

### Recent manuals ####################################################################################

PDFium  (use this)
https://pdfium.googlesource.com/pdfium/
Building PDFium (don't use it, outdated)
https://pdfium.googlesource.com/pdfium/+/80aa8c26887d98f654a9eecf7298663656cfc40f/README.md
Checking out and Building Chromium for Windows (also needed)
https://chromium.googlesource.com/chromium/src/+/HEAD/docs/windows_build_instructions.md

### Install depot_tools ####################################################################################

https://www.chromium.org/developers/how-tos/install-depot-tools/
https://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_setting_up

WINDOWS
Download the depot_tools bundle and extract it somewhere.
https://storage.googleapis.com/chrome-infra/depot_tools.zip

From a cmd.exe shell, run the command gclient (without arguments).

Note
If you run gclient from a non-cmd shell (e.g., cygwin, PowerShell), it may appear to run properly, but msysgit, python, and other tools may not get installed correctly.

From https://chromium.googlesource.com/chromium/src/+/HEAD/docs/windows_build_instructions.md:

Also, add a DEPOT_TOOLS_WIN_TOOLCHAIN environment variable in the same way, and set it to 0. This tells depot_tools to use your locally installed version of Visual Studio (by default, depot_tools will try to use a google-internal version).

### Get the code ####################################################################################

mkdir pdfium
cd pdfium
gclient config --unmanaged https://pdfium.googlesource.com/pdfium.git
gclient sync

Estimated time:
gclient config - instant
gclient sync - about 20 mins for Syncing projects first time

cd pdfium
gn gen out\Default
# Done. Made 652 targets from 186 files in 8752ms
ninja -C out/Default pdfium_test
ninja -C out/Default pdfium_all

]=]