module;

#pragma region Header
export module main;
import :start;
export import :concepts;
import logging;
import utils;
using namespace std;
#pragma endregion

namespace app {
    export void intro()
    {
        // This function's purpose is only to provide a callable instance so that
        // object file could be detected by the compiler as containing something.
        // Other functions in this module are actually all template functions
        // and  because of this, compiler will not let `doctest` detect any tests
        // inside this library.
        logging::info("Starting application...");
    }
    export template<typename T>
        requires creates_view<T>
    int main(T create_view)
    {
        try {
            if (auto result = ::details::start(create_view); result)
                return *result;
            else
                logging::error("Error: {}", result.error());
        } catch (const system_error& e) {
            logging::critical(R"(Unhandled system exception: {})", utils::errmsg(e.code()));
        } catch (const runtime_error& e) {
            logging::critical(R"(Unhandled runtime exception: {})", e.what());
        } catch (const exception& e) {
            logging::critical(R"(Unhandled "{}" exception: {})", typeid(e).name(), e.what());
        } catch (...) {
            logging::critical("Caught unexpected exception of unknown type");
        }
        return 1;
    }
}