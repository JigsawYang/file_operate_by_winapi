//This header is for file operate
/*
** This file without any header of CRT
** All the header is winapi
** author      Chris Yang
** guide man   Jerry MaX
** data        3/16/2014
*/
#pragma once
#include <Windows.h>

#define FAIL (0)
#define SUCCEED (1)

#define MAX_TARGET_FILE 8

typedef TCHAR FILE_PATH[MAX_PATH];
typedef struct _SEARCH_LIST
{
	FILE_PATH result;
	_SEARCH_LIST *next;
} SEARCH_LIST;
typedef SEARCH_LIST SEARCH_NODE;

typedef int(*FILE_OP)(LPWSTR file, PVOID other);

typedef struct _TARGET_FILE
{
	TCHAR dir[MAX_PATH];
	TCHAR name[MAX_PATH];
}TARGET_FILE;

typedef struct FILE_OPERATOR_ARGS
{
	TARGET_FILE target;
	FILE_OP file_op;
	LPVOID other;
} FILE_OPERATOR_ARGS;

extern CRITICAL_SECTION cs;

extern SEARCH_LIST *list_file(TARGET_FILE *target, SEARCH_LIST *slist);

extern int create_log(LPWSTR log_dir);

extern int write_log(LPWSTR log_file, LPWSTR buffer);

extern TCHAR * splite_path(TCHAR *allpath, TCHAR *path_or_name, int splite_flag);

extern int delete_file(LPWSTR delete_path, PVOID other);

extern int append_suffix(LPWSTR file, PVOID suffix);

extern DWORD WINAPI file_operator(PVOID lpparam);

extern TCHAR *cut_path(TCHAR *target_path, TCHAR *dir); 

extern HANDLE start_task(LPTHREAD_START_ROUTINE file_opt, FILE_OPERATOR_ARGS *file_op_args);

extern int init_args(int argc, TCHAR* argv[], FILE_OPERATOR_ARGS* args);
