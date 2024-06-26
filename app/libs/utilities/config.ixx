module;

#pragma region Header
#include <nlohman_impl.hxx>
#include <imgui_impl.hxx>
export module config;
import logging;
import utf;
import os;
using json = nlohmann::json;
using namespace std;
using namespace std::filesystem;
#pragma endregion

#pragma region Consts
const int DEFAULT_FRAME_WIDTH = 1280;
const int DEFAULT_FRAME_HEIGHT = 720;
const int DEFAULT_FRAME_LEFT = 100;
const int DEFAULT_FRAME_TOP = 100;
const int DEFAULT_FONT_SIZE = 16;
const auto CONFIG_FILE_NAME = "config.json";
#pragma endregion

#pragma region Statics
static path config_file{};
static json config_json{};
#pragma endregion

export namespace app {

    export using json = nlohmann::json;

    struct Config {
        int window_top = DEFAULT_FRAME_TOP;
        int window_left = DEFAULT_FRAME_LEFT;
        int window_width = DEFAULT_FRAME_WIDTH;
        int window_height = DEFAULT_FRAME_HEIGHT;
        bool window_maximized = false;
        bool dark_theme = true;
        int font_size = DEFAULT_FONT_SIZE;
        bool use_system_font_size = false;
        bool show_demo_window = false;
        Config()
        {
            auto rect = os::get_display_size();
            window_top = (rect.bottom - rect.top) / 2 - window_height / 2;
            window_left = (rect.right - rect.left) / 2 - window_width / 2;
        };
    };

    void with_config(function<void(Config&)> action);

    /// Include `<nlohman_impl.hxx>` to avoid unclear issues.
    void with_config_json(function<void(json&)> action)
    {
        action(config_json);
    }

    const Config& get_config();

    const Config& load_config(
        const expected<path, string>& base_config_path,
        const string_view& file_name = CONFIG_FILE_NAME);

    void update_config(
        function<void(int& left, int& top)> get_pos,
        function<void(int& width, int& height)> get_size,
        bool maximized,
        bool dark_theme_enabled);

    void update_config(
        function<void(int& left, int& top, int& width, int& height, bool& maximized)> get_placement,
        bool dark_theme_enabled);
}

module :private;

namespace app {
    static Config config_instance{};

    constexpr auto WINDOW_CONFIG_JSON = "window.config";

    namespace details {
        auto to_json(const Config& cfg)
        {
            return json{
                {"window_top", cfg.window_top},
                {"window_left", cfg.window_left},
                {"window_width", cfg.window_width},
                {"window_height", cfg.window_height},
                {"window_maximized", cfg.window_maximized},
                {"font_size", cfg.font_size},
                {"use_system_font_size", cfg.use_system_font_size},
                {"dark_theme", cfg.dark_theme},
                {"show_demo_window", cfg.show_demo_window}};
        }

        void update_from_json(Config& cfg, const auto& j)
        {
            j.at("window_top").get_to(cfg.window_top);
            j.at("window_left").get_to(cfg.window_left);
            j.at("window_width").get_to(cfg.window_width);
            j.at("window_height").get_to(cfg.window_height);
            j.at("window_maximized").get_to(cfg.window_maximized);
            j.at("font_size").get_to(cfg.font_size);
            j.at("use_system_font_size").get_to(cfg.use_system_font_size);
            j.at("dark_theme").get_to(cfg.dark_theme);
            j.at("show_demo_window").get_to(cfg.show_demo_window);
        }

        void save_config(const Config& config)
        {
            try {
                std::ofstream file(config_file);
                config_json[WINDOW_CONFIG_JSON] = to_json(config);
                auto pretty_printed = config_json.dump(4);
                file << pretty_printed;
            } catch (const std::exception& e) {
                logging::warn("Failed to save config: {}", e.what());
            }
        }
    }

    const Config& load_config(
        const expected<path, string>& base_config_path,
        const string_view& file_name)
    {
        if (!base_config_path.has_value()) {
            logging::warn("Failed to get config folder: {}", base_config_path.error());
            return config_instance;
        }

        config_file = *base_config_path / file_name;

        try {
            ifstream file(config_file);
            if (file.is_open()) {
                file >> config_json;
                details::update_from_json(config_instance, config_json[WINDOW_CONFIG_JSON]);
            }
        } catch (const std::exception& e) {
            auto file_path = utf::codecvt_utfstr(config_file);
            logging::warn("Failed to load config from file {}: {}",
                file_path ? *file_path : "unrepresentable ASCII path", e.what());
        }

        return config_instance;
    }

    void update_config(
        function<void(int& left, int& top)> get_pos,
        function<void(int& width, int& height)> get_size,
        bool maximized,
        bool dark_theme_enabled)
    {
        get_pos(config_instance.window_left, config_instance.window_top);
        get_size(config_instance.window_width, config_instance.window_height);
        config_instance.window_maximized = maximized;
        config_instance.dark_theme = dark_theme_enabled;
        details::save_config(config_instance);
    }

    void update_config(
        function<void(int& left, int& top, int& width, int& height, bool& maximized)> get_placement,
        bool dark_theme_enabled)
    {
        get_placement(
            config_instance.window_left,
            config_instance.window_top,
            config_instance.window_width,
            config_instance.window_height,
            config_instance.window_maximized);
        config_instance.dark_theme = dark_theme_enabled;
        details::save_config(config_instance);
    }

    const Config& get_config()
    {
        return config_instance;
    }

    void with_config(function<void(Config&)> action)
    {
        action(config_instance);
    }
}