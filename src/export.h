#ifndef __EXPORT_H_
#define __EXPORT_H_

#ifdef _WIN32
    #ifdef MAKEDLL
    #   define NKAR_EXPORT __declspec(dllexport)
    #else
    #   define NKAR_EXPORT __declspec(dllimport)
    #endif
#else
    #define NKAR_EXPORT
#endif

#endif // __EXPORT_H