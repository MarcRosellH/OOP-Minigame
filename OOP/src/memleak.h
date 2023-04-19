#pragma once
#ifndef __MEMLEAK_H__
#define __MEMLEAK_H__

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#ifdef _DEBUG
	#define DBG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
	#define DBG_NEW new
#endif
#endif // !__MEMLEAK_H__