#pragma once

#if defined(WIN32)
#	include <Windows.h>
#	define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <stdexcept>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <set>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <array>
#include <cstdarg>
#include <unordered_map>

#define GRT_CHECK(x, msg) if(!(x)){ throw std::runtime_error(msg);}

#define SAFE_DELETE(ptr)		if(ptr) { delete ptr; ptr = nullptr; }
#define SAFE_DELETE_ARRAY(ptr)	if(ptr) { delete[] ptr; ptr = nullptr; }


