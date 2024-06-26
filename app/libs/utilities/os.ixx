export module os;

#if defined(__linux__) || defined(__unix__)
#    error "Not implemented."
#elif defined(_WIN32)
export import win;
#else
#    error "Not implemented."
#endif