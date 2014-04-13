#include "detest.h"
#include "allocdefine.h"
#include "file_utility.h"

#define MAX_STACK_DEPTH (32)
#define FILE_NAME (1)
#define PATH_NO_NAME (2)

CRITICAL_SECTION cs;


//把路径中遇到的第2个 '//'之前的分离出来
TCHAR *cut_path(TCHAR *target_path, TCHAR *dir)
{
	if (0 == target_path)
	{
		return NULL;
	}
	TCHAR temp2[100] = { 0 };
	lstrcpy(temp2, target_path);
	TCHAR *temp = temp2;
	TCHAR *temp1 = temp;
	int flag = 0;
	while (*temp != 0)
	{
		if (3 == flag)
		{

			break;
		}
		if (*temp == L'\\')
		{
			flag++;
		}
		temp++;
	}
	*temp = L'\0';
	lstrcpy(dir, temp1);
	return dir;
}



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

SEARCH_LIST *create_list(void)
{
	PRINT_INFO();
	SEARCH_LIST *list = LIST_ALLOC;
	if (!list)
	{
		PRINT(L"申请链表失败, 内存不足");
		PRINTLN;
		return NULL;
	}
	list->next = NULL;
	return list;
}

int add_path(SEARCH_LIST *slist, TCHAR *path)
{
	SEARCH_NODE *new_node = NULL;
	SEARCH_NODE *temp = NULL;
	new_node = LIST_ALLOC;
	if (!new_node)
	{
		PRINT(L"申请内存失败");PRINTLN;
		return FAIL;
	}
	lstrcpy(new_node->result, path);
	new_node->next = NULL;
	if (NULL == slist->next)
	{
		slist->next = new_node;
		return SUCCEED;
	}
	temp = slist;
	while (temp->next)
	{
		temp = temp->next;
	}
	temp->next = new_node;
	return SUCCEED;
}

void destroy_list(SEARCH_LIST *slist)
{
	PRINT_INFO();
	SEARCH_NODE *temp1 = slist;
	while (temp1)
	{
		SEARCH_NODE *temp2 = temp1->next;
		LIST_FREE(temp1);
		temp1 = temp2;
	}
	
}

SEARCH_LIST *list_file(TARGET_FILE *target, SEARCH_LIST *slist)
{
	PRINT_INFO();
	static int depth = 0;
	if (depth > MAX_STACK_DEPTH)
	{
		//PRINT(L"异常: 超过最大目录层数");
		int ret = add_path(slist, L"over32");
		if (SUCCEED == ret)
		{
			return slist;
		}
		return NULL;
	}
	TCHAR wildcard[MAX_PATH] = { 0 };
	wsprintf(wildcard, L"%s%s", target->dir, L"\\*");//保存的路径
	TARGET_FILE current_target = { 0 };//加了文件名字后的路径
	RtlCopyMemory(&current_target, target, sizeof(current_target));

	WIN32_FIND_DATA file_data = { 0 };//初始化
	HANDLE hfile = FindFirstFile(wildcard, &file_data);//寻找文件

	if (INVALID_HANDLE_VALUE == hfile)
	{
		TCHAR strcode[32] = { 0 };
		StringCchPrintf(strcode, 32, L"异常原因操作失败, 错误码:%d", GetLastError());
		PERROR(strcode);PRINTLN;
		int ret = add_path(slist, L"wrong");
		if (SUCCEED == ret)
		{
			FindClose(hfile);
			return slist;
		}
		FindClose(hfile);
		return NULL;
	}

	do{
		wsprintf(current_target.dir, L"%s\\%s", target->dir, file_data.cFileName);

		if (lstrcmp(file_data.cFileName, L".") == 0)//过滤掉 "." 目录自己
		{
			continue;
		}
		if (lstrcmp(file_data.cFileName, L"..") == 0)//过滤掉".."上级目录
		{
			continue;
		}

		//PRINT(file_data.cFileName);
		//PRINTLN;

		if (FILE_ATTRIBUTE_DIRECTORY == file_data.dwFileAttributes)//如果是目录.
		{
			depth++;
			list_file(&current_target, slist);

		}
		if (lstrcmp(file_data.cFileName, current_target.name) == 0)
		{
			int ret = add_path(slist, current_target.dir);
			if (FAIL == ret)
			{
				return NULL;
			}
		}
	} while (FindNextFile(hfile, &file_data));//寻找下一个文件
	FindClose(hfile);
	return slist;
}

int create_log(LPWSTR log_dir)
{
	PRINT_INFO();
	if (wcslen(log_dir) > MAX_PATH) {
		return 0;
	}
	BOOL bret = CreateDirectory(log_dir, NULL);
	if (bret) {
		PRINT(L"目录创建成功");
		PRINTLN;
	}
	return SUCCEED;
}

