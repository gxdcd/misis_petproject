module;

#pragma region Header
#include <nlohman_impl.hxx>
#include <imgui_impl.hxx>
export module view.tiles;
import :table;
import :state;
import :controls;
import :status;
import :paths;
import :pdfbox;
import logging;
import config;
import model;
import utf;
import widgets;
using namespace std;
using namespace model;
using namespace ImGui;
#pragma endregion

export struct ViewTiles {
    ViewTiles();
    ~ViewTiles();

    ViewTiles(const ViewTiles&) = delete;
    ViewTiles(ViewTiles&&) = delete;
    ViewTiles& operator=(const ViewTiles&) = delete;
    ViewTiles& operator=(ViewTiles&&) = delete;

    void display();

private:
    void start_scan();
    void stop_scan();
    void clear_all();
    void file_selected(size_t entry_index);
    void display_status(float status_height);

private:
    Sync sync;
    Model model{sync};
    ViewTilesState state{};
    TilesTable table;
    Controls controls{};
    Status status{};
    widgets::AppButton appbutton{ImVec2{64, 64}};
    Paths paths{};
    widgets::MsgBox msg{};
    PdfBox pdf{};
    widgets::BusyIndicator busy{};
};

module :private;

constexpr auto TILE_VIEW_JSON = "tile.view";
constexpr auto SEARCH_PATH_JSON = "search.path";

ViewTiles::ViewTiles():
    table{model, [&](size_t entry_index) {
              file_selected(entry_index);
          }}
{
    string search_path;
    optional<string> warning;
    app::with_config_json([&](auto& config_json) {
        if (config_json.find(TILE_VIEW_JSON) != config_json.end())
            try {
                search_path = config_json[TILE_VIEW_JSON][SEARCH_PATH_JSON].get<string>();
            } catch (const std::exception& e) {
                warning = format("Error reading search path from config: {}", e.what());
            }
    });
    if (warning)
        logging::error(*warning);

    auto& sp = model.get_search_paths();
    sp.clear();
    if (!search_path.empty())
        sp.emplace_back(search_path);
    if (!sp.empty())
        state.search_path = sp.front();

    model.get_matcher().extensions = {".pdf"};

    sync.abort = [&] { return !state.scanning; };
    sync.started = [&] {
        state.total_rows = 0;
        state.visible_rows = 0;
        state.scanning = true; };
    sync.progress = [&](size_t n) {
        state.total_rows = n;
        state.empty = n == 0;
    };
    sync.finished = [&] {
        state.scanning = false;
        state.total_rows = model.size();
        state.visible_rows = state.total_rows;
        state.empty = model.empty();
    };
}

ViewTiles::~ViewTiles()
{
    stop_scan();
    auto& sp = model.get_search_paths();
    if (!sp.empty()) {
        app::with_config_json([&](auto& config_json) {
            config_json[TILE_VIEW_JSON][SEARCH_PATH_JSON] = sp.front();
        });
    }
}

void ViewTiles::file_selected(size_t entry_index)
{
    const auto& entry = model.get_entry(entry_index);
    auto full_path = utf::codecvt_utfstr(entry.path());
    auto full_path_s = full_path ? *full_path : full_path.error();
    if (!filesystem::exists(entry))
        msg.show_dialog("File not found", full_path_s);
    else
        pdf.show_dialog("File selected", full_path_s);
}

void ViewTiles::display()
{
    auto* viewport = GetMainViewport();
    auto status_height = status.get_height();

    msg.display();
    pdf.display();

    if (paths.display(model) && !model.get_search_paths().empty())
        state.search_path = model.get_search_paths().front();

    if (!model.empty()) {
        auto max_table_height = viewport->Size.y - GetCursorPosY() - status_height;
        if (state.scanning) {
            auto lock = model.lock_updates();
            table.display(max_table_height);
        } else
            table.display(max_table_height);
        Separator();
    } else {
        auto status_pos = GetCursorPos();
        status_pos.y += viewport->Size.y - status_height;
        SetCursorPos(status_pos);
    }

    display_status(status_height);
}

void ViewTiles::display_status(float status_height)
{
    status.display(state);

    auto display_controls = [&](optional<ImVec2> button_size) {
        controls.display(
            model.empty(),
            button_size,
            state,
            model.get_search_paths().empty(),
            [&] { start_scan(); CloseCurrentPopup(); },
            [&] { stop_scan(); CloseCurrentPopup(); },
            [&] { clear_all(); CloseCurrentPopup(); },
            [&] { paths.show_dialog(); CloseCurrentPopup(); });
    };

    auto display_options = [&](optional<ImVec2>) {
        SeparatorText("Options");
        auto show_demo_window = app::get_config().show_demo_window;
        Checkbox("Show Demo Window", &show_demo_window);
        app::with_config([&](auto& config) {
            config.show_demo_window = show_demo_window;
        });
    };

    auto display_options_and_controls = [&](optional<ImVec2> button_size) {
        display_options(button_size);
        Spacing();
        Separator();
        Spacing();
        display_controls(button_size);
    };

    {
        auto button_style = widgets::styles::ButtonStyle(ImGuiCol_ModalWindowDimBg);
        if (model.empty()) {
            display_controls(nullopt);
            appbutton.display(display_options, {status_height, status_height});
        } else
            appbutton.display(display_options_and_controls, {status_height, status_height});
    }

    busy.display(state.is_busy());
}

void ViewTiles::start_scan()
{
    clear_all();
    model.start_scan();
}

void ViewTiles::stop_scan()
{
    state.scanning = false;
}

void ViewTiles::clear_all()
{
    model.clear();
    state.empty = model.empty();
}
