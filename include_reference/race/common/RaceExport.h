#ifndef __EXPORTS_H_
#define __EXPORTS_H_

#if defined(_WIN32)
#define EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
#define EXPORT __attribute__((visibility("default")))
#elif defined(SWIG)
#define EXPORT
#else
#error "Unknown Platform"
#endif

#endif
