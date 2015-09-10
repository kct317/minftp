#include "sbftp.h"

// 客户处理线程的入口函数
void *sbftp_thread_job_entry(int arg)
{ 	
	user_env_t user_env;
	user_env.conn_fd = arg;
	user_env.is_login_in = 0;
	user_env.passive_on = 0;
	user_env.data_fd = 0;
	memset(user_env.cur_path, 0, MAX_PATH);
	getcwd(user_env.cur_path, MAX_PATH);
	sbftp_print_info(LOG_INFO, (const char *)user_env.cur_path);

	sbftpBuffer *conn_buff = &conn_buff_global[user_env.conn_fd];

#ifdef FTP_DEBUG
	sbftp_print_info(LOG_INFO, "A Job Create!");
#endif
	// 客户端连接刚建立状态
	client_state_t client_state = state_conn;

	// 只要用户不处于已退出状态就保持状态机循环
	while (client_state != state_close) {
		switch (client_state) {
		case state_conn:
			if (!sbftp_send_client_msg(user_env.conn_fd, ftp_send_msg[FTP_WELCOME])) {
				sbftp_print_info(LOG_INFO, "Write Data To Client Error!");
			 	client_state = state_close;
				break;
			}
			client_state = state_login;
			break;
		case state_login:
			recv_buffer rbuff;
			rbuff.len = 0;
			bzero(rbuff.buff, BUFF_LENGTH);

			if (read_msg(user_env.conn_fd, conn_buff, rbuff) > 0) {
				client_state = sbftp_parse_cmd(&user_env, rbuff);
			} else {
				sbftp_print_info(LOG_INFO, "Read Data From Client Error!");
				client_state = state_close;
				break;
			}
			break;
		case state_quit:
			if (!sbftp_send_client_msg(user_env.conn_fd, ftp_send_msg[FTP_BYE])) {
				sbftp_print_info(LOG_INFO, "Write Data To Client Error!");
				return NULL;
			}
			client_state = state_close;
			break;
		default:
			break;
		}
	}

	// 关闭前一定重置缓冲区
	sbftp_reset_one_buff(conn_buff);

	// 插入内存屏障，严格控制时序
	_barrier();
	
	// 所有的套接字关闭全部由状态机轮转到此处
	close(user_env.conn_fd);

#ifdef FTP_DEBUG
	sbftp_print_info(LOG_INFO, "A Job Exit!");
#endif

	return NULL;
}

// 向客户端发送一条消息
bool sbftp_send_client_msg(int conn_fd, const char *msg)
{
	if (rio_writen(conn_fd, msg, strlen(msg)) > 0) {
		return true;
	}

	return false;
}

