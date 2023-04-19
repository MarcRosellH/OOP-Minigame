#pragma once
#ifndef __UTILS_H__
#define __UITLS_H__

#include <stdio.h>
#include <Windows.h>
#include <assert.h>

#define RELEASE(x)\
	{\
		if(x != nullptr)\
		{\
			delete x;\
			x = nullptr;\
		}\
	}

#define RELEASE_ARRAY(x)\
	{\
		if(x != nullptr)\
		{\
			delete[] x;\
			x = nullptr;\
		}\
	}

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);
void log(const char _file[], int _line, const char* _format, ...);

#define MIN(a,b) ((a)<(b)) ? (a) : (b)
#define MAX(a,b) ((a)>(b)) ? (a) : (b)

#define ASSERT(condition, message) assert((condition) && message)

#endif // !__UTILS_H__