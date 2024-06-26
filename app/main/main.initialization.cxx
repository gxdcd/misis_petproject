module;

#pragma region Header
#include <glfw_impl.hxx>
module main:initialization;
import :consts;
import :concepts;
import os;
import config;
import themes;
import logging;
using namespace std;
#pragma endregion

namespace details {

    auto create_glfw_window(auto config, const char* title = WINDOW_TITLE)
    {
        // Set window parameters
        glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
        glfwWindowHint(GLFW_MAXIMIZED, config.window_maximized ? GL_TRUE : GL_FALSE);
        // this thing also is not absolutely necessary,
        // let us immediately switch between windows
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, GL_TRUE);
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

        // Create window with graphics context
        auto window = glfwCreateWindow(
            config.window_width, config.window_height, title, nullptr, nullptr);

        if (window != nullptr) {
            // Set window position (size was set in `glfwCreateWindow` call)
            // Usng set_window_placement instead of glfwSetWindowPos to be
            // able to restore normal window position if it was maximized
            os::set_window_placement(
                glfwGetWin32Window(window), config.window_left, config.window_top,
                config.window_width, config.window_height, config.window_maximized, false);

            // Set minimum window size, don't care about maximum size
            glfwSetWindowSizeLimits(window, 450, 300, GLFW_DONT_CARE, GLFW_DONT_CARE);

            // Make the window's context current
            glfwMakeContextCurrent(window);
            // Enable vsync. Calling this function without a current
            // context will cause a GLFW_NO_CURRENT_CONTEXT error
            glfwSwapInterval(1);
        };

        return unique_ptr<GLFWwindow, decltype([](auto* window) {
            // Cleanup GLFW context and destroy window
            if (glfwGetCurrentContext() == window)
                glfwMakeContextCurrent(nullptr);
            glfwDestroyWindow(window);
            logging::info("GLFW window destroyed");
        })>{window};
    }

    auto initialize_glfw()
    {
        static auto glfw_initialized = glfwInit() == GL_TRUE;
        return unique_ptr<bool, decltype([](auto) {
            glfwTerminate();
            logging::info("GLFW terminated");
        })>{glfw_initialized ? &glfw_initialized : nullptr};
    }

    auto create_imgui_context()
    {
        // This thing is to assert ImGui version compatibility.
        // It uses IM_ASSERT which expands to `assert` in debug mode.
        IMGUI_CHECKVERSION(); // Debug only code
        // Create ImGui context. Initilized context
        // must be destroyed with `ImGui::DestroyContext()`.
        // Here we can pass `ImFontAtlas *shared_font_atlas` as a parameter
        return unique_ptr<ImGuiContext, decltype([](auto) {
            // Destroy ImGui context
            ImGui::DestroyContext();
            logging::info("ImGui context destroyed");
        })>{ImGui::CreateContext()};
    }

    auto initialize_imgui_bindings(const auto& window)
    {
        static auto initialized{true};
        // Setup Platform/Renderer bindings
        initialized &= ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
        // The 'glsl_version' initialization parameter should
        // be nullptr (default) or a "#version XXX" string.
        initialized &= ImGui_ImplOpenGL3_Init(nullptr);
        return unique_ptr<bool, decltype([](auto) {
            // Cleanup ImGui
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            logging::info("ImGui shutdown");
        })>{initialized ? &initialized : nullptr};
    }

}