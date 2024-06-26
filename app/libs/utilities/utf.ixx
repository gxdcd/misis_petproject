export module utf;

using namespace std;
#pragma warning(push)
#pragma warning(disable : 4996)

namespace utf {

    export using string_result = expected<string, string>;

    /// https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
    /// https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
    export string_result codecvt_utfstr(const wstring& wstr)
    {
        try {
            static wstring_convert<codecvt_utf8<wchar_t>, wchar_t> wstring_to_utf8;
            // If this wstring_convert object was constructed without a user-supplied
            // byte-error string, throws range_error on conversion failure.
            return wstring_to_utf8.to_bytes(wstr);
        } catch (const std::range_error& e) {
            return unexpected(std::format("Failed to convert string to `utf-8`: {}", e.what()));
        }
    }

    /// https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
    /// https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
    export string_result codecvt_utfstr(const filesystem::path& path)
    {
        return codecvt_utfstr(path.native());
    }
}
#pragma warning(pop)