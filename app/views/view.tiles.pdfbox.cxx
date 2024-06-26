module;

#pragma region Header
#include <imgui_impl.hxx>
export module view.tiles:pdfbox;
import :pdf;
import widgets;
using namespace ImGui;
using namespace std;
#pragma endregion

template<typename T>
void swap_without_squiggles(T& v1, T&& v2)
{
    v1.swap(v2);
}

export struct PdfBox {
    PdfBox() = default;
    ~PdfBox() = default;
    PdfBox(const PdfBox&) = delete;
    PdfBox(PdfBox&&) = delete;
    PdfBox& operator=(const PdfBox&) = delete;
    PdfBox& operator=(PdfBox&&) = delete;

    bool display()
    {
        if (!full_path)
            return false;
        OpenPopup(title->c_str());
        if (BeginPopupModal(title->c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            // we do the below ops in the following order to avoid flicker when changing pages

            // render page if it has been changed
            if (update_page_index) {
                tex = pdf.and_then([=](auto& p) { return p.render_page(*update_page_index - 1); });
                update_page_index.reset();
            }

            // display the page or the error
            if (tex) {
                Image(tex->get_texture_id(), tex->get_size());
                Text(full_path->c_str());
            } else {
                Text(full_path->c_str());
                Text(tex.error().c_str());
            }

            // change the page by slider if needed
            if (pdf && ImGui::SliderInt("Page", &page_index, 1, pdf->page_count()))
                update_page_index = page_index;

            // draw the button in center
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
#pragma warning(disable : 4458)
    void show_dialog(const string& title, const string& full_path)
    {
        this->title = title;
        this->full_path = full_path;

        swap_without_squiggles(pdf, try_load(full_path)); // avoid weird error squiggles from intellisense
        page_index = 1;
        update_page_index = 1;

        if (!IsPopupOpen(this->title->c_str())) {
            set_pos();
            OpenPopup(this->title->c_str());
            display();
        }
    }
#pragma warning(pop)

    void close_dialog()
    {
        if (full_path) {
            CloseCurrentPopup();
            full_path.reset();
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
    optional<string> full_path;
    expected<PDF, string> pdf;
    std::expected<Texture, std::string> tex;
    int page_index{};
    optional<int> update_page_index{};
};
