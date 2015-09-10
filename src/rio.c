#include "sbftp.h"

size_t rio_readn(int fd, void *usrbuf, size_t n)
{
	size_t nleft = n;
	int nread = 0;
	char   *bufp = (char *)usrbuf;

	while (nleft > 0) {
		if ((nread = read(fd, bufp, nleft)) == -1) {
			if (errno == EINTR) { 	// Interrupted by sig handler return
				nread = 0; 	// and call read() again
			} else {
				return -1; 	// errno set by read()
			}
		} else if (nread == 0) {
			break; 		// EOF
		}
		nleft -= nread;
		bufp += nread;
	}

	return (n - nleft); 	// return >= 0
}

size_t rio_writen(int fd, const void *usrbuf, size_t n)
{
	char buf[BUFF_LENGTH];
	memset(buf, 0, sizeof(buf));
	memcpy(buf, &msgHead, sizeof(unsigned short) );
	unsigned short slen = (unsigned short)n;
	memcpy(buf+sizeof(unsigned short), &slen, sizeof(unsigned short) );
	memcpy(buf+sizeof(unsigned short)+sizeof(unsigned short), usrbuf, n );
	memcpy(buf+sizeof(unsigned short)+sizeof(unsigned short)+n, &msgTail, sizeof(unsigned short) );
  
	size_t nleft = n+3*sizeof(unsigned short);
	size_t nwritten = 0;
	char   *bufp = buf;
	
	while (nleft > 0) {
		if ((nwritten = write(fd, bufp, nleft)) <= 0) {
			if (errno == EINTR) { 	// Interrupted by sig handler return
				nwritten = 0; 	// and call write() again
			} else {
				return -1; 	// errno set by write()
			}
		}
		nleft -= nwritten;
		bufp += nwritten;
	}

	return n;
}


int read_msg(int fd, sbftpBuffer * conn_buff, recv_buffer & rbuff)
{
	int nread = 0;
  
	while (1) {
		if ((nread = read(fd, conn_buff->getEndPoint(), conn_buff->getFreeLength())) == -1) {
			if (errno == EINTR || errno == EAGAIN) {
				continue;
			} else {
				printf("Debug Info : sbftp read errno %d\n", errno);
				return -1*errno;
			}
		} else if (nread == 0) {
			return 0;
		}
		
		conn_buff->appendLength(nread);
		if ( conn_buff->hasCompleteMsg() ) {
			  conn_buff->getOneMsg(rbuff.buff, BUFF_LENGTH);
			  rbuff.len = strlen(rbuff.buff);
			  return rbuff.len;
		}
	}
}


