#ifndef PARSE_CMD_H_
#define PARSE_CMD_H_

// 最多计划支持 23 个命令
#define MAX_FTP_COMMAND 23

// 解析读取到的内容
client_state_t sbftp_parse_cmd(user_env_t *user_env, recv_buffer &conn_buff);

// 从缓冲区中解析出命令
int sbftp_anaylse_buff(user_env_t *user_env, ftp_cmd_t *recv_cmd, recv_buffer &rbuff);

#endif 	// PARSE_CMD_H_
