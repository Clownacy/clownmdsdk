#ifndef ASSERT_H
#define ASSERT_H

/* TODO: Proper assert handling. */
#ifdef NDEBUG
#define assert(x)
#else
#define assert(x) do {if (!(x)) asm("illegal");} while(0)
#endif

#endif /* ASSERT_H */
