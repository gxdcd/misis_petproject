module;

#pragma region Header
#include <Windows.h>
#include <shlobj_core.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
export module win;
using namespace std;
using namespace filesystem;
#pragma endregion

constexpr auto __MAX_PATH = MAX_PATH;
#undef MAX_PATH

/// @brief The os namespace contains functions for interacting with the operating system.
export namespace os {

    constexpr auto MAX_PATH = __MAX_PATH;

    struct DriveInfo {
        enum class DriveType {
            /// @brief The drive type cannot be determined.
            Unknown = 0,
            /// @brief The root path is invalid; for example, there is no volume mounted at the specified path.
            NoRootDir = 1,
            /// @brief The drive has a removable media device, such as a floppy drive or flash card reader.
            Removable = 2,
            /// @brief The drive has a fixed (nonremovable) media device, such as a hard drive or flash memory device.
            Fixed = 3,
            /// @brief The drive is a remote (network) drive.
            Remote = 4,
            /// @brief The drive is a CD-ROM drive.
            CDROM = 5,
            /// @brief The drive is a RAM disk.
            RAMDisk = 6
        };

        string path;
        DriveType type;

        static string type_description(DriveType type);
    };

    struct FontInfo {
        wstring font_path;
        LONG font_height;
    };

    struct MemoryInfo {
        /// The current working set size, in kilobytes.
        size_t working_set_size;
        /// The peak working set size, in kilobytes.
        size_t peak_working_set_size;
    };
}

using error_t = decltype(GetLastError());

export namespace os {
    bool file_exists(const string& file_path)
    {
        return INVALID_FILE_ATTRIBUTES != GetFileAttributesA(file_path.c_str());
    }
    RECT get_display_size();
    void get_window_placement(HWND hwnd, int& left, int& top, int& width, int& height, bool& maximized);
    void set_window_placement(HWND hwnd, int left, int top, int width, int height, bool maximized, bool show = true);
    MemoryInfo get_memory_info();
    string get_error_message(const error_t err, bool system_wide_language = true);
    string current_exe_path();
    expected<path, string> get_current_exe();
    /// Retrieves the current executable folder path.
    /// @return An `std::expected` object containing the current executable folder path on success,
    ///         or an error message as an `std::string` on failure.
    expected<path, string> get_current_exe_folder();
    expected<path, string> get_appdata_folder();
    expected<path, string> get_appdata_config_folder(const string& application_name);
    /// Retrieves the configuration folder path.
    /// @return An expected object containing the configuration folder path on success,
    ///         or an error message on failure.
    expected<path, string> get_config_folder();
    vector<DriveInfo> get_logical_drives();
    expected<path, string> get_font_filename(const wstring& font_name);
    /// Retrieves the path to the fonts folder.
    /// @return An `expected` object containing the path to the fonts folder on success,
    ///         or an `unexpected` object containing an error message on failure.
    expected<path, string> get_fonts_folder();
    /// Retrieves the system UI font name and height.
    /// @return An expected object containing the FontInfo struct if successful, or an error message if failed.
    expected<FontInfo, string> get_system_ui_font_metrics();
    /// Retrieves the system UI font data.
    /// The path to the system UI font file is retrieved from the Windows registry
    /// and verified to exist, if file does not exist, an error is returned.
    /// @param font_size The optional font size. If provided, the font will be scaled to this size.
    /// @return An expected object containing the FontInfo if successful, or an error message if failed.
    expected<FontInfo, string> get_system_ui_font_data(optional<int> font_size);
}

module :private;

namespace os {

    RECT get_display_size()
    {
        RECT rect;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
        return rect;
    }

    void get_window_placement(HWND hwnd, int& left, int& top, int& width, int& height, bool& maximized)
    {
        WINDOWPLACEMENT wp;
        wp.length = sizeof(wp);
        if (GetWindowPlacement(hwnd, &wp)) {
            left = wp.rcNormalPosition.left;
            top = wp.rcNormalPosition.top;
            width = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
            height = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
            maximized = wp.showCmd == SW_SHOWMAXIMIZED;
        }
    }

