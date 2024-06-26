module;

#pragma region Header
export module model:matcher;
import logging;
using namespace std;
using namespace filesystem;
#pragma endregion

namespace model {
    export struct Matcher {
        void enable(bool value) { enabled = value; }
        bool is_enabled() const { return enabled; }
        bool match(const directory_entry& entry) const
        {
            return !enabled || internal_match(entry);
        }
        optional<vector<string>> extensions{};
    private:
        bool internal_match(const directory_entry& entry) const;
        bool match_extension(const directory_entry& entry) const;
    private:
        bool enabled{true};
    };
}

bool model::Matcher::internal_match(const directory_entry& entry) const
{
    return match_extension(entry);
}

bool model::Matcher::match_extension(const directory_entry& entry) const
{
    return !extensions || find(extensions->begin(), extensions->end(), entry.path().extension()) != extensions->end();
}
