#ifndef STATUS_H_
#define STATUS_H_

// 声明 FTP 发送字符串的数组指针
extern const char *ftp_send_msg[];

// 定义字符串所在编号
enum ftp_code
{
	FTP_S_FSO = 0,			// 150 File status okay; about to open data connection.
	FTP_S_HCDL,			// 150 here comes the directory listing.
	FTP_S_CONN,			// 200 Succeed to create data connection.
	FTP_WELCOME,			// 200 Welcome to sbftp server.
	FTP_E_OTHER_USER,		// 220 Only one user is anonymous.
	FTP_BYE,			// 221 Goodbye.
	FTP_LO,				// 226 list ok.
	FTP_S_ANONYMOUS,		// 230 Login successful.
	FTP_S_RMD,			// 250 RMD command successful.
	FTP_S_COMMAND,			// 250 Command succeed.
	FTP_S_FILE_DEL,			// 250 File sucessfully deleted.
	FTP_S_CREAT_DIR,		// 257 Directory successfully created:
	FTP_NEED_PASS,			// 331 Please specify the password.
	FTP_E_CONN,			// 421 Failed to create data connection.
	FTP_E_PASS,			// 430 Invalid username or password.
	FTP_E_NO_FILE,			// 450 No such file or directory.
	FTP_E_UNKNOW_CMD,		// 500 Unknown command.
	FTP_E_ARGS_WRONG,		// 501 Arguments wrong, the file or directory does not exists!
	FTP_E_ARGS_LONG,		// 501 Diretory or file name is too long.
	FTP_E_CHG_DIR,			// 501 Can't change directory.
	FTP_E_RENAME,			// 501 Can't rename this file.
	FTP_E_NO_LOGIN,			// 503 You have already logged in.
	FTP_E_LOGIN,			// 530 Login incorrect.
	FTP_E_NO_USER_PASS,		// 530 Please login with USER and PASS.
	FTP_E_PERMISSION,		// 550 Permission denied.
	FTP_E_NO_FILE_DIR		// 550 No such file or directory.
};

// 用户名长度
#define USER_NAME_LEN 20

// 用户登录服务器的环境变量
typedef struct user_env
{
	int is_login_in;			// 是否已经登录
	char user_name[USER_NAME_LEN];		// 登录的用户名
	bool passive_on;			// 是否为被动模式
	int conn_fd;				// 控制连接
	int data_fd;				// 数据连接
	char cur_path[MAX_PATH];                // 用户当前目录
}user_env_t;

#endif 	// STATUS_H_
