#include "sbftp.h"

// 线程池实现后，这里的线程处理函数不再使用，作为历史版本保留在项目中

// 创建一个具有脱离属性的线程
/*
static int sbftp_create_thread_detached(pthread_t *thread, void *(*thread_func) (void *), void *arg)
{
	pthread_attr_t thread_attr;
			
	if (pthread_attr_init(&thread_attr)) {
		return -1;
	}
	
	if (pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED)) {
		return -1;
	}

	if (pthread_create(thread, &thread_attr, thread_func, arg) == -1) {
		return -1;
	}

	pthread_attr_destroy(&thread_attr);

	return 0;
}

// 获取一个线程处理客户连接
bool sbftp_get_thread(void *arg)
{
	pthread_t thread;

	if (sbftp_create_thread_detached(&thread, sbftp_thread_job_entry, arg) == -1) {
		return false;
	}

	return true;
}
*/
