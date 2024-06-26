module;

#pragma region Header
#include <glfw_impl.hxx>
module main:start;
import :core;
import :initialization;
import :concepts;
import os;
import imgui;
import fonts;
using namespace std;
#pragma endregion

// This block is about initialization
namespace details {

    template<typename T>
        requires app::creates_view<T>
    expected<int, string> start(T create_view)
    {
        auto glfw = initialize_glfw();
        if (!glfw)
            return unexpected("Failed to initialize GLFW");
        logging::info("GLFW initialized");

        auto config = app::load_config(
            os::get_current_exe_folder());

        // Create window with graphics context
        auto window = create_glfw_window(config);
        if (!window)
            return unexpected("Failed to create GLFW window");
        logging::info("GLFW window created");

        // Bind window context to glad's opengl funcs, this can be
        // done only after the window is created and the context is set
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            return unexpected("Unable to bind context to OpenGL");

        // Initialize and setup ImGui

        logging::info("Initializing ImGui...");

        auto imgui_context = create_imgui_context();
        if (!imgui_context)
            return unexpected("Failed to create ImGui context");

        auto imgui_bindings = initialize_imgui_bindings(window);
        if (!imgui_bindings)
            return unexpected("Failed to initialize ImGui render bindings");

        // Disable saving `imgui.ini` and log files
        ImGui::Utils::DisableImguiConfig();

        // Initialize ImGui fonts
        utils::ImGui::initialize_fonts(config);

        // Set ImGui theme based on configuration
        ImGui::Utils::SetTheme(window.get(), config.dark_theme);

        glfw_main(window, create_view);

        app::update_config(
            [&](int& left, int& top, int& width, int& height, bool& maximized) {
                os::get_window_placement(
                    glfwGetWin32Window(window.get()),
                    left, top, width, height, maximized);
            },
            ImGui::Utils::IsThemeDark());

        return {};
    }
}
