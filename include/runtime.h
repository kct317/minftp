#ifndef RUNTIME_H_
#define RUNTIME_H_

// 声明客户连接的数据缓冲区指针
extern sbftpBuffer *conn_buff_global;

// 声明线程池指针
extern thread_pool_t *thread_pool_global;

// 初始化运行状态
bool sbftp_init();

// 清理服务器占据资源
void sbftp_destroy();

// 重置一个数据缓冲区
bool sbftp_reset_one_buff(sbftpBuffer *buff);

// 增大一个数据缓冲区
bool sbftp_resize_one_buff(sbftpBuffer *buff);

// 修改程序当前工作目录为设置的目录
bool sbftp_chdir();

// 当前系统时间打印函数
void sbftp_print_time();

#endif 	// RUNTIME_H_
