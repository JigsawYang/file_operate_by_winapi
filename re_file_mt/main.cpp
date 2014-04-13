#include "detest.h"
#include "allocdefine.h"
#include "file_utility.h"


int wmain(int argc, TCHAR *argv[])
{
	FILE_OPERATOR_ARGS fopargs[MAX_TARGET_FILE] = { 0 };
	int num = init_args(argc, argv, fopargs);	//先初始化
	if (!num) {
		return 0;
	}

	HANDLE *handle_arr = (HANDLE *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(HANDLE)* num);

	InitializeCriticalSection(&cs);
	for (int i = 0; i < num; i++) {
		handle_arr[i] = start_task(file_operator, &fopargs[i]);
	}
	DWORD ret = WaitForMultipleObjects(num, handle_arr, TRUE, 11115000);	//无限等不合理,要有超时
	DeleteCriticalSection(&cs);

	if (ret) {
		GetLastError(); //超时返回,ret不为0,则某个线程必定出了什么错.
	}

	for (int i = 0; i < num; i++) {
		if (handle_arr[i]) {
			CloseHandle(handle_arr[i]);
		}
	}
	LIST_FREE(handle_arr);
	system("pause");
	return 0;
}