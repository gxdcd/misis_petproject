module;

#pragma region Header
#include <imgui_impl.hxx>
#include <IconsFontAwesome5.h>
export module widgets:symbuttons;
using namespace ImGui;
#pragma endregion

namespace widgets {
    export enum class Symbol {
        Default,
        Asterisk,
        Plus,
        Minus,
        Tablet,
        MAX
    };

    export const char* GetSymbol(Symbol symbol)
    {
        switch (symbol) {
            case Symbol::Default:
            case Symbol::Asterisk:
                return ICON_FA_ASTERISK;
            case Symbol::Plus:
                return ICON_FA_PLUS_CIRCLE;
            case Symbol::Minus:
                return ICON_FA_MINUS_CIRCLE;
            case Symbol::Tablet:
                return ICON_FA_TABLET;
            default:
                return ICON_FA_ASTERISK;
        }
    }

    export auto SymbolSize(Symbol symbol = Symbol::Default)
    {
        return CalcTextSize(GetSymbol(symbol), NULL, true);
    }

    export bool SymbolButton(Symbol symbol = Symbol::Default)
    {
        return Button(GetSymbol(symbol));
    }
}