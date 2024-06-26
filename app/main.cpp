import main;
import logging;
import seh;
#include <seh.h>

import view.tiles;
auto create_view() { return ViewTiles{}; }

int run_with_seh(int /*argc*/, char* /*argv*/[])
{
    __try {
        app::intro();
        return app::main(create_view);
    } __except (seh::exception_handler(exception_info())) {
        return exception_code();
    }
}

int run_with_logging(int argc, char* argv[])
{
    auto scope = logging_scope{true};
    return run_with_seh(argc, argv);
}

#pragma region Main
// Consider using NDEBUG instead of DEBUG, _DEBUG
// https://github.com/microsoft/qsharp-runtime/issues/600
#ifndef NDEBUG
// `main` is the standard entry function to start a C++ program
// https://en.cppreference.com/w/cpp/language/main_function
int main(int argc, char* argv[])
{
    return run_with_logging(argc, argv);
}
#else
#    include <windows.h>
// `WinMain` is the standard entry function to start a Windows program
int APIENTRY WinMain(HINSTANCE /*hCurrentInst*/, HINSTANCE /*hPreviousInst*/,
    LPSTR /*lpszCmdLine*/, int /*nCmdShow*/)
{
    // We configured application to use Windows subsystem and WinMain
    // but still want to see console output. We can attach console to
    // the parent process (which is the console) and use it for logging.
    AttachConsole(ATTACH_PARENT_PROCESS);
    // https://stackoverflow.com/questions/2346328/breaking-down-winmains-cmdline-in-old-style-mains-arguments
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/argc-argv-wargv
    return run_with_logging(__argc, __argv);
}
#endif
#pragma endregion