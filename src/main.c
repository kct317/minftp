#include "sbftp.h"

int main(int argc, char *argv[])
{
	// 初始化服务器资源
	if(!sbftp_init()) {
		exit(-1);
	}

	// 进入网络连接处理的主循环
	if (!sbftp_accept_client()) {
		exit(-1);
	}

	// 释放资源并退出
	sbftp_destroy();

	return EXIT_SUCCESS;
}

