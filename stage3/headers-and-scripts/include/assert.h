#ifndef ASSERT_H
#define ASSERT_H

#include <stdlib.h>

#if defined(__cplusplus) && __cplusplus >= 202302L
	#define _assert_internal(CONDITION, MESSAGE_PREFIX, MESSAGE, MESSAGE_SUFFIX) \
		do \
		{ \
			if (!(CONDITION)) \
			{ \
				/* TODO: What do we do about the SubCPU? */ \
				ClownMDSDK::MainCPU::Debug::PrintLine(__PRETTY_FUNCTION__, " at " __FILE__ ":", __LINE__); \
				ClownMDSDK::MainCPU::Debug::PrintLine("    Assertion '" #CONDITION "' failed" MESSAGE_PREFIX MESSAGE MESSAGE_SUFFIX "."); \
				abort(); \
			} \
		} while(0)
#else
	#define _assert_internal(CONDITION, MESSAGE_PREFIX, MESSAGE, MESSAGE_SUFFIX) \
		do \
		{ \
			if (!(CONDITION)) \
				abort(); \
		} while(0)
#endif

/* TODO: Proper assert handling. */
#ifdef NDEBUG
	#define assert(CONDITION) ((void)0)
#else
	#define assert(CONDITION) _assert_internal(CONDITION, "", "", "")
#endif

#define _assertm(CONDITION, MESSAGE) _assert_internal(CONDITION, " with message '", MESSAGE, "'")

#include <clownmdsdk.h>

#endif /* ASSERT_H */
