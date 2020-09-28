#pragma once

#if !defined(_DEBUG) && !defined(NDEBUG)
#error Specify NDEBUG or _DEBUG
#endif

#define _UNICODE
#define UNICODE

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <sdkddkver.h>
#include <Windows.h>
