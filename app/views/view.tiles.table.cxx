module;

#pragma region Header
#include <imgui_impl.hxx>
module view.tiles:table;
import model;
import logging;
import stopwatch;
import widgets;
import utf;
import themes;
using namespace ImGui;
using namespace std;
using namespace model;
using namespace widgets;
#pragma endregion

using file_selected_callback = const function<void(size_t)>;

struct TilesTable {
    TilesTable(const Model& model, file_selected_callback file_selected):
        model{model}, file_selected{file_selected} { }

    void display(float max_height);
    void display_table(float max_height);
    void display_table_row(int row_index, int tiles_count, int column_count);

private:
    const Model& model;
    const ImVec2 button_size{64, 64};
    const float text_width = 256.f;
    ThemedTexture document{"file-document.png", 0.25f};
    file_selected_callback file_selected;
};

void TilesTable::display(float max_height)
{
    if (model.empty()) return;
    display_table(max_height);
}

void TilesTable::display_table(float max_height)
{
    ImGuiStyle& style = GetStyle();
    auto button_styles = styles::ButtonStyle(ImGuiCol_ModalWindowDimBg);

    static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY
                                   | ImGuiTableFlags_NoBordersInBody
                                   | ImGuiTableFlags_SizingFixedFit;
    static ImGuiTableColumnFlags columns_base_flags = ImGuiTableColumnFlags_None;

    auto column_width = button_size.x + text_width;

    auto base_content_width
        = GetContentRegionAvail().x - style.FramePadding.x * 2 - style.ItemSpacing.x * 2;

    auto paddings = base_content_width / column_width / 2 - 1;
    auto content_width = base_content_width
                         - style.ItemSpacing.x * paddings - style.ScrollbarSize;

    auto column_count = int(content_width / column_width);
    auto outer_size_value = ImVec2(0.0f, max_height > 0 ? max_height : 0.0f);

    if (!BeginTable("table_advanced", column_count, flags, outer_size_value))
        return;

    for (int i = 0; i < column_count; i++) {
        auto column_id = format("##{}", i);
        TableSetupColumn(column_id.c_str(), columns_base_flags, column_width, i);
    }

    int tiles_count = model.size() <= INT_MAX ? static_cast<int>(model.size()) : INT_MAX;

    int row_count = (tiles_count + column_count - 1) / column_count;
    auto row_height = button_size.y + style.ItemSpacing.y * 4;

    ImGuiListClipper clipper;
    clipper.Begin(row_count, row_height);
    while (clipper.Step()) {
        auto row_n = clipper.DisplayStart;
        auto rows_to_display = clipper.DisplayEnd - clipper.DisplayStart;
        while (rows_to_display > 0) {
            if (row_n >= row_count) break;
            display_table_row(row_n, tiles_count, column_count);
            row_n++;
            rows_to_display--;
        }
    }
    EndTable();
}

void TilesTable::display_table_row(int row_index, int tiles_count, int column_count)
{
    TableNextRow();

    auto first_index = row_index * column_count;
    auto index_limit = first_index + column_count;
    if (index_limit > tiles_count)
        index_limit = tiles_count;

    for (int i = first_index; i < index_limit; i++) {
        auto doc_id = format("File##{}", i);

        const auto& entry = model.get_entry(i);

        const auto path = utf::codecvt_utfstr(entry.path().filename());

        const ImVec4 tint_col = ImVec4(1, 1, 1, 1);

        TableNextColumn();
        PushID(i);
        if (ImageButton(doc_id.c_str(), document.get_texture_id(), button_size, {}, {1, 1}, {}, tint_col))
            file_selected(i);

        SameLine();
        if (path)
            TextWrapped(path.value().c_str());
        else
            TextWrapped(path.error().c_str());


        PopID();
    }
}