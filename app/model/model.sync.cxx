module;

#pragma region Header
export module model:sync;
using namespace std;
#pragma endregion

namespace model {
    export struct Progress {
        size_t found;
        size_t scanned;
    };

    export struct Sync {
        function<void()> started;
        function<void(size_t)> progress;
        function<void()> finished;
        function<bool()> abort;
    };
}