#include <cstdint>
#include "core/log/PLog.h"
#define PENGINE_ASSERT(condition, ...)				\
	{												\
		if(!(condition))							\
		{											\
			PLOGE("Assertion Failed : {0}", __VA_ARGS__); \
			__debugbreak();                              				\
		}											\
	}		

#define BIT(x) (1 << x)

#define NO_COPYABLE(TypeName) \
	TypeName(const TypeName &) = delete;   \
	TypeName(TypeName &&) = delete;	\
	TypeName& operator=(TypeName &&) = delete;	\
	TypeName& operator=(const TypeName &) = delete

#ifdef PEGNINE_ENGINE
	#define PENGINE_API __declspec(dllexport)
#else
#define PENGINE_API __declspec(dllimport)
#endif // PEGNINE_ENGINE

