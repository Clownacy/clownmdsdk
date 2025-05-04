#ifndef ASSERT_H
#define ASSERT_H

/* TODO: Proper assert handling. */
#ifdef NDEBUG
#define assert(x)
#else
#define assert(x) \
	do \
	{ \
		if (!(x)) \
		{ \
			/* TODO: What do we do about the SubCPU? */ \
			ClownMDSDK::MainCPU::Debug::PrintLine(__func__, " at " __FILE__ ":", __LINE__, " - Assertion '" #x "' failed."); \
			asm("illegal"); \
		} \
	} while(0)
#endif

#include <clownmdsdk.h>

#endif /* ASSERT_H */
