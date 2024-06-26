import utils;
#include <functional>
using namespace std;

TEST_CASE("testing removing trailing slashes")
{
    std::string with_slashes = "a/b/c/";
    std::string without_slashes = utils::remove_trailing_slashes(with_slashes);
    CHECK(with_slashes == "a/b/c/");
    CHECK(without_slashes == "a/b/c");
    without_slashes = utils::remove_trailing_slashes(without_slashes);
    CHECK(without_slashes == "a/b/c");
}

namespace folding {
    // https://www.scs.stanford.edu/~dm/blog/param-pack.html
    // https://www.linkedin.com/pulse/smart-tricks-parameter-packs-fold-expressions-rainer-grimm
    // https://devblogs.microsoft.com/oldnewthing/20200625-00/?p=103903

    template<typename T, typename... Args>
    size_t countMatches(function<bool(T)> pred, Args&&... args)
    {
        return (pred(args) + ...);
    }

    template<typename T, typename... Args>
    bool anyMatches(function<bool(T)> pred, Args&&... args)
    {
        return (pred(args) || ...);
    }

    template<typename T, typename... Args>
    bool allMatches(function<bool(T)> pred, Args&&... args)
    {
        return (pred(args) && ...);
    }

    template<typename T, typename... Args>
    int sumMatches(function<bool(T)> pred, Args&&... args)
    {
        T sum{};
        ((pred(args) ? sum += args : T{}), ...);
        return sum;
    }
}

TEST_CASE("Test expansion packs")
{
    using namespace folding;
    {
        // sum only even numbers in the pack
        CHECK(sumMatches(
                  function<bool(int)>{[](int v) -> bool { return v % 2 == 0; }},
                  1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
              == 0 + 2 + 0 + 4 + 0 + 6 + 0 + 8 + 0 + 10);
        CHECK(sumMatches(
                  function<bool(int)>{[](int v) -> bool { return v % 2 == 0; }},
                  1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
              == sumMatches(
                  function<bool(int)>{[](int) -> bool { return true; }},
                  0, 2, 0, 4, 0, 6, 0, 8, 0, 10));
    }
    {
        // check if we have 7 in the pack
        CHECK(anyMatches(
                  function<bool(int)>([](auto v) { return v == 7; }),
                  1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
              == true);
    }
    {
        // check if we have 17 in the pack
        auto pred = [](auto v) { return v == 17; };
        auto matches = anyMatches(
            function<bool(int)>(pred),
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
        CHECK(matches != true);
    }
    {
        auto pred = [](auto v) { return v < 42; };
        auto matches = allMatches(
            function<bool(int)>(pred),
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
        CHECK(matches == true);
    }
    {
        auto pred = [](auto v) { return v > 42; };
        auto matches = allMatches(
            function<bool(int)>(pred),
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
        CHECK(matches != true);
    }
}