module;

#pragma region Header
#include <imgui_impl.hxx>
#include <ImGuiFileDialog.h>
module view.tiles:paths;
import model;
import utils;
import logging;
import themes;
using namespace ImGui;
using namespace IGFD;
using namespace std;
using namespace model;
#pragma endregion

struct Paths {
    using container = Model::input_container;

    bool display(Model& model)
    {
        auto& search_paths = model.get_search_paths();
        if (file_dialog.IsOpened())
            return check_dialog(search_paths);
        return false;
    }

    void show_dialog()
    {
        // https://github.com/aiekick/ImGuiFileDialog
        FileDialogConfig config;
        config.path = ".";
        config.flags = ImGuiFileDialogFlags_Default
                       | ImGuiFileDialogFlags_DisableCreateDirectoryButton
                       | ImGuiFileDialogFlags_ShowDevicesButton;
        file_dialog.OpenDialog("ChooseSearchPaths", " Choose search path", nullptr, config);
    }

private:
    void selected(container& search_paths, const string& path)
    {
        search_paths.clear();
        search_paths.emplace_back(utils::normalize_path(path));
    }

    bool check_dialog(container& search_paths)
    {
        if (file_dialog.IsOpened())
            try {
                auto* viewport = GetMainViewport();
                if (file_dialog.Display("ChooseSearchPaths",
                        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings,
                        {viewport->Size.x * 0.7f, viewport->Size.y * 0.7f},
                        {viewport->Size.x * 0.7f, viewport->Size.y * 0.7f})) //
                {
                    if (file_dialog.IsOk()) {
                        auto filePath = file_dialog.GetCurrentPath();
                        selected(search_paths, filePath);
                        file_dialog.Close();
                        return true;
                    }
                    file_dialog.Close();
                }
            } catch (const exception& e) {
                logging::error("Error in file dialog: {}", e.what());
            }
        return false;
    }

private:
    ImGuiFileDialog file_dialog;
};