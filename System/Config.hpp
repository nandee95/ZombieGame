#pragma once

#define MIN_CPP_STANDARD 201402L

//determine compiler
#define CP_GCC 0
#define CP_MSVC 1

#ifdef _MSC_VER //MSVC
#define CP CP_MSVC
#else
#define CP CP_GCC
#endif

//Check C++ standard
#ifndef __cplusplus
#error Not C++ compiler!
#endif

#if (__cplusplus < MIN_CPP_STANDARD && CP == CP_GCC) || (_MSVC_LANG < MIN_CPP_STANDARD && CP == CP_MSVC )
#error Required c++ standard is not supported by your compiler!
#endif

//Determine operation system
#define OS_WINDOWS 0
#define OS_LINUX 1
#define OS_MAC 2

#if defined(_WIN32) || defined(__WIN32__)
#define OS OS_WINDOWS
#endif // __WIN32__

#ifdef __linux__
#define OS OS_LINUX
#endif // __linux__

#ifdef __APPLE__
#define OS OS_MAC
#endif // __APPLE__

#ifndef OS
#error Unknown operation system!
#endif // OS

//Exit values
#define EXIT_OK 0
#define EXIT_ERROR 1
