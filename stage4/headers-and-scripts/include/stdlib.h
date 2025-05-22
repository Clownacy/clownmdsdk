#ifndef STDLIB_H
#define STDLIB_H

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((noreturn)) __attribute__((always_inline)) static int abs(const int i)
{
	return i < 0 ? -i : i;
}

__attribute__((noreturn)) __attribute__((always_inline)) static void abort(void)
{
	for (;;);
}

#ifdef __cplusplus
}
#endif

#endif /* STDLIB_H */
