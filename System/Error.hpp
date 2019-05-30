#pragma once
#include <cstdio>
#include <string>
#include <iostream>

//#include "../System/Config.hpp"
//#include "../System/Globals.hpp"

#if OS == OS_WINDOWS
#ifdef _MSVC_LANG
#include <Windows.h>
#else
#include <windows.h>
#endif
#endif

namespace Error
{
	const void Critical(std::string message);
};
