#ifndef TYPES_H_
#define TYPES_H_

// 定义服务器数据缓冲区类型
// 循环缓冲区
// 判断是否有一个完整的数据包   2个字节包头(0x7FFF) + 2个字节数据包内容长度 + 数据包内容 + 2个字节包尾(0xFFFF)   以后再加crc后缀验证
class sbftpBuffer {
public:
	unsigned char *buff;
	int  size;
	int  head; // [0, MAX_RECV_BUFF)
	int  tail; // [0, 2*MAX_RECV_BUFF)
	bool headready;
	
	BYTE * getHeadPoint();
	BYTE * getHeadSecondPoint();
	
	BYTE * getEndPoint();
	
	BYTE * getTailHPoint();
	BYTE * getTailLPoint();
	unsigned short getMsgLen();
	
	int getFreeLength();
	
	void AdjustHead();
	
	void appendLength( int len );
	
	bool hasCompleteMsg();
	
	void getOneMsg(void * p, int buflen);
};


typedef struct  recv_buffer {
	int  len;
	char buff[BUFF_LENGTH];
}recv_buffer;

#endif 	// TYPES_H_
