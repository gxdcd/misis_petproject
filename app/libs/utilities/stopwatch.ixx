export module stopwatch;

using namespace std;

/// Milliseconds
export struct Stopwatch {
    using value_type = chrono::milliseconds::rep;

    Stopwatch():
        started{chrono::high_resolution_clock::now()}
    {
    }

    /// Milliseconds
    auto unit() const { return "ms"; }

    /// Milliseconds
    value_type elapsed() const
    {
        auto finished = chrono::high_resolution_clock::now();
        return chrono::duration_cast<chrono::milliseconds>(finished - started).count();
    }

    /// Milliseconds
    auto elapsed_units()
    {
        return format("{} {}", elapsed(), unit());
    }

    void reset() { started = chrono::high_resolution_clock::now(); }

private:
    chrono::time_point<chrono::high_resolution_clock> started;
};

/// Nanoseconds
export struct NanoStopwatch {
    using value_type = chrono::nanoseconds::rep;

    NanoStopwatch():
        started{chrono::high_resolution_clock::now()}
    {
    }

    /// Nanoseconds
    auto unit() const { return "ns"; }

    /// Nanoseconds
    value_type elapsed() const
    {
        auto finished = chrono::high_resolution_clock::now();
        return chrono::duration_cast<chrono::nanoseconds>(finished - started).count();
    }

    /// Nanoseconds
    auto elapsed_units()
    {
        return format("{} {}", elapsed(), unit());
    }

    void reset() { started = chrono::high_resolution_clock::now(); }

private:
    chrono::time_point<chrono::high_resolution_clock> started;
};