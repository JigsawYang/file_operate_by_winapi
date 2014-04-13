#pragma once
#include <Windows.h>

#define LIST_ALLOC (SEARCH_LIST *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(SEARCH_LIST))
#define LIST_FREE(HPOINT) HeapFree(GetProcessHeap(), 0, HPOINT)
