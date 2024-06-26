module;

#pragma region Header
#include <imgui_impl.hxx>
#include <glfw_impl.hxx>
module view.tiles:controls;
import :state;
import themes;
import themes;
import widgets;
using namespace ImGui;
using namespace std;
using namespace widgets;
#pragma endregion

struct Controls {
    Controls() = default;

    template<typename C1, typename C2, typename C3, typename C4>
        requires invocable<C1> && invocable<C2> && invocable<C3> && invocable<C4>
    void display(
        bool center,
        optional<ImVec2> requested_button_size,
        const ViewTilesState& state,
        bool search_paths_empty,
        C1 start_scan, C2 stop_scan, C3 clear_all, C4 configure) const
    {
        auto button_size = requested_button_size.value_or(default_button_size);

        auto button_styles = styles::ButtonStyle(
            Utils::IsThemeDark()
                ? nullopt
                : optional<ImVec4>{GetStyle().Colors[ImGuiCol_ModalWindowDimBg]});

        auto viewport_size = GetMainViewport()->Size;
        auto size = get_size(button_size);
        if (size.x > viewport_size.x * 0.75f) {
            auto coeff = viewport_size.x * 0.75f / size.x;
            button_size.x *= coeff;
            button_size.y *= coeff;
        }

        set_pos(center, button_size);

        if (image_button("play", button_size, play_button,
                state.scanning || !state.empty || search_paths_empty, false))
            start_scan();

        if (image_button("stop", button_size, stop_button, !state.scanning))
            stop_scan();

        if (image_button("clear", button_size, clear_button, state.scanning ? true : state.empty))
            clear_all();

        if (image_button("wrench", button_size, wrench_button, false)) {
            configure();
        }
    }

    bool image_button(const char* label, ImVec2 button_size, const ThemedTexture& texture, bool disabled, bool same_line = true) const;
    ImVec2 get_size(ImVec2 button_size) const;
    void set_pos(bool center, ImVec2 button_size) const;

private:
    // https://pictogrammers.com/library/mdi/
    ThemedTexture play_button{"play.png", 0.5f};
    ThemedTexture stop_button{"stop.png", 0.5f};
    ThemedTexture clear_button{"table-large-remove.png", 0.5f};
    ThemedTexture wrench_button{"folder-wrench.png", 0.5f};
    const int button_count = 4;
    const ImVec2 default_button_size{128, 128};
};

void Controls::set_pos(bool center, ImVec2 button_size) const
{
    if (center) {
        auto& style = GetStyle();
        auto cursor_pos = GetCursorPos();
        auto* v = GetMainViewport();
        auto sz = v->Size;
        auto size = get_size(button_size);
        auto pos = center
                       ? ImVec2{sz.x / 2 - size.x / 2, sz.y / 2 - size.y / 2}
                       : ImVec2{sz.x / 2 - size.x / 2, sz.y - size.y - style.ItemSpacing.y * 2};
        SetCursorPos(pos);
    }
}

ImVec2 Controls::get_size(ImVec2 button_size) const
{
    auto& style = GetStyle();
    auto width = (button_size.x + style.ItemSpacing.x * 2) * button_count - style.ItemSpacing.x;
    auto height = button_size.y + style.ItemSpacing.y * 2;
    return {width, height};
}

bool Controls::image_button(const char* label, ImVec2 button_size, const ThemedTexture& texture, bool disabled, bool same_line) const
{
    // Tint colors for the buttons
    const ImVec4 tint_col = ImVec4(1, 1, 1, 1);
    const ImVec4 tint_col_hover = ImVec4(1, 0.62f, 0, 1);

    static map<const ThemedTexture*, ImVec4> tint_colors{
        {&play_button, tint_col},
        {&stop_button, tint_col},
        {&clear_button, tint_col},
        {&wrench_button, tint_col}};

    auto tint = tint_colors.at(&texture);

    BeginDisabled(disabled);
    if (same_line) SameLine();
    auto result = ImageButton(label, texture.get_texture_id(), button_size, {}, {1, 1}, {}, tint);
    EndDisabled();

    if (IsItemHovered()) {
        tint_colors[&texture] = tint_col_hover;
    } else {
        tint_colors.at(&texture) = tint_col;
    }

    return result;
}
