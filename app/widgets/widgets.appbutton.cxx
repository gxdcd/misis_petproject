module;

#pragma region Header
#include <imgui_impl.hxx>
export module widgets:appbutton;
import :textures;
using namespace std;
using namespace ImGui;
#pragma endregion

namespace widgets {
    export struct AppButton {

        AppButton() = default;
        AppButton(optional<ImVec2> default_button_size):
            default_button_size{default_button_size}
        {
        }

        template<typename DisplayPopupContent>
            requires invocable<DisplayPopupContent, optional<ImVec2>>
        void display(DisplayPopupContent display_content, optional<ImVec2> button_size, ImVec2 fit_size) const
        {
            auto& style = GetStyle();

            auto save_pos = GetCursorPos();
            auto vp_size = GetMainViewport()->Size;

            auto size = fit_size;
            size.x -= style.FramePadding.x * 2.0f + style.ItemSpacing.x * 2.0f;
            size.y = size.x;

            vp_size.x -= size.x + style.FramePadding.x * 2.0f + style.ItemSpacing.x;
            vp_size.y -= size.y + style.FramePadding.y * 2.0f + style.ItemSpacing.x;
            SetCursorPos(vp_size);

            SetNextItemAllowOverlap();

            if (ImageButton("##Options", button.get_texture_id(), size))
                OpenPopup("my_select_popup");

            if (BeginPopup("my_select_popup")) {
                display_content(button_size);
                EndPopup();
            }

            SetCursorPos(save_pos);
        }

        template<typename DisplayPopupContent>
            requires invocable<DisplayPopupContent, optional<ImVec2>>
        void display(DisplayPopupContent display_content, ImVec2 fit_size) const
        {
            display(display_content, default_button_size, fit_size);
        }

    private:
        optional<ImVec2> default_button_size;
        ThemedTexture button{"gear.png", 0.25f};
    };
}
