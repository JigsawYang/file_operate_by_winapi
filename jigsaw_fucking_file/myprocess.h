#pragma once

#include <Windows.h>
#include <TlHelp32.h>

#define PATH_NO_NAME (1)
#define BUFSIZE ((MAX_PATH * argc * 2 + 1) * sizeof(TCHAR))

extern void create_proc(int argc, TCHAR* argv[]);

extern int check_prco(void);