    void set_window_placement(HWND hwnd, int left, int top, int width, int height, bool maximized, bool show)
    {
        WINDOWPLACEMENT wp;
        wp.length = sizeof(wp);
        if (GetWindowPlacement(hwnd, &wp)) {
            wp.rcNormalPosition.left = left;
            wp.rcNormalPosition.top = top;
            wp.rcNormalPosition.right = left + width;
            wp.rcNormalPosition.bottom = top + height;
            wp.showCmd = show
                             ? maximized ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL
                             : SW_HIDE;
            SetWindowPlacement(hwnd, &wp);
        }
    }
}

namespace os {
    MemoryInfo get_memory_info()
    {
        auto process = OpenProcess(
            PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
            FALSE, GetCurrentProcessId());

        if (process == NULL) return {};

        auto process_closer = unique_ptr<void, decltype([](auto h) {
            CloseHandle(h);
        })>(process);

        PROCESS_MEMORY_COUNTERS_EX pmc;
        pmc.cb = sizeof(pmc);
        if (GetProcessMemoryInfo(process, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
            return MemoryInfo{
                pmc.WorkingSetSize / 1024,
                pmc.PeakWorkingSetSize / 1024,
            };
        return {};
    }
    expected<path, string> get_font_filename(const wstring& font_name)
    {
        const auto fonts_reg_key = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
        HKEY h_key;
        LONG result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, fonts_reg_key, 0, KEY_READ, &h_key);
        if (result == ERROR_SUCCESS) {
            auto close_key = unique_ptr<HKEY, function<void(HKEY*)>>(&h_key, [](auto h) { RegCloseKey(*h); });
            WCHAR val_name[MAX_PATH];
            WCHAR val_data[MAX_PATH];
            DWORD val_index = 0;
            DWORD val_name_size = MAX_PATH;
            DWORD val_data_size = MAX_PATH;
            while (ERROR_SUCCESS == RegEnumValueW(h_key, val_index, val_name, &val_name_size, nullptr, nullptr, (LPBYTE)val_data, &val_data_size)) {
                wstring name(val_name);
                if (name.find(font_name) != wstring::npos) {
                    wstring data(val_data);
                    return data;
                }
                val_index++;
                val_name_size = MAX_PATH;
                val_data_size = MAX_PATH;
            }
        }
        return unexpected("Failed to get font file name");
    }

    expected<path, string> get_fonts_folder()
    {
        TCHAR buffer[MAX_PATH];
        if (!SHGetSpecialFolderPath(0, buffer, CSIDL_FONTS, 0))
            return unexpected("Failed to get fonts folder");
        return filesystem::path(buffer);
    }

    expected<FontInfo, string> get_system_ui_font_metrics()
    {
        NONCLIENTMETRICS metrics;
        metrics.cbSize = sizeof(NONCLIENTMETRICS);
        if (!SystemParametersInfo(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0))
            return unexpected("Failed to get system UI font info");
        return FontInfo{
            // use here the lfFaceName member of the LOGFONTA structure
            // this is not the path to the font file, but the name of the font
            metrics.lfMessageFont.lfFaceName,
            metrics.lfCaptionFont.lfHeight};
    }

    expected<FontInfo, string> get_system_ui_font_data(optional<int> font_size)
    {
        auto fonts_folder_path = get_fonts_folder();
        if (!fonts_folder_path.has_value())
            return unexpected(fonts_folder_path.error());

        auto system_ui_font = get_system_ui_font_metrics();
        if (!system_ui_font.has_value())
            return unexpected(system_ui_font.error());

        auto system_ui_font_filename = get_font_filename(system_ui_font.value().font_path);
        if (!system_ui_font_filename.has_value())
            return unexpected(system_ui_font_filename.error());

        auto system_ui_font_path = fonts_folder_path.value() / system_ui_font_filename.value();

        if (!filesystem::exists(system_ui_font_path))
            return unexpected("System UI font file not found");

        HDC hdc = GetDC(NULL);                                   // Get a handle to the display device context
        auto pixels_per_inch_y = GetDeviceCaps(hdc, LOGPIXELSY); // Get the number of pixels per logical inch along the screen height
        ReleaseDC(NULL, hdc);                                    // Release the device context

        auto point_size = font_size.has_value() ? *font_size : system_ui_font.value().font_height;

        // Convert from logical units to pixels
        // https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-logfonta
        auto font_height = float(abs(MulDiv(point_size, pixels_per_inch_y, 72)));

        return FontInfo{
            system_ui_font_path,
            long(font_height)};
    }

}

namespace os {

