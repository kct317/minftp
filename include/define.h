#ifndef DEFINE_H_
#define DEFINE_H_

#ifndef MAX_PATH
#define MAX_PATH 4096
#endif

#ifndef MAX_RECV_BUFF
#define MAX_RECV_BUFF 8192
#endif

// 套接字缓冲区默认尺寸
#ifndef BUFF_LENGTH
#define BUFF_LENGTH 4096
#endif

#define BUF_SIZE 1024
#define KEY_SIZE 50

/*
 * Using __builtin_constant_p(x) to ignore cases where the return
 * value is always the same.  This idea is taken from a similar patch
 * written by Daniel Walker.
 */
# ifndef likely
#  define likely(x)	(__builtin_constant_p(x) ? !!(x) : __branch_check__(x, 1))
# endif
# ifndef unlikely
#  define unlikely(x)	(__builtin_constant_p(x) ? !!(x) : __branch_check__(x, 0))
# endif



// 服务器名称
#define SERVER_NAME "sbftp Server"

// 服务器端口
#ifdef FTP_DEBUG
	#define SERV_PORT 9527
#else
	#define SERV_PORT 21
#endif

// 服务器最大连接数
#define MAX_CONNECT_SIZE 1024

// 服务端最大支持用户数
#define MAX_CONNECT_USER 100

// 默认线程池大小
#define THREAD_POOL_SIZE 10

// 线程池每次增加粒度
#define THREAD_POOL_ADD_SIZE 10

// listen() 的 backlog 队列长度
#define LISTEN_SIZE 20

// 套接字缓冲区增加粒度(经验值)
#define BUFF_ADD_LENGTH 128


const unsigned short msgHead = 0x7FFF;
const unsigned short msgTail = 0xFFFF;



#ifndef __cplusplus
typedef enum _bool {
	false = 0,
	true = 1
}bool;
#endif

typedef unsigned char BYTE;

#endif 	// DEFINE_H_
