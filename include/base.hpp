// include freetype, glm, and standard library files
#pragma once
#ifndef KUBICS_CONSOLE_BASE_H
#define KUBICS_CONSOLE_BASE_H
#define KUBICS_CONSOLE

#include <stdint.h>
#include <string>

#ifndef _VC_RUNTIME // not using MSVC
typedef unsigned int size_t;
#endif // _VC_RUNTIME

// kubics-console's namespace
namespace kconsole 
{
	void lib_test();
}

#endif // KUBICS_CONSOLE_BASE_H