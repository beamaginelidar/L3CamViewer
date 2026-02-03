#ifndef LIBL3CAM_GLOBAL_H
#define LIBL3CAM_GLOBAL_H

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#  define BMG_DECL_EXPORT __declspec(dllexport)
#  define BMG_DECL_IMPORT __declspec(dllimport)
#else
#  define BMG_DECL_EXPORT     __attribute__((visibility("default")))
#  define BMG_DECL_IMPORT     __attribute__((visibility("default")))
#endif

#if defined(LIBL3CAM_LIBRARY)
#  define LIBL3CAM_EXPORT BMG_DECL_EXPORT
#else
#  define LIBL3CAM_EXPORT BMG_DECL_IMPORT
#endif

#endif // LIBL3CAM_GLOBAL_H
