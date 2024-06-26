module;

#pragma region Header
#include <imgui_impl.hxx>
#include <IconsFontAwesome5.h>

export module fonts;

import os;
import config;
import utf;
using namespace std;
using namespace filesystem;
using namespace ImGui;
using namespace os;
using namespace app;
#pragma endregion

namespace utils::ImGui {
    using void_result = expected<void, string>;
    export void_result initialize_fonts(const Config& config);
}

module :private;

namespace utils::ImGui {

    void_result initialize_system_ui_font(const FontInfo& font_info)
    {
        auto& io = GetIO();

        ImFontConfig font_config;
        font_config.MergeMode = true;

        auto font_path = utf::codecvt_utfstr(font_info.font_path);

        if (!font_path.has_value())
            return unexpected(font_path.error());

        auto font_size = float(font_info.font_height);
        io.Fonts->AddFontFromFileTTF((*font_path).c_str(), font_size);

        // GetGlyphRangesCyrillic, GetGlyphRangesDefault etc.:
        // Helpers to retrieve list of common Unicode ranges (2 value per range, values are inclusive, zero-terminated list)
        // NB: Make sure that your string are UTF-8 and NOT in your local code page.
        // Read https://github.com/ocornut/imgui/blob/master/docs/FONTS.md/#about-utf-8-encoding for details.
        // NB: Consider using ImFontGlyphRangesBuilder to build glyph ranges from textual data.

        io.Fonts->AddFontFromFileTTF((*font_path).c_str(), font_size,
            &font_config, io.Fonts->GetGlyphRangesCyrillic());

        io.Fonts->AddFontFromFileTTF((*font_path).c_str(), font_size,
            &font_config, io.Fonts->GetGlyphRangesDefault());

        return {};
    }

    void_result initialize_fontawesome_font(decltype(FontInfo::font_height) font_height)
    {
        auto exe_path = get_current_exe_folder();
        if (!exe_path.has_value())
            return unexpected(exe_path.error());

        // https://github.com/beakerbrowser/beakerbrowser.com/blob/master/fonts/fontawesome-webfont.ttf
        auto font_path = exe_path.value() / "fontawesome-webfont.ttf";

        auto c_str = utf::codecvt_utfstr(font_path);
        if (!c_str.has_value())
            return unexpected(c_str.error());

        if (!filesystem::exists(font_path))
            return unexpected(
                format("Font file not found: {}", *c_str));

        ImGuiIO& io = GetIO();
        ImFontConfig font_config;
        font_config.MergeMode = true;
        auto font_size = float(font_height);
        // https://pixtur.github.io/mkdocs-for-imgui/site/FONTS/
        // https://pixtur.github.io/mkdocs-for-imgui/site/FONTS/#using-icons
        // https://github.com/ocornut/imgui/blob/master/docs/FONTS.md#using-icon-fonts
        static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
        io.Fonts->AddFontFromFileTTF((*c_str).c_str(),
            font_size, &font_config, icon_ranges);
        return {};
    }

    /// Dear ImGui: Using Fonts
    /// https://github.com/ocornut/imgui/blob/master/docs/FONTS.md
    /// How to use different fonts
    /// https://github.com/ocornut/imgui/discussions/5771
    void_result initialize_fonts(const Config& config)
    {
        auto ui_font_data = get_system_ui_font_data(
            config.use_system_font_size ? nullopt
                                        : optional(config.font_size));

        if (!ui_font_data.has_value())
            return unexpected(format("{}", ui_font_data.error()));

        optional<string> warning;
        auto result = initialize_system_ui_font(ui_font_data.value());
        if (!result.has_value())
            warning = format("Failed to initialize system ui font: {}", result.error());

        result = initialize_fontawesome_font(ui_font_data.value().font_height);
        if (!result.has_value())
            return unexpected(
                format("{}{}Failed to initialize fontawesome font: {}",
                    warning ? ". " : "", warning ? *warning : "", result.error()));
        else if (warning)
            return unexpected(
                format("{}", *warning));

        return {};
    }
}