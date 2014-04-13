#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <Windows.h>
#include <Strsafe.h>

#ifdef  UNICODE
	#define PRINT(WSTR) WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), WSTR, wcslen(WSTR), NULL, NULL)
	#define PERROR(WSTR) WriteConsoleW(GetStdHandle(STD_ERROR_HANDLE), WSTR, wcslen(WSTR), NULL, NULL)
	#define PRINTLN WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), L"\n", wcslen(L"\n"), NULL, NULL)
#else
	#define PRINT(STR) WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), STR, strlen(STR), NULL, NULL)
	#define PERROR(STR) WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), STR, strlen(STR), NULL, NULL)
	#define PRINTLN WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), "\n", strlen("\n"), NULL, NULL)
#endif

#ifdef _DEBUG
	#define PRINT_INFO()  \
	{ \
		fprintf(stderr, "%s (%d) {%s}\n", __FILE__, __LINE__, __FUNCTION__); \
	}
#else
	#define NOP 
	#define PRINT_INFO() NOP
#endif

