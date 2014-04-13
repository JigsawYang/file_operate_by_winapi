#include "detest.h"
#include "myprocess.h"


TCHAR * splite_path(TCHAR *allpath, TCHAR *path_or_name, int splite_flag)
{
	PRINT_INFO();
	if (PATH_NO_NAME == splite_flag)
	{
		wchar_t a = L'\\';
		int index = wcslen(allpath);
		while (allpath[index] != a)
		{
			index--;
		}
		allpath[index + 1] = L'\0';
		lstrcpy(path_or_name, allpath);
		return path_or_name;
	}

	TCHAR *temp1 = allpath;
	TCHAR *temp2 = NULL;
	int len = wcslen(allpath);
	temp2 = &allpath[len];
	while (--temp2 != allpath)
	{
		if (*temp2 == L'\\')
		{
			temp1 = ++temp2;
			break;
		}
	}
	lstrcpy(path_or_name, temp1);
	return path_or_name;
}

void create_proc(int argc, TCHAR* argv[])
{
	TCHAR temp[MAX_PATH] = { 0 };
	splite_path(argv[0], temp, PATH_NO_NAME);
	TCHAR* buf = (TCHAR*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, BUFSIZE);
	lstrcat(temp, L"re_file_mt.exe");
	__try {
		
		for (int i = 1; i < argc; i += 2)
		{
			RtlCopyMemory(&buf[lstrlen(buf)], argv[i], lstrlen(argv[i]) * 2);
			int a = lstrlen(buf);
			RtlCopyMemory(&buf[lstrlen(buf)], L" ", sizeof(TCHAR));
			RtlCopyMemory(&buf[lstrlen(buf)], argv[i + 1], lstrlen(argv[i + 1]) * 2);
			RtlCopyMemory(&buf[lstrlen(buf)], L" ", sizeof(TCHAR));
			if ((i + 1) % 8 == 0)
			{
				ShellExecute(NULL, L"open", temp, buf, NULL, SW_SHOW);
				RtlZeroMemory(buf, BUFSIZE);
			}
		}
		ShellExecute(NULL, L"open", temp, buf, NULL, SW_SHOW);
		
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		TCHAR num_buffer[32] = { 0 };
		StringCchPrintf(num_buffer, 32, L"异常原因操作失败, 错误码:%d", GetLastError());
		PERROR(num_buffer); PRINTLN;
	}
	if (buf)
	{
		HeapFree(GetProcessHeap(), 0, buf);//放到外面....不然里面可能有情况释放不了
	}
}

int check_prco(void)
{
	PROCESSENTRY32W pe32 = { 0 };
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	BOOL bmore = FALSE;
	if (hsnap == INVALID_HANDLE_VALUE){
		PERROR(L"CreateToolhelp32Snapshot Wrong\n");
		return 0;
	}
	pe32.dwSize = sizeof(pe32);
	bmore = Process32First(hsnap, &pe32);
	if (!bmore)
	{
		PERROR(L"CreateToolhelp32Snapshot Wrong\n");
		return 0;
	}
	do {
		if (!wcscmp(pe32.szExeFile, L"re_file_mt.exe"))
		{
			CloseHandle(hsnap);
			return 0;
		}
	} while ((Process32Next(hsnap, &pe32)));
	if (hsnap)
	{
		CloseHandle(hsnap);
	}
	return 1;
}