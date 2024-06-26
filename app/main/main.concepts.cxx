module;

#pragma region Header
export module main:concepts;
#pragma endregion

namespace app {

    export template<typename T>
    concept is_view = requires(T t) {
        {
            t.display()
        };
    };

    export template<typename T>
    concept creates_view = requires(T t) {
        {
            t()
        } -> is_view;
    };

}