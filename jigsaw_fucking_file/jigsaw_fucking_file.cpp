#include "detest.h"
#include "myprocess.h"


int wmain(int argc, TCHAR* argv[])
{
	create_proc(argc, argv);
	int bret = check_prco();
	while (!bret)
	{
		check_prco();
	}
	
	MessageBox(NULL, L"¥¶¿ÌÕÍ±œ", L"Hi man!", NULL);
	
	
	
	return 0;
}

