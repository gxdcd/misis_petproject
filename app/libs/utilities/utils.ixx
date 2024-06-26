module;

#pragma region Header
export module utils;
using namespace std;
#pragma endregion

namespace details {
    template<typename T>
        requires invocable<T>
    struct finalizer {
        finalizer(T t):
            t{t} { }
        ~finalizer() { t(); }
        T t;
    };
}

namespace utils {
    export template<typename T>
        requires invocable<T, void*>
    auto make_finalizer(T t) -> unique_ptr<void, decltype(t)>
    {
        return unique_ptr<void, T>((void*)"", t);
    }

    /// This thing will be applied to regular functions
    export template<typename T>
        requires invocable<T>
    auto make_finalizer(T t)
    {
        return details::finalizer(t);
    }

    /// This thing will be applied to lambda functions
    export template<typename T>
        requires invocable<T> && is_class_v<T>
    auto make_finalizer(T t)
    {
        return make_finalizer([=](auto) { t(); });
    }
}

namespace utils {
    /// https://stackoverflow.com/questions/73631820/how-do-i-normalize-a-filepath-in-c-using-stdfilesystempath
    export string normalize_path(const string& messyPath)
    {
        filesystem::path path(messyPath);
        filesystem::path canonicalPath = filesystem::weakly_canonical(path);
        return canonicalPath.make_preferred().string();
    }
    export string remove_trailing_slashes(const string& dir)
    {
        auto dir_len = dir.size();
        while (dir_len > 0 && (dir[dir_len - 1] == '\\' || dir[dir_len - 1] == '/'))
            --dir_len;
        return dir.substr(0, dir_len);
    }
}

namespace utils {
    export string errmsg(const error_code& err)
    {
        char buffer[256];
        strerror_s(buffer, err.value());
        return buffer;
    }
}

namespace utils {
    export string semicolon_fold(string a, string b)
    {
        auto e = a.empty();
        return std::move(a) + (e ? "" : "; ") + b;
    };
}

namespace utils {

    // https://en.cppreference.com/w/cpp/filesystem/directory_entry/file_size
    struct HumanReadable {
        uintmax_t size{};

        template<typename Os>
        friend Os& operator<<(Os& os, HumanReadable hr)
        {
            int i{};
            double mantissa = double(hr.size);
            for (; mantissa >= 1024.0; mantissa /= 1024.0, ++i) { }
            os << ceil(mantissa * 10.0) / 10.0 << " " << i["BKMGTPE"];
            // if (i) os << "B (" << hr.size << ')';
            if (i) os << "B";
            return os;
        }
    };

    export string human_readable(uintmax_t size)
    {
        stringstream ss;
        ss << utils::HumanReadable{size};
        return ss.str();
    }
}