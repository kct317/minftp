#include "sbftp.h"

BYTE * sbftpBuffer::getHeadPoint() { 
	if( headready != true ) {
		AdjustHead();
		if( headready != true ) 
		  return NULL;
	}
	return (BYTE *)(buff+head);
}

BYTE * sbftpBuffer::getHeadSecondPoint() { 
	if( headready != true ) {
		AdjustHead();
		if( headready != true ) 
		  return NULL;
	}
	return (BYTE *)(buff+(head+1)% MAX_RECV_BUFF);
}

BYTE * sbftpBuffer::getEndPoint() { 
	return (BYTE *)(buff + ( tail % MAX_RECV_BUFF ));
}

BYTE * sbftpBuffer::getTailHPoint() { 
	return (BYTE *)(buff + ( (tail-1) % MAX_RECV_BUFF ));
}

BYTE * sbftpBuffer::getTailLPoint() { 
	return (BYTE *)(buff + (tail-2) % MAX_RECV_BUFF);
}

unsigned short sbftpBuffer::getMsgLen() { 
	if( headready != true ) {
		AdjustHead();
		if( headready != true ) 
		  return 0;
	}
	BYTE * hLen = (buff+(head+3)% MAX_RECV_BUFF);
	BYTE * lLen = (buff+(head+2)% MAX_RECV_BUFF);
	unsigned short len = *hLen;
	len = len << 8;
	len = len + *lLen;
	return len;
}

int sbftpBuffer::getFreeLength() {
  if( tail >= MAX_RECV_BUFF )
    return head - ( tail % MAX_RECV_BUFF );
  else
    return MAX_RECV_BUFF - tail;
}

void sbftpBuffer::AdjustHead() {
	if( memcmp(buff+head, (void *)&msgHead, sizeof(unsigned short)) == 0 ) {
		headready = true;
		return;
	}
	// 下面禁用getHeadPoint() or getHeadSecondPoint() !!!
	unsigned char * p = NULL;
	unsigned char hhead = 0x7f;
	unsigned char lhead = 0xff;
	for(int i=head; i<tail; ++i ) {
		p = buff + (i % MAX_RECV_BUFF);
		if( lhead == *p ) {
			if( i == (MAX_RECV_BUFF - 1) ) {
				if( hhead == *buff ) {
					head = MAX_RECV_BUFF - 1;
					headready = true;
				}
			} else {
				if( hhead == *(p + 1) ) {
					head = (i % MAX_RECV_BUFF);
					headready = true;
				}
			}
		}
	}
	head = tail % MAX_RECV_BUFF;
	tail = head;
}

void sbftpBuffer::appendLength( int len ) {
	tail += len;
	if( tail > MAX_RECV_BUFF ) {
		if( ( (tail+2) % MAX_RECV_BUFF ) >= head ) {
			printf("Debug Info : sbftpBuffer tail > head over %d\n", MAX_RECV_BUFF);
			tail = head;
		}
	}
	if( headready != true ) {
		AdjustHead();
	}
	
}

bool sbftpBuffer::hasCompleteMsg() {
	if( headready != true ) {
		return false;
	}
	int len = getMsgLen();
	if(len <= 0)   return false;
	unsigned char * pHtail = getTailHPoint();
	if(pHtail == NULL )   return false;
	unsigned char * pLtail = getTailLPoint();
	if(pLtail == NULL )   return false;
	if( len > (tail - head) ) {
		return false;
	}
	if( *pHtail == 0xFF && *pLtail == 0xFF ) {
		return true;
	}
	return false;
}

void sbftpBuffer::getOneMsg(void * p, int buflen) {
	if( hasCompleteMsg() ) {
		int len = getMsgLen();
		unsigned char * pHtail = (unsigned char *)(buff + ((head + 4) % MAX_RECV_BUFF));
		len = buflen > len ? len : buflen;
		memcpy(p, pHtail, len);
		char * pp = (char *)p;
		pp[len] = 0;
		
		head = (head + len + 6) % MAX_RECV_BUFF;
	} else {
		char * pp = (char *)p;
		pp[0] = 0;
	}
}
