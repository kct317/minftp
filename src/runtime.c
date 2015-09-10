#include "sbftp.h"

// 客户连接的数据缓冲区指针
sbftpBuffer *conn_buff_global;

// 线程池指针
thread_pool_t *thread_pool_global;

// 为客户连接的数据缓冲区申请内存
static bool sbftp_malloc_buff();

// 为客户连接的数据缓冲区释放内存
static bool sbftp_free_buff();

// 初始化运行状态
bool sbftp_init()
{ 	
	// 读取配置文件
	if (!sbftp_read_config()) {
		sbftp_print_info(LOG_ERR, "Read Config File Error!");
		return false;
	}

	// 设置当前工作目录
	if (!sbftp_chdir()) {
		sbftp_print_info(LOG_ERR, "Change DIR Error!");
		return false;
	}

#ifndef FTP_DEBUG
	// 守护进程模式运行
	if (daemon(NULL, NULL) == -1) {
		sbftp_print_info(LOG_ERR, "Into Deamon Mode Error!");
		return false;
	}
#else
	printf("\n-------------------- " SERVER_NAME " --------------------\n\n");
	sbftp_print_time();
	printf(" Started\n\nWaiting ...\n\n");
#endif
	// 为客户连接的数据缓冲区申请内存
	if (!sbftp_malloc_buff()) {
		sbftp_print_info(LOG_ERR, "Alloc Buff Memery Error!");
		return false;
	}
	
	// 初始化线程池
	if ((thread_pool_global = thread_pool_init(THREAD_POOL_SIZE, THREAD_POOL_SIZE)) == NULL) {
		sbftp_print_info(LOG_ERR, "Thread Pool Init Error!");
		return false;
	}

	return true;
}

// 销毁服务器占据资源
void sbftp_destroy()
{
	// 释放线程数据接收缓存
	sbftp_free_buff();

	// 释放线程池资源
	thread_pool_destroy(thread_pool_global);
}

// 为客户连接的数据缓冲区申请内存
static bool sbftp_malloc_buff()
{
	conn_buff_global = (sbftpBuffer *)malloc(sizeof(sbftpBuffer) * MAX_CONNECT_SIZE);
	if (conn_buff_global == NULL) {
		return false;
	}

	int i;
	for (i = 0; i < MAX_CONNECT_SIZE; i++) {
		conn_buff_global[i].buff = (BYTE *)malloc(MAX_RECV_BUFF);
		conn_buff_global[i].size = MAX_RECV_BUFF;
		conn_buff_global[i].head = 0;
		conn_buff_global[i].tail = 0;
		conn_buff_global[i].headready = false;
		if (conn_buff_global[i].buff == NULL) {
			return false;
		}
	}

	return true;
}

// 为客户连接的数据缓冲区释放内存
static bool sbftp_free_buff()
{
	int i;
	for (i = 0; i < MAX_CONNECT_SIZE; i++) {
		if (conn_buff_global[i].buff != NULL) {
			free(conn_buff_global[i].buff);
		}
	}
	free(conn_buff_global);

	return true;
}

// 重置一个数据缓冲区
bool sbftp_reset_one_buff(sbftpBuffer *buff)
{
	if (buff != NULL) {
		buff->head = buff->tail = 0;
		return true;
	}

	return false;
}

// 增大一个数据缓冲区
bool sbftp_resize_one_buff(sbftpBuffer *buff)
{
	if (buff != NULL) {
		buff->buff = (BYTE *)realloc(buff->buff, buff->size + BUFF_ADD_LENGTH);
		if (buff != NULL) {
			buff->size += BUFF_ADD_LENGTH;
			return true;
		}
	}

	return false;
}

// 修改程序当前工作目录为设置的目录
bool sbftp_chdir()
{
	if (chdir(config_global.ftp_path) == -1) {
		return false;
	}

	return true;
}

// 当前系统时间打印函数
void sbftp_print_time()
{
	const char *wday[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
	time_t 	  timep;
	struct	  tm *p;
	
	time(&timep);
	p = localtime(&timep);
	printf("%02d/%02d/%02d ", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday);
	printf("%s %02d:%02d:%02d", wday[p->tm_wday], p->tm_hour, p->tm_min, p->tm_sec);
}
