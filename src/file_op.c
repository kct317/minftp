#include "sbftp.h"


int IsDirExist(const char* path)
{
	DIR *dp;
	if ((dp = opendir(path)) == NULL) {
		return -1;
	}	 
	closedir(dp);
	return 0;
}

int IsFileExist(const char* path)
{
	return !access(path, F_OK);
}