    string get_error_message(const error_t err, bool system_wide_language)
    {
        LPWSTR wbuf = nullptr;
        size_t size = FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            err,
            system_wide_language
                ? MAKELANGID(LANG_USER_DEFAULT, SUBLANG_DEFAULT)
                : MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
            (LPWSTR)&wbuf,
            0,
            nullptr);

        if (wbuf == nullptr) return {};
        if (size == 0) return {};

        wstring wstr{wbuf, wbuf + size};

        LocalFree(wbuf);

        while (!wstr.empty() && isspace(wstr.back()))
            wstr.erase(wstr.length() - 1, 1);

#pragma warning(push)
#pragma warning(disable : 4996)
        wstring_convert<codecvt_utf8_utf16<wchar_t>> conv;
        return conv.to_bytes(wstr);
#pragma warning(pop)
    }

    string current_exe_path()
    {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, sizeof(buffer));
        return buffer;
    }

    expected<path, string> get_current_exe()
    {
        TCHAR buffer[MAX_PATH];
        /*
        https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamea

        If the function succeeds, the return value is the length of the string
        that is copied to the buffer, in characters, not including the terminating
        null character. If the buffer is too small to hold the module name, the
        string is truncated to nSize characters including the terminating null
        character, the function returns nSize, and the function sets the last
        error to ERROR_INSUFFICIENT_BUFFER.
        */
        if (DWORD len = GetModuleFileName(NULL, buffer, MAX_PATH); len == 0)
            return unexpected("Failed to get module file name");
        return path(buffer);
    }

    expected<path, string> get_current_exe_folder()
    {
        auto exe = get_current_exe();
        return exe ? exe.value().parent_path() : exe;
    }

    expected<path, string> get_appdata_folder()
    {
        TCHAR buffer[MAX_PATH];
        if (!SHGetSpecialFolderPath(0, buffer, CSIDL_APPDATA, 0))
            return unexpected("Failed to get appdata");
        return buffer;
    }

    expected<path, string> get_appdata_config_folder(const string& application_name)
    {
        auto appdata_folder = get_appdata_folder();
        // If the appdata folder is not found, return the error `unexpected` object
        // otherwise, return the path to the application's config folder
        return !appdata_folder ? appdata_folder : appdata_folder.value() / application_name;
    }

    expected<path, string> get_config_folder()
    {
        return get_current_exe_folder();
    }
}

namespace os {

    vector<DriveInfo> get_logical_drives()
    {
        vector<DriveInfo> drives{};
        DWORD drives_mask = GetLogicalDrives();
        char buf[] = "_:\\\0";
        for (char bit = 0; bit < sizeof(DWORD) * 8; ++bit) {
            if (drives_mask & (1 << bit)) {
                buf[0] = char('A' + bit);
                drives.emplace_back(
                    DriveInfo{
                        buf,
                        static_cast<DriveInfo::DriveType>(GetDriveTypeA(buf)) // formatting
                    });
            }
        }
        return drives;
    }

    string DriveInfo::type_description(DriveType type)
    {
        const auto drive_type_names = array<const char*, 7>{
            "Unknown",
            "NoRootDir",
            "Removable",
            "Fixed",
            "Remote",
            "CDROM",
            "RAMDisk"};
        return drive_type_names[static_cast<int>(type)];
    }
}
