#pragma once

#include "Saffron/Core/Macros.h"

#ifdef SE_DEBUG
#define SE_ENABLE_ASSERTS
#endif

#ifdef SE_ENABLE_ASSERTS

#define SE_ASSERT_NO_MESSAGE(condition) { if(!(condition)) { SE_ERROR("Assertion Failed"); SE_DEBUGBREAK(); } }
#define SE_ASSERT_MESSAGE(condition, ...) { if(!(condition)) { SE_ERROR("Assertion Failed: {0}", __VA_ARGS__); SE_DEBUGBREAK(); } }

#define SE_ASSERT_RESOLVE(arg1, arg2, macro, ...) macro
#define SE_GET_ASSERT_MACRO(...) SE_EXPAND_VARGS(SE_ASSERT_RESOLVE(__VA_ARGS__, SE_ASSERT_MESSAGE, SE_ASSERT_NO_MESSAGE))

#define SE_ASSERT(...) SE_EXPAND_VARGS( SE_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#define SE_CORE_ASSERT(...) SE_EXPAND_VARGS( SE_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )

#else

#define SE_ASSERT_NO_MESSAGE(condition)
#define SE_ASSERT_MESSAGE(condition, ...)

#define SE_ASSERT_RESOLVE(arg1, arg2, macro, ...)
#define SE_GET_ASSERT_MACRO(...)

#define SE_ASSERT(...)
#define SE_CORE_ASSERT(...)

#endif
