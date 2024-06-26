module;

#pragma region Header
#include <imgui_impl.hxx>
export module widgets:msgbox;
using namespace ImGui;
using namespace std;
#pragma endregion

namespace widgets {

    export struct MsgBox {
        bool display()
        {
            if (!message)
                return false;
            OpenPopup(title->c_str());
            if (BeginPopupModal(title->c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                Text(message->c_str());
                ImVec2 button_size(GetFontSize() * 7.0f, 0.0f);
                auto available = GetContentRegionAvail().x;
                auto button_pos = (available - button_size.x) / 2;
                Spacing();
                Spacing();
                SameLine(button_pos);
                if (Button("Ok", button_size))
                    close_dialog();
                EndPopup();
            }
            return false;
        }

#pragma warning(push)
#pragma warning(disable: 4458)
        void show_dialog(const string& title, const string& message)
        {
            this->title = title;
            this->message = message;
            if (!IsPopupOpen(this->title->c_str())) {
                set_pos();
                OpenPopup(this->title->c_str());
                display();
            }
        }
#pragma warning(pop)

        void close_dialog()
        {
            if (message) {
                CloseCurrentPopup();
                message.reset();
                title.reset();
            }
        }

    private:
        void set_pos()
        {
            auto pos = GetIO().DisplaySize;
            pos.x *= 0.5f;
            pos.y *= 0.5f;
            SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        }

    private:
        optional<string> title;
        optional<string> message;
    };

}