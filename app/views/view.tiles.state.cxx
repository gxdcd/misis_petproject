module;

#pragma region Header
module view.tiles:state;
using namespace std;
#pragma endregion

struct ViewTilesState {
    string search_path;
    bool empty{true};
    bool scanning{};
    size_t total_rows{};
    optional<size_t> visible_rows{};
    bool is_busy() const { return scanning; }
};