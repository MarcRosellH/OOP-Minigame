#pragma once

#include "memleak.h"
#include "utils.h"

void log(const char _file[], int _line, const char* _format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list args;

	va_start(args, _format);
	vsprintf_s(tmp_string, 4096, _format, args);
	va_end(args);

	sprintf_s(tmp_string2, 4096, "\n%s(%d): %s", _file, _line, tmp_string);
	OutputDebugString(tmp_string2);
}

std::string read_text_file(const char* _file_path)
{
	std::string ret;

	FILE* file = fopen(_file_path, "rb");

	if (file)
	{
		fseek(file, 0, SEEK_END);
		unsigned int length = ftell(file);
		fseek(file, 0, SEEK_SET);
		char* content = DBG_NEW char[length];
		fread(content, sizeof(char), length, file);
		ret = content;
		fclose(file);
		RELEASE_ARRAY(content);
	}
	else
	{
		LOG("fopen() function failed reading file %s", _file_path);
	}

	return ret;
}