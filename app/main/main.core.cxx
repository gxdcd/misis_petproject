module;

#pragma region Header
#include <glfw_impl.hxx>
module main:core;
import :concepts;
import :consts;
import :core.imgui;
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
    void glfw_core(auto* window)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // Feed inputs to dear imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        imgui_core(static_cast<View*>(glfwGetWindowUserPointer(window)));

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers to display the rendered frame
        glfwSwapBuffers(window);
    }

    template<typename T>
        requires app::creates_view<T>
    auto create_view_in_context(T create_view)
    {
        // We create the view instance in the valid context
        // which can be initialized by NewFrame() call.
        // This guarantees that the view and widgets could
        // access ImGui context right from their constuctors.
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        auto end_frame = utils::make_finalizer(ImGui::Render);
        return create_view();
    }

    template<typename T>
        requires app::creates_view<T>
    void glfw_main(const auto& window, T create_view)
    {
        using View = decltype(create_view());
        logging::info("Initializing main view...");

        auto view = create_view_in_context(create_view);

        glfwSetWindowUserPointer(window.get(), &view);

        // Set callback to handle window resize event
        // and set viewport accordingly to the new size
        glfwSetFramebufferSizeCallback(window.get(), [](auto* w, int /*width*/, int /*height*/) {
            glfw_core<View>(w);
        });

        glfwSetWindowPosCallback(window.get(), [](auto* w, int /*xpos*/, int /*ypos*/) {
            glfw_core<View>(w);
        });

        // Show the window and run the main loop
        logging::info("Entering main event loop...");
        // First render to initialize the window background
        // We do it here to avoid flickering
        glfw_core<View>(window.get());
        // Now show the window, no flickering should be visible
        glfwShowWindow(window.get());
        while (!glfwWindowShouldClose(window.get())) {
            ImGui::Utils::CheckCurrentTheme([&](bool dark) {
                ImGui::Utils::SetTheme(window.get(), dark);
            });
            // Run the core rendering process
            glfw_core<View>(window.get());
            // Poll events with specified timeout to limit frame rate.
            // If one or more events are available, `glfwWaitEventsTimeout` behaves as `glfwPollEvents`.
            glfwWaitEventsTimeout(1.0 / REFRESH_RATE_FPS); // or // glfwPollEvents();
        }
    }
}
