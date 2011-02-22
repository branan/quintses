#ifndef QNT_CORE_UTIL_ATOMIC_HPP
#define QNT_CORE_UTIL_ATOMIC_HPP

#if defined(__GNUC__)
#define fetchAndIncrement(var) __sync_fetch_and_add(&var, 1)
#elif defined(_MSC_VER)
#include <windows.h>
#define fetchAndIncrement(var) InterlockedExchangeAdd(&var, 1)
#else
#error "Sorry, don't know how to do an atomic increment on this compiler"
#endif

#endif // QNT_CORE_UTIL_ATOMIC_HPP
