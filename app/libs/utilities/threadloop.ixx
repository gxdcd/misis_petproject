export module threadloop;

import logging;
using namespace std;

export struct ThreadLoop {
    template<typename Callable>
        requires invocable<Callable>
    ThreadLoop(string_view id, Callable&& callable)
    {
        auto id_str = string(id);
        thread_stop_token = thread_stop_source.get_token();
        my_thread = jthread([=] {
            while (true) {
                if (!thread_stop_token.stop_requested())
                    logging::debug("{} thread awaiting signal", id_str);
                thread_signal.acquire();
                if (thread_stop_token.stop_requested()) {
                    logging::debug("{} thread requested to stop", id_str);
                    return;
                }
                callable();
            }
        });
    }

    void start()
    {
        thread_signal.release();
    }

    void stop()
    {
        thread_stop_source.request_stop();
        thread_signal.release();
    }

    ~ThreadLoop()
    {
        stop();
    }

    bool stop_requested() const
    {
        return thread_stop_token.stop_requested();
    }

private:
    binary_semaphore thread_signal{0};
    stop_source thread_stop_source;
    stop_token thread_stop_token;
    jthread my_thread;
};