module;

#define COMBINE1(X, Y) X##Y
#define COMBINE(X, Y)  COMBINE1(X, Y)
#define scope_ptr      auto COMBINE(scope, __LINE__) =

#pragma region Header
#include <imgui_impl.hxx>
export module imgui;
#pragma endregion

namespace ImGui {
    namespace Utils {
        export void DisableImguiConfig()
        {
            auto& io = GetIO();
            io.IniFilename = NULL;
            io.LogFilename = NULL;
        }
    }
}