#ifndef ASSERT_H
#define ASSERT_H

/* TODO: Proper assert handling. */
#ifdef NDEBUG
#define assert(x) ((void)0)
#else
#define assert(x) \
	do \
	{ \
		if (!(x)) \
		{ \
			/* TODO: What do we do about the SubCPU? */ \
			ClownMDSDK::MainCPU::Debug::PrintLine(__func__, "() at " __FILE__ ":", __LINE__); \
			ClownMDSDK::MainCPU::Debug::PrintLine("    Assertion '" #x "' failed."); \
			asm("illegal"); \
		} \
	} while(0)
#endif

#define assertm(x, message) assert((x) && message)

#include <clownmdsdk.h>

#endif /* ASSERT_H */
