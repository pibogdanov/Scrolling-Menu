#include "menu.h"

#include <stdio.h>
#include <windows.h>
#include <conio.h>

#define UpArrowKey 72
#define WKey 'w'
#define DownArrowKey 80
#define SKey 's'
#define EnterKey 13
#define EscapeKey 27
#define MoveUp(keyPressed)   ( (keyPressed == UpArrowKey)    ||  (((keyPressed) | 32) == WKey) )
#define MoveDown(keyPressed) ( (keyPressed == DownArrowKey)  ||  (((keyPressed) | 32) == SKey) )

static inline void printOptions(const char*[], const uint64_t, const char*, const uint64_t);

static inline unsigned char isValidKeyPressOnMenu(const int, const uint64_t, const unsigned char);

static inline void handleFindError(void);

void drawMenu(const char *menuOptions[], const uint64_t optionsCount, const menuOptionsPointer *menuOptionsPointers, const uint64_t menuOptionsPointersCount, const char *menuTypeDescription, const unsigned char isMainMenu)
{
	uint64_t indexOfArrow = 0;

	int keyPressed = 0;

	while (1)
	{
		printOptions(menuOptions, optionsCount, menuTypeDescription, indexOfArrow);

		while (!isValidKeyPressOnMenu(keyPressed = PressKey(), optionsCount, isMainMenu));

		if (MoveUp(keyPressed))
		{
			if (indexOfArrow == 0)
			{
				indexOfArrow = optionsCount - 1;

				continue;
			}

			indexOfArrow--;
		}

		else if (MoveDown(keyPressed))
		{
			if ( indexOfArrow == (optionsCount - 1) )
			{
				indexOfArrow = 0;

				continue;
			}

			indexOfArrow++;
		}

		else if (keyPressed == EnterKey) // Enter / Return
		{
			clearScreen();

			if ( isMainMenu && (indexOfArrow == (optionsCount - 1)) && (menuOptionsPointersCount < optionsCount) ) break;

			menuOptionsPointers[indexOfArrow]();
		}

		else if ( (keyPressed == EscapeKey) && !isMainMenu ) break; // Escape
	}
}

char *drawDirectoryMenu(const char **menuOptions, const uint64_t optionsCount, const uint64_t menuOptionsPointersCount, const char *menuTypeDescription, const unsigned char isMainMenu)
{
	uint64_t indexOfArrow = 0;

	int keyPressed = 0;

	while (1)
	{
		printOptions(menuOptions, optionsCount, menuTypeDescription, indexOfArrow);

		while (!isValidKeyPressOnMenu(keyPressed = PressKey(), optionsCount, isMainMenu));

		if (MoveUp(keyPressed))
		{
			if (indexOfArrow == 0)
			{
				indexOfArrow = optionsCount - 1;

				continue;
			}

			indexOfArrow--;
		}

		else if (MoveDown(keyPressed))
		{
			if ( indexOfArrow == (optionsCount - 1) )
			{
				indexOfArrow = 0;

				continue;
			}

			indexOfArrow++;
		}

		else if (keyPressed == EnterKey) // Enter / Return
		{
			clearScreen();

			if ( isMainMenu && (indexOfArrow == (optionsCount - 1)) && (menuOptionsPointersCount < optionsCount) ) break;

			return menuOptions[indexOfArrow];
		}

		else if ( (keyPressed == EscapeKey) && !isMainMenu ) return NULL; // Escape
	}
}

static inline void printOptions(const char *menuOptionsAsStrings[], const uint64_t optionsCount, const char *menuTypeDescription, const uint64_t indexOfArrow)
{
	clearScreen();

	fputs("\n     ", stdout);

	fputs(menuTypeDescription, stdout);
	fputc('\n', stdout);
	for (uint64_t i = 0; i < optionsCount; i++)
	{
		fprintf(stdout, "\n%s%s", i == indexOfArrow ? " \033[0;31m->  \033[0m" : "     ", menuOptionsAsStrings[i]);
	}
}

