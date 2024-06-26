module;

#pragma region Header
#include <nlohman_impl.hxx>
export module model;
import logging;
import stopwatch;
import config;
import os;
import utils;
import :matcher;
import threadloop;
export import :sync;
export import :matcher;
using namespace std;
using namespace filesystem;
#pragma endregion

constexpr auto SEARCH_PATHS_JSON = "search.paths";

namespace model {
    export struct Model {
        using input_container = vector<string>;

        Model(Sync& sync);
        ~Model();
        Model(const Model&) = delete;
        Model(Model&&) = delete;
        Model& operator=(const Model&) = delete;
        Model& operator=(Model&&) = delete;

        input_container& get_search_paths() { return search_paths; }
        Matcher& get_matcher() { return matcher; }

        const auto& get_entry(size_t i) const
        {
            return items.at(i);
        }

        size_t size() const { return items.size(); }
        bool empty() const { return items.empty(); }
        void clear() { items.clear(); }

        void start_scan();
        void stop_scan();

        auto lock_updates() const { return lock_guard(scan_mutex); }

    private:
        void scan();
        void scan_directory(const string& unc_path);

        void read_config(const string& section_name = SEARCH_PATHS_JSON);
        void write_config(const string& section_name = SEARCH_PATHS_JSON);

    private:
        input_container search_paths;
        Matcher matcher{};
        ThreadLoop thread_loop;
        mutable mutex scan_mutex;
        Sync& sync;
        vector<directory_entry> items;
    };
}

module :private;

namespace model {
    Model::Model(Sync& sync):
        sync{sync}, thread_loop{"Scanning", [&] { scan(); }}
    {
        logging::info("Model created");
    }

    void Model::read_config(const string& section_name)
    {
        // https://stackoverflow.com/questions/74549928/concept-for-constraining-parameter-pack-to-string-like-types-or-types-convertibl
        auto loaded = false;
        app::with_config_json([&](auto& config_json) {
            if (config_json.find(section_name) != config_json.end()) {
                search_paths = config_json[section_name].get<input_container>();
                loaded = true;
            }
        });
        if (loaded)
            logging::info("Search paths loaded from config");
    }

    void Model::write_config(const string& section_name)
    {
        app::with_config_json([&](auto& config_json) {
            config_json[section_name] = search_paths;
        });
    }

    Model::~Model()
    {
        logging::info("Destroying the model");
        stop_scan();
        clear();
    }

    void Model::start_scan() { thread_loop.start(); }

    void Model::stop_scan() { thread_loop.stop(); }

    void Model::scan()
    {
        sync.started();

        for (const string& path: get_search_paths()) {
            Stopwatch sw{};
            try {
                logging::info(R"(Scanning directory "{}")", path);

                // We use "\\?\" to support long paths exceeding MAX_PATH 260 characters
                // https://learn.microsoft.com/en-us/dotnet/standard/io/file-path-formats
                scan_directory(format("\\\\?\\{}\\", path));

                if (thread_loop.stop_requested()) {
                    logging::debug(R"(Scanning thread requested to stop inside the scanning loop)");
                    break;
                }
                if (sync.abort()) {
                    logging::debug(R"(Scanning thread requested to abort from UI)");
                    break;
                }

            } catch (const system_error& e) {
                logging::error(R"(Error scanning "{}": {})", path, utils::errmsg(e.code()));
            } catch (const exception& e) {
                logging::error(R"(Error scanning "{}": {})", path, e.what());
            }
            logging::debug(R"(Scanning "{}" took {})", path, sw.elapsed_units());
        }

        sync.finished();
    }

    void Model::scan_directory(const string& unc_path)
    {
        auto process_entry = [&](const directory_entry& dir_entry) {
            std::error_code ec{};
            // Do not process symlinks
            auto status = dir_entry.symlink_status(ec);
            if (ec.value() != 0 || is_symlink(status)) return;

            status = dir_entry.status(ec);
            if (ec.value() != 0) return;

            if (!is_directory(status) && is_regular_file(status)) {
                auto matched = matcher.match(dir_entry);
                if (!matched)
                    return;
                auto lock = lock_updates();
                items.emplace_back(dir_entry);
                sync.progress(items.size());
            }
        };

        try {
            logging::debug(R"(Scanning files in: "{}")", unc_path);

            std::error_code error{};
            auto options = directory_options::skip_permission_denied;

            auto it = recursive_directory_iterator(unc_path, options, error);
            auto stop = end(it);

            if (error.value() != 0) {
                auto err = utils::errmsg(error);
                logging::error(R"(Error creating iterator for "{}": {})", unc_path, err);
                return;
            }

            while (it != stop) {
                auto& dir_entry = *it;

                process_entry(dir_entry);

                it.increment(error);

                if (error.value() != 0) {
                    auto err = utils::errmsg(error);
                    logging::error("Error incrementing iterator: {}", err);
                }

                if (sync.abort()) {
                    logging::debug("Aborted scanning files");
                    break;
                }
            }
        } catch (const std::exception& e) {
            logging::error(R"(Error scanning files in "{}": {})", unc_path, e.what());
        }
    }
}