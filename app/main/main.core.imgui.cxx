module;

#pragma region Header
#include <glfw_impl.hxx>
module main:core.imgui;
import :concepts;
import :consts;
import config;
import logging;
import themes;
import utils;
using namespace std;
using namespace ImGui;
#pragma endregion

namespace details {

    template<typename View>
        requires app::is_view<View>
    void imgui_core(View* view)
    {
        // Start new ImGui frame (necessary for all ImGui widgets)
        NewFrame();

        // Apply some style changes to the window
        // See `PopStyleVar` below to restore the default style
        // It's necessary to call `PushStyleVar` and `PopStyleVar` in pairs
        PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f); // rounded corners for controls
        PushStyleVar(ImGuiStyleVar_GrabRounding, 4.0f);

        if (app::get_config().show_demo_window) {
            PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f); // rounded corners for windows
            PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
            // Create and display Demo window to demonstrate ImGui features.
            // Call this to learn about the library!
            // Try to make it always available in your application!
            ShowDemoWindow();
            PopStyleVar(2);
        }

        PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        // The option to use ImGui viewport functions
        // `GetMainViewport` is not a costly function,
        // so we actually can call it every frame
        auto viewport = GetMainViewport();
        SetNextWindowPos(viewport->WorkPos);
        SetNextWindowSize(viewport->WorkSize);

        Begin(WINDOW_TITLE, nullptr, FULL_SIZE_WINDOW_FLAGS);
        // we want rounded corners for windows created later
        PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
        PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

        view->display();

        PopStyleVar(2);
        End();

        // Restore default style - 2 styles were pushed above
        // so we need to call `PopStyleVar` twice
        PopStyleVar(4);

        // Render ImGui
        Render();
    }

}
