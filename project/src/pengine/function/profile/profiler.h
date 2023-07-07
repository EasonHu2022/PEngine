#pragma once
#ifdef PEGNINE_WINDOWS
#define TRACY_CALLSTACK 1
#endif // PEGNINE_WINDOWS
#include "tracy/Tracy.hpp"
#	define PROFILE_SCOPE_WITHNAME(name) ZoneScopedN(name)
#	define PROFILE_SCOPE_WITHCOLOR(color) ZoneScopedC(color)
#	define PROFILE_FUNCTION() ZoneScoped
#	define PROFILE_FRAMEMARKER() FrameMark
