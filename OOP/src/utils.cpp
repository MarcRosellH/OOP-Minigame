#pragma once

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