module;

#pragma region Header
#include <imgui_impl.hxx>
module view.tiles:status;
import :state;
import themes;
using namespace ImGui;
using namespace std;
#pragma endregion

struct Status {
    void display(const ViewTilesState& state);
    float get_height() const;
};

float Status::get_height() const
{
    auto& style = GetStyle();
    const float TEXT_BASE_HEIGHT = GetTextLineHeightWithSpacing();
    auto text_height = TEXT_BASE_HEIGHT + style.ItemSpacing.y;
    return text_height * 3;
}

void Status::display(const ViewTilesState& state)
{
    state.scanning
        ? Text("Scanning: %d found", state.total_rows)
    : state.empty
        ? Text("Idle")
        : Text("Found %d files", state.total_rows);

    if (state.search_path.empty()) {
        Text("No search path");
    } else {
        Text("Search path: %s", state.search_path.c_str());
    }

    auto& io = GetIO();
    Text("Display stats: %.1f FPS (%.3f ms/frame).", io.Framerate, 1000.0f / io.Framerate);
}