uint64_t findTheLongestFileNameLength(const char *directoryPath)
{
	WIN32_FIND_DATA findFileData;

	HANDLE handleFind = FindFirstFileA(directoryPath, &findFileData);

	if (handleFind == INVALID_HANDLE_VALUE)
	{
		handleFindError();

		pressEnterToContinue();

		return 0;
	}

	uint64_t currentFileNameLength = 0;

	uint64_t longestFileNameLength = 0;

	char *currentFileName = NULL;

	do
	{
		if ( (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
		{
			currentFileName = convertWideToMultiByte(findFileData.cFileName);

			if (currentFileName == NULL)
			{
				pressEnterToContinue();

				break;
			}

			currentFileNameLength = strlen(currentFileName);

			free(currentFileName);

			if (currentFileNameLength > longestFileNameLength) longestFileNameLength = currentFileNameLength;
		}
	} while (FindNextFileW(handleFind, &findFileData) != 0);

	FindClose(handleFind);

	return longestFileNameLength;
}

uint64_t countFilesInFolder(const char *directoryPath)
{
	WIN32_FIND_DATA findFileData;

	HANDLE handleFind = FindFirstFileA(directoryPath, &findFileData);

	if (handleFind == INVALID_HANDLE_VALUE)
	{
		handleFindError();

		pressEnterToContinue();

		return 0;
	}

	uint64_t fileCount = 0;

	do
	{
		if ( (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 ) fileCount++;
	} while (FindNextFileW(handleFind, &findFileData) != 0);

	FindClose(handleFind);

	if (fileCount == 0)
	{
		fputs("No files were found!\n\n", stdout);

		pressEnterToContinue();
	}

	return fileCount;
}

char *fillTheTestsMenuOptions(char **testsMenuOptions, const uint64_t optionsCount, const char *directoryPath)
{
	WIN32_FIND_DATA findFileData;

	HANDLE handleFind = FindFirstFileA(directoryPath, &findFileData);

	if (handleFind == INVALID_HANDLE_VALUE)
	{
		handleFindError();

		pressEnterToContinue();

		return NULL;
	}

	uint64_t currentOptionIndex = 0;

	do
	{
		if ( (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
		{
			testsMenuOptions[currentOptionIndex] = convertWideToMultiByte(findFileData.cFileName);

			if (testsMenuOptions[currentOptionIndex++] == NULL)
			{
				FindClose(handleFind);

				pressEnterToContinue();

				return NULL;
			}
		}
	} while ( (FindNextFileW(handleFind, &findFileData) != 0) && (currentOptionIndex < optionsCount) );

	FindClose(handleFind);
}

char *convertWideToMultiByte(const wchar_t *wideString)
{
	uint64_t currentBufferSize = WideCharToMultiByte(CP_ACP, 0, wideString, -1, NULL, 0, NULL, NULL);

	if (currentBufferSize == 0)
	{
		fputs("No files were found!\n\n", stderr);

		return NULL;
	}

	char *multiByteString = (char*)malloc(currentBufferSize * sizeof(char));

	if (multiByteString == NULL)
	{
		fputs("Memory allocation failed for a file name!\n\n", stderr);

		return NULL;
	}

	WideCharToMultiByte(CP_ACP, 0, wideString, -1, multiByteString, currentBufferSize, NULL, NULL);

	return multiByteString;
}

inline void setFullScreenExclusive(void)
{
	keybd_event(VK_MENU, 0x38, 0, 0);
	keybd_event(VK_RETURN, 0x1c, 0, 0);
	keybd_event(VK_RETURN, 0x1c, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
}

inline void setCursorVisibility(const char *visibilityCommand) { fputs(visibilityCommand, stdout); }

static inline unsigned char isValidKeyPressOnMenu(const int keyPressed, const uint64_t optionsCount, const unsigned char isMainMenu)
{
	return	( MoveUp(keyPressed)		&&		(optionsCount > 1) )	||
			( MoveDown(keyPressed)		&&		(optionsCount > 1) )	||
			( (keyPressed == EnterKey))									||
			( (keyPressed == EscapeKey) && !isMainMenu					);
}

inline int PressKey(void) { return _getch(); }

inline void clearScreen(void) { fputs("\033[H\033[J", stdout); }

inline void pressEnterToContinue(void)
{
	fputs("\n\nPress \"ENTER \\ RETURN\" to continue!", stdout);

	while (PressKey() != EnterKey);
}

static inline void handleFindError(void)
{
	switch (GetLastError())
	{
		case ERROR_FILE_NOT_FOUND:	fputs("ERROR_FILE_NOT_FOUND: No files match the wildcard!\n\n", stderr); break;

		case ERROR_PATH_NOT_FOUND:	fputs("ERROR_PATH_NOT_FOUND: The folder path is invalid!\n\n",	stderr); break;

		case ERROR_ACCESS_DENIED:	fputs("ERROR_ACCESS_DENIED: Permission denied!\n\n",			stderr); break;

		default:					fputs("Unknown error occurred (%llu)!\n\n",						stderr); break;
	}
}