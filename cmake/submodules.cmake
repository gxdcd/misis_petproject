#
# Configure external libs (submodules)
#

#
# Update all submodules in a Git repo
# git submodule update --init
#
# List all submodules in a Git repo
# https://snippets.khromov.se/list-all-submodules-in-a-git-repo/
# git config --file .gitmodules --name-only --get-regexp path
#
# How effectively delete a git submodule.
# https://gist.github.com/myusuf3/7f645819ded92bda6677
#

#
# ImGuiFileDialog - peek the fonts (we don't use it for now, although it's a good idea)
#
# https://stackoverflow.com/questions/20985983/git-add-only-some-files-from-remote-repository-as-submodule
# https://stackoverflow.com/questions/1911109/how-do-i-clone-a-specific-git-branch
# https://www.silverdreamracer.me/git-checkout-only-specific-folder.html
# git clone --no-checkout -b DemoApp --single-branch --depth=1 https://github.com/aiekick/ImGuiFileDialog.git app/libs/ImGuiFileDialog-Fonts
# git sparse-checkout set CustomFont.* Roboto_Medium.*
# git reset --hard HEAD

#
# ImGuiFileDialog
#
# The MIT License (MIT)
#
# ImGuiFileDialog is a file selection dialog built for (and using only) Dear ImGui.
# https://github.com/aiekick/ImGuiFileDialog
#
# Add ImGuiFileDialog as a submodule
# git submodule add --depth 1 -b master https://github.com/aiekick/ImGuiFileDialog.git app/libs/ImGuiFileDialog
# https://stackoverflow.com/questions/16773642/pull-git-submodules-after-cloning-project-from-github
#
function(use_imgui_file_dialog)
    set(target ${ARGN})

    if(NOT TARGET ImGuiFileDialog)
        set(filedialog_lib ImGuiFileDialog)
        set(filedialog_repo "https://github.com/aiekick/ImGuiFileDialog.git")
        set(app_libs_dir deps)
        set(app_libs_path "${APP_SOURCE_DIR}/${app_libs_dir}")
        set(filedialog_lib_path "${app_libs_path}/${filedialog_lib}")

        if(NOT EXISTS "${filedialog_lib_path}")
            # Study ExternalProject_Add approach described here:
            # https://github.com/doctest/doctest/blob/master/doc/markdown/build-systems.md
            message(STATUS "Library `${filedialog_lib}` not found, cloning...")
            find_package(Git REQUIRED)
            file(MAKE_DIRECTORY "${app_libs_path}") # necessary to use as WORKING_DIRECTORY
            execute_process(
                COMMAND ${GIT_EXECUTABLE} clone --depth=1 ${filedialog_repo} ${filedialog_lib}
                WORKING_DIRECTORY "${app_libs_path}"
                RESULT_VARIABLE GIT_CLONE_RESULT
            )

            if(GIT_CLONE_RESULT)
                message(FATAL_ERROR "Failed to clone `${filedialog_lib}` library")
            endif()

            file(MAKE_DIRECTORY "${filedialog_lib_path}") # necessary to update status
        endif()

        add_subdirectory("${filedialog_lib_path}")

        if(POLICY CMP0079)
            # This is required to allow us to link ImGuiFileDialog
            # to `imgui` from outside it's project directory
            cmake_policy(SET CMP0079 NEW)
        endif()

        use_imgui_package(ImGuiFileDialog)
        put_target_to_extern_libs_folder(ImGuiFileDialog)

        # warning C5054: operator '|': deprecated between enumerations of different types
        # warning C4100: unreferenced formal parameter
        target_compile_options(ImGuiFileDialog PRIVATE /wd5054 /wd4100)

        # Instead of editing the ImGuiFileDialog source code, we can
        # define some preprocessor macros which supposed to be
        # defined in the ImGuiFileDialogConfig.h or it's equivalent.
        target_compile_definitions(
            ImGuiFileDialog
            PUBLIC # THIS THING HERE MUST BE PUBLIC!!! OTHER WAY DIALOG CRASHES!!!
            USE_STD_FILESYSTEM
            USE_EXPLORATION_BY_KEYS=1
            IGFD_KEY_UP=ImGuiKey_UpArrow
            IGFD_KEY_DOWN=ImGuiKey_DownArrow
            IGFD_KEY_ENTER=ImGuiKey_Enter
            IGFD_KEY_BACKSPACE=ImGuiKey_Backspace
            USE_DIALOG_EXIT_WITH_KEY=1
            IGFD_EXIT_KEY=ImGuiKey_Escape
            USE_PLACES_FEATURE=1
            PLACES_PANE_DEFAULT_SHOWN=1
            USE_PLACES_DEVICES=1
            PLACES_DEVICES_DEFAULT_OPEPEND=1
        )
    endif()

    if(TARGET ${target})
        target_link_libraries(${target} PRIVATE ImGuiFileDialog)
    endif()
endfunction()

function(use_stb_image target)
    if(NOT TARGET ImGuiFileDialog)
        message(FATAL_ERROR "ImGuiFileDialog is required to "
            "use stb_image in this configuration but is not found")
    else()
        reuse_include_directories(ImGuiFileDialog ${target} PRIVATE)
    endif()
endfunction()