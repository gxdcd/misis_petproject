#pragma once

#ifndef NO_AUTHOR_WARNING
// https://stackoverflow.com/questions/2670816/how-can-i-use-the-compile-time-constant-line-in-a-string
// https://forums.codeguru.com/showthread.php?215151-__LINE__-MACRO-to-string-literal
#    define __LINESTR____(LN)     #LN
#    define __LINESTR___(LN)      __LINESTR____(LN)
#    define __LINESTR__           __LINESTR___(__LINE__)
#    define __SRCLOC__            __FILE__ ":" __LINESTR__
#    define __AUTHOR_WARNING__(x) "Warning (dev): " x " --> " __SRCLOC__
#    define AUTHOR_WARNING(x)     message(__AUTHOR_WARNING__(x))
#else
#    define AUTHOR_WARNING(x)
#endif