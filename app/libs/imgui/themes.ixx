module;

#pragma region Header
#include <glfw_impl.hxx>
#include <imgui_impl.hxx>
#include <dwmapi.h>
#pragma comment(lib, "Dwmapi")
export module themes;
using namespace std;
using namespace ImGui;
#pragma endregion

namespace ImGui {
    export namespace Utils {

        // Initializes ImGui with the given theme and
        // sets the clear color to the window background color
        void SetTheme(bool dark);
        // Initializes ImGui with the given theme and
        // sets the clear color to the window background color.
        // Also sets the window style to match the ImGui theme
        void SetTheme(GLFWwindow* window, bool dark);
        // Checks if the active ImGui theme is dark
        bool IsThemeDark();
        // Sets the window style to match the ImGui theme
        void SetWindowTheme(GLFWwindow* window);

        template<typename Callback>
            requires invocable<Callback, bool>
        void CheckCurrentTheme(Callback on_change)
        {
            static optional<bool> last_is_dark = nullopt;
            auto is_dark_theme = IsThemeDark();
            if (last_is_dark != is_dark_theme) {
                if (last_is_dark.has_value())
                    on_change(is_dark_theme);
                last_is_dark = is_dark_theme;
            }
        }
    }
}

module :private;

namespace ImGui::Utils {

    void set_clear_color(ImColor clr)
    {
        glClearColor(clr.Value.x, clr.Value.y, clr.Value.z, clr.Value.w);
    }

    float rel_brightness(float r, float g, float b)
    {
        return (5.0f * g) + (2.0f * r) + b;
    };

    bool is_rgb_dark(float r, float g, float b)
    {
        return rel_brightness(r * 255.0f, g * 255.0f, b * 255.0f) < 8.0f * 128.0f;
    }

    void SetTheme(bool dark)
    {
        auto& style = GetStyle();
        dark ? StyleColorsDark() : StyleColorsLight();
        set_clear_color(style.Colors[ImGuiCol_WindowBg]);
    }

    bool IsThemeDark(ImGuiStyle& style)
    {
        auto current_color = style.Colors[ImGuiCol_Text];
        return !is_rgb_dark(current_color.x, current_color.y, current_color.z);
    }

    bool IsThemeDark()
    {
        return IsThemeDark(GetStyle());
    }

    void SetWindowTheme(GLFWwindow* window)
    {
        auto& style = GetStyle();
        auto bg_color = style.Colors[ImGuiCol_WindowBg];
        auto dark = IsThemeDark(style);
        auto native_handle = glfwGetWin32Window(window);
        auto brush = CreateSolidBrush(RGB(bg_color.x * 255, bg_color.y * 255, bg_color.z * 255));
        auto current = (HBRUSH)GetClassLongPtr(native_handle, GCLP_HBRBACKGROUND);
        SetClassLongPtr(native_handle, GCLP_HBRBACKGROUND, (LONG_PTR)brush);
        if (current)
            DeleteObject(current);
        BOOL value = dark ? TRUE : FALSE;
        DwmSetWindowAttribute(native_handle, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));
    }

    void SetTheme(GLFWwindow* window, bool dark)
    {
        SetTheme(dark);
        SetWindowTheme(window);
    }
}