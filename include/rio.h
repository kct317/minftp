#ifndef RIO_H_
#define RIO_H_

// RIO 无缓冲的 I/O 函数
size_t rio_readn(int fd, void *usrbuf, size_t n);
size_t rio_writen(int fd, const void *usrbuf, size_t n);

// 有缓冲大 I/O 函数
// 返回值大于0：有数据包且是数据长度；
//      小于0：接收报错；
//      等于0：对方关闭连接
int read_msg(int fd, sbftpBuffer * conn_buff, recv_buffer & rbuff);

#endif // RIO_H_
