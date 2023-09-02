#pragma once
#ifndef __UTILS_H__
#define __UITLS_H__

#include <stdio.h>
#include <Windows.h>
#include <assert.h>
#include <string>

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

#define ARRAY_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))

std::string read_text_file(const char* _file_path);

std::string get_directory_part(std::string _path);

std::string get_filepath_name(std::string _file_path);

std::string make_path(std::string _dir, std::string _filename);


#endif // !__UTILS_H__