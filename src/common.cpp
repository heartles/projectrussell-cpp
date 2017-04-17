#include "common.h"

#ifdef WINDOWS
#include <Windows.h>
#endif

void
Log(const std::string &s)
{
#ifdef WINDOWS
    OutputDebugString(s.c_str());
	OutputDebugString("\n");
#endif
    std::printf("%s\n", s.c_str());
}
