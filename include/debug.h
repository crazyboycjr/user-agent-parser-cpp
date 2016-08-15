#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cassert>

#ifdef TRACE
#define trace(...) __f(#__VA_ARGS__, __VA_ARGS__)
template <typename Arg1>
void __f(const char *name, Arg1 &&arg1) {
	std::cerr << name << " : "  << arg1 << std::endl;
}

template <typename Arg1, typename... Args>
void __f(const char *names, Arg1 &&arg1, Args &&... args) {
	const char *comma = strchr(names + 1, ','); // if we trace(f(3,4), g(2,5))? FIXME
	std::cerr.write(names, comma - names) << " : " << arg1 << " | ";
	__f(comma + 1, args...);
}
#else
#define trace(...)
#endif

#define Assert(cond, ...) \
	do { \
		if(!(cond)) { \
			fflush(stdout); \
			fprintf(stderr, "\33[1;31m"); \
			trace(__VA_ARGS__); \
			fprintf(stderr, "\33[0m\n"); \
			assert(cond); \
		} \
	} while(0)

#define Try(cond) \
	do { \
		if (!(cond)) { \
			return -1; \
		} \
	} while (0)

#endif