int write_log(LPWSTR log_file, LPWSTR buffer)
{
	PRINT_INFO();
	if (MAX_PATH < wcslen(log_file))//检查是否过长
	{
		PRINT(L"给出的路径超过最大路径, 建立失败.");PRINTLN;
		return FAIL;
	}

	EnterCriticalSection(&cs);
	HANDLE hopen_file = CreateFile(log_file, GENERIC_WRITE, 0, NULL,
								OPEN_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
	if (INVALID_HANDLE_VALUE == hopen_file)
	{
		TCHAR num_buffer[32] = { 0 };//这里写11  因为int 最大有10个数字..
		if (ERROR_ALREADY_EXISTS != GetLastError())
		{
			StringCchPrintf(num_buffer, 32, L"异常原因操作失败, 错误码:%d", GetLastError());
			PERROR(num_buffer);PRINTLN;
			CloseHandle(hopen_file);
			return FAIL;
		}
	}
	else
	{
		WORD head = 0xfeff;//unicode头,告诉编辑器.
		DWORD dwWritten = 0;
		WriteFile(hopen_file, &head, 2, &dwWritten, NULL);
		SetFilePointer(hopen_file, 0, NULL, FILE_END);//设置文件指针到文件尾部
		BOOL wret = WriteFile(hopen_file, buffer, wcslen(buffer) * sizeof(TCHAR), &dwWritten, NULL);//写入内容
		if (wret)
		{
			PRINT(L"记录成功.");PRINTLN;
		}
	}
	CloseHandle(hopen_file);
	LeaveCriticalSection(&cs);
	return SUCCEED;
}

int delete_file(LPWSTR delete_path, PVOID other)
{
	BOOL bret = DeleteFile(delete_path);
	if (!bret) {
		TCHAR buffer[32] = { 0 };//int最大也不超过32位数字
		StringCchPrintf(buffer, 32, L"异常原因操作失败, 错误码:%d", GetLastError());
		PERROR(buffer); PRINTLN;
		return FAIL;
	}
	return SUCCEED;
}


int append_suffix(LPWSTR file, PVOID suffix)
{
	TCHAR newfile[MAX_PATH] = { 0 };
	wsprintf(newfile, L"%s%s", file, (LPWSTR)suffix);
	BOOL bret = MoveFile(file, newfile);
	if (!bret) {
		TCHAR buffer[32] = { 0 };//int最大也不超过32位数字
		StringCchPrintf(buffer, 32, L"异常原因操作失败, 错误码:%d", GetLastError());
		PERROR(buffer); PRINTLN;
		return FAIL;
	}
	return SUCCEED;
}



//线程函数
DWORD WINAPI file_operator(PVOID lpparam)
{
	//强转参数
	FILE_OPERATOR_ARGS *args= (FILE_OPERATOR_ARGS *)lpparam;
	SEARCH_LIST *slist = create_list();

	list_file(&args->target, slist);
	SEARCH_NODE *temp = slist;
	if (!temp->next)
	{
		PRINT(L"没找到此文件");PRINTLN;
		TCHAR sp_path[MAX_PATH] = { 0 };
		TCHAR log[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, log, MAX_PATH);
		splite_path(log, sp_path, PATH_NO_NAME);
		lstrcat(sp_path, L"check\\log.log");
		write_log(sp_path, L"失败\r\n");
		return FAIL;
	}
	if (lstrcmp(temp->next->result, L"over32") == 0)
	{
		PRINT(L"异常: 超过最大目录层数");PRINTLN;
		return FAIL;
	}
	temp = temp->next;
	while (temp)
	{
		args->file_op(temp->result, args->other);
		TCHAR sp_path[MAX_PATH] = { 0 };
		TCHAR log[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, log, MAX_PATH);
		splite_path(log, sp_path, PATH_NO_NAME);
		lstrcat(sp_path, L"check\\log.log");
		write_log(sp_path, L"成功\r\n");
		temp = temp->next;
	}
	destroy_list(slist);
	return SUCCEED;
}

//多了一个参数保存线程句柄......
HANDLE start_task(LPTHREAD_START_ROUTINE file_opt, FILE_OPERATOR_ARGS *file_op_args)
{
	
	HANDLE handle = CreateThread(NULL, 0, file_opt, file_op_args, 0, 0);
	if (!handle)
	{
		TCHAR num_buffer[32] = { 0 };//这里写11  因为int 最大有10个数字..
		StringCchPrintf(num_buffer, 32, L"异常原因操作失败, 错误码:%d", GetLastError());
		PERROR(num_buffer);PRINTLN;
		return NULL;
	}
	return handle;
}

int init_args(int argc, TCHAR* argv[], FILE_OPERATOR_ARGS* args)
{
	if (argc < 3 || argc > 17 || ((argc % 2) != 1)) 
	{
		PRINT(L"请输入路径和路径下的文件名, 最多可输入8组目录和文件\n");
		PRINT(L"使用方法: d:\\test\\a a.txt c:\\pro b.txt\n");
		return 0;
	}

	for (int i = 0; i < argc; i += 2) 
	{
		lstrcpy(args[i / 2].target.dir, argv[i + 1]);
		lstrcpy(args[i / 2].target.name, argv[i + 2]);
		args[i / 2].other = L".txt";
		args[i / 2].file_op = append_suffix;
	}

	int num_list = (argc - 1) / 2;
	int same_name_flag = 0;
	for (int i = 0; i < num_list; i++) 
	{
		TCHAR temp[MAX_PATH] = { 0 };
		cut_path(args[i].target.dir, temp);
		for (int j = i + 1; j < num_list; j++) 
		{
			TCHAR temp1[MAX_PATH] = { 0 };
			cut_path(args[j].target.dir, temp1);
			if (0 == lstrcmp(temp, temp1)) 
			{
				PRINT(L"输入的目录不能是相同的目录或者同一个目录下的"); PRINTLN;
				return 0;
			}
		}
	}
	TCHAR log[MAX_PATH];
	lstrcpy(log, argv[0]);
	TCHAR sp_path[MAX_PATH] = { 0 };
	splite_path(log, sp_path, PATH_NO_NAME);
	lstrcat(sp_path, L"check");
	create_log(sp_path);
	return num_list;
}