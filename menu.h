#pragma once

#include <stdint.h>

#if defined(_MSC_VER) // Microsoft Visual C++

#define NOINLINE __declspec(noinline)

#elif defined(__GNUC__) // GCC and Clang

#define NOINLINE __attribute__((noinline))

#else

#define NOINLINE // Default to no attribute if not recognized

#endif

#define showCursor "\033[?25h"
#define hideCursor "\033[?25l"

#ifdef __cplusplus

extern "C"
{

#endif

	typedef void (*menuOptionsPointer)();

	extern void drawMenu(const char*[], const uint64_t, const menuOptionsPointer*, const uint64_t, const char*, const unsigned char);

	extern char* drawDirectoryMenu(const char**, const uint64_t, const uint64_t, const char*, const unsigned char);

	extern uint64_t findTheLongestFileNameLength(const char*);

	extern uint64_t countFilesInFolder(const char*);

	NOINLINE extern char* fillTheTestsMenuOptions(char**, const uint64_t, const char*);

	extern char* convertWideToMultiByte(const wchar_t*);

	extern inline void setFullScreenExclusive(void);

	extern inline void setCursorVisibility(const char*);

	extern inline int PressKey(void);

	extern inline void clearScreen(void);

	extern inline void pressEnterToContinue(void);

#ifdef __cplusplus
}

#endif