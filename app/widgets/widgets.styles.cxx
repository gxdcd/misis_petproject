module;

#pragma region Header
#include <imgui_impl.hxx>
export module widgets:styles;
using namespace ImGui;
using namespace std;
#pragma endregion

namespace widgets::styles {
    export struct ButtonStyle {
        ButtonStyle():
            ButtonStyle(nullopt) { }

        ButtonStyle(optional<ImVec4> col, float frame_border_size = 1.0f)
        {
            auto& style = GetStyle();

            border_size = style.FrameBorderSize;
            style.FrameBorderSize = frame_border_size;

            PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_WindowBg]);
            auto col_hover = col.value_or(style.Colors[ImGuiCol_WindowBg]);
            PushStyleColor(ImGuiCol_ButtonActive, col_hover);
            PushStyleColor(ImGuiCol_ButtonHovered, col_hover); // col_hover
        }

        ButtonStyle(ImGuiCol col, float frame_border_size = 1.0f):
            ButtonStyle(GetStyle().Colors[col], frame_border_size) { }

        ~ButtonStyle()
        {
            auto& style = GetStyle();
            style.FrameBorderSize = border_size;
            PopStyleColor(3);
        }

    private:
        float border_size;
    };
}