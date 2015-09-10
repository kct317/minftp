#include "sbftp.h"

// 各个命令的解析函数
// 用户验证
int do_user(user_env_t *user_env, ftp_cmd_t *cmd)
{
	//sbftpBuffer *conn_buff = &conn_buff_global[user_env->conn_fd];
	//ftp_cmd_t recv_cmd;

	if (config_global.anonymous_enable == false) {
		if (!sbftp_send_client_msg(user_env->conn_fd, ftp_send_msg[FTP_E_LOGIN])) {
			sbftp_print_info(LOG_INFO, "Write Data To Client Error!");
		}
		return -1;
	} else if (user_env->is_login_in == true) {
		if (!sbftp_send_client_msg(user_env->conn_fd, ftp_send_msg[FTP_S_ANONYMOUS])) {
			sbftp_print_info(LOG_INFO, "Write Data To Client Error!");
			return -1;
		}
	} else if (strcmp(cmd->arg, "anonymous") == 0) {
		if (!sbftp_send_client_msg(user_env->conn_fd, ftp_send_msg[FTP_NEED_PASS])) {
			sbftp_print_info(LOG_INFO, "Write Data To Client Error!");
			return -1;
		}
		strcpy(user_env->user_name, "anonymous");
		/*
		assert(conn_buff->len >= 0 && conn_buff->len < conn_buff->size);
		bzero(conn_buff->buff, conn_buff->len);
		*/
		if (!sbftp_send_client_msg(user_env->conn_fd, ftp_send_msg[FTP_S_CONN])) {
			sbftp_print_info(LOG_INFO, "Write Data To Client Error!");
			return -1;
		}

		/*
		conn_buff->len = read(user_env->conn_fd, conn_buff->buff, conn_buff->size);
		if (conn_buff->len <= 0) {
			sbftp_print_info(LOG_INFO, "Read Data From Client Error!");
			return -1;
		}
		int cmd_index = sbftp_anaylse_buff(&recv_cmd, conn_buff);
		if (cmd_index == 1 && strcmp(recv_cmd.arg, "anonymous") == 0) {
			user_env->is_login_in = true;
			if (!sbftp_send_client_msg(user_env->conn_fd, ftp_send_msg[FTP_S_ANONYMOUS])) {
				sbftp_print_info(LOG_INFO, "Write Data To Client Error!");
				return -1;
			}
			return 0;
		} else {
			if (!sbftp_send_client_msg(user_env->conn_fd, ftp_send_msg[FTP_E_NO_USER_PASS])) {
				sbftp_print_info(LOG_INFO, "Write Data To Client Error!");
				return -1;
			}
		}
		*/
	} else {
		if (!sbftp_send_client_msg(user_env->conn_fd, ftp_send_msg[FTP_NEED_PASS])) {
			sbftp_print_info(LOG_INFO, "Write Data To Client Error!");
			return -1;
		}
		strcpy(user_env->user_name, cmd->arg);
	}

	return 0;
}

// 验证密码(程序内部不会显示调用)
int do_pass(user_env_t *user_env, ftp_cmd_t *cmd)
{
	if (user_env->is_login_in == true) {
		if (!sbftp_send_client_msg(user_env->conn_fd, ftp_send_msg[FTP_S_ANONYMOUS])) {
			sbftp_print_info(LOG_INFO, "Write Data To Client Error!");
			return -1;
		}
	} else {
		// 判断密码是否正确
		if (strlen(cmd->arg) > 0) {
			user_env->is_login_in = true;
			if (!sbftp_send_client_msg(user_env->conn_fd, ftp_send_msg[FTP_S_ANONYMOUS])) {
		        	sbftp_print_info(LOG_INFO, "Write Data To Client Error!");
				return -1;
			}
		} else {
			if (!sbftp_send_client_msg(user_env->conn_fd, ftp_send_msg[FTP_E_PASS])) {
				sbftp_print_info(LOG_INFO, "Write Data To Client Error!");
				return -1;
			}
		}
	}

	return 0;
}

// 用户退出
int do_quit(user_env_t *user_env, ftp_cmd_t *cmd)
{
	return -1;
}

// 重新登录
int do_rein(user_env_t *user_env, ftp_cmd_t *cmd)
{
	user_env->is_login_in = 0;
	return 1;
}

// 写入当前目录
int write_cur_path(user_env_t *user_env, char * dir)
{
	unsigned int i = 0;
	int idx = 0;
	int stack[64];
	memset(stack, 0, sizeof(stack));
	for(; i<strlen(dir); ++i) {
		if( dir[i] == '/' ) {
			stack[idx++] = i;
		}
		if( dir[i] == '.' ) {
			if( *(char*)(dir+i+1) == '.' ) {
				if(idx > 1) {
					if( *(char*)(dir+i+2) == '/' ) {
						strncpy(dir+stack[idx-2]+1, dir+i+3, strlen(dir)-i-2);
					} else {
						strncpy(dir+stack[idx-2]+1, dir+i+2, strlen(dir)-i-1);
					}
					idx--;
					i = stack[idx-1];
				} else {
					return -1;
				}
			} else if( *(char*)(dir+i+1) == '/' ) {
				strncpy(dir+i, dir+i+2, strlen(dir)-i-1);
				i--;
			} else if( *(char*)(dir+i+1) == 0 ) {
				dir[i] =  0;
				break;
			}
		}
	}
	if(strlen(dir) > 0 && dir[strlen(dir)-1] == '/') {
		dir[strlen(dir)-1] = 0;
	} else if ( strlen(dir) == 0 ) {
		return -1; 
	}
	if ( IsDirExist(dir) == 0 ) {
		strcpy(user_env->cur_path, dir);
	} else {
		return -1;
	}
	return 0;
}

// 修改当前目录
int do_cwd(user_env_t *user_env, ftp_cmd_t *cmd)
{
	int flag = 0;
	if (cmd->arg[0] == 0 ) {
		if (!sbftp_send_client_msg(user_env->conn_fd, ftp_send_msg[FTP_E_ARGS_WRONG])) {
			sbftp_print_info(LOG_INFO, "Write Data To Client Error!");
			return -1;
		}
		return 0;
	}
	if (cmd->arg[0] == '/')
		flag = 1;

	char dir[ MAX_PATH ];
	memset(dir, 0, sizeof(dir));
	strcpy(dir, user_env->cur_path);
	int len = strlen(dir);
	if ( len == 0 )
		return -1;

	if ( dir[len-1] == '/' ) {
		dir[len-1] = 0;
	}
	if ( len == 1 ) {
		dir[0] = '/';
		dir[1] = 0;
	}

	switch ( flag )
	{
	case 0:
	{
		if ( len != 1 ) {
			strcat(dir, "/");
			strcat(dir, cmd->arg);
		} else {
			strcat(dir, cmd->arg);
		}
		if( write_cur_path(user_env, dir) < 0 ) {
			if (!sbftp_send_client_msg(user_env->conn_fd, ftp_send_msg[FTP_E_NO_FILE_DIR])) {
				sbftp_print_info(LOG_INFO, "Write Data To Client Error!");
			}
			return -1;
		}
		return 1;
	}
	case 1:
	{
		if( write_cur_path(user_env, dir) < 0 ) {
			if (!sbftp_send_client_msg(user_env->conn_fd, ftp_send_msg[FTP_E_NO_FILE_DIR])) {
				sbftp_print_info(LOG_INFO, "Write Data To Client Error!");
			}
			return -1;
		}
		return 1;
	}
	}

	return 0;
}

// 返回上级目录
int do_cdup(user_env_t *user_env, ftp_cmd_t *cmd)
{
	return 2;
}

// 指定要改名的原文件
int do_rnfr(user_env_t *user_env, ftp_cmd_t *cmd)
{
	return 2;
}

// 指定要改名的新文件
int do_rnto(user_env_t *user_env, ftp_cmd_t *cmd)
{
	return 2;
}

// 中断上一个命令
int do_abor(user_env_t *user_env, ftp_cmd_t *cmd)
{
	return 2;
}

// 删除指定文件
int do_dele(user_env_t *user_env, ftp_cmd_t *cmd)
{
	return 2;
}

// 删除指定目录
int do_rmd(user_env_t *user_env, ftp_cmd_t *cmd)
{
	return 2;
}

// 创建指定目录
int do_mkd(user_env_t *user_env, ftp_cmd_t *cmd)
{
	return 2;
}

// 显示当前目录
int do_pwd(user_env_t *user_env, ftp_cmd_t *cmd)
{
	char buf[ MAX_PATH ];
	memset(buf, 0, sizeof(buf));
	strncpy(buf, user_env->cur_path, strlen(user_env->cur_path));
	strncat(buf, "\r\n", 2);
	if (!sbftp_send_client_msg(user_env->conn_fd, buf)) {
		sbftp_print_info(LOG_INFO, "Write Data To Client Error!");
		return -1;
	}
	return 0;
}

// 列出当前目录和子目录
int do_list(user_env_t *user_env, ftp_cmd_t *cmd)
{
	if (!sbftp_send_client_msg(user_env->conn_fd, ftp_send_msg[FTP_S_HCDL])) {
		sbftp_print_info(LOG_INFO, "Write Data To Client Error!");
		return -1;
	}
  
	char tmp[13];
	char dir[MAX_PATH];
	char buf[MAX_PATH];
	memset(dir, 0, sizeof(dir));
	memset(buf, 0, sizeof(buf));
	int len;
	
	DIR* dir_info;
	struct dirent* dir_entry;
	dir_info = opendir(user_env->cur_path);
	int last_slash = 0;
	if( dir_info ){
		if (user_env->cur_path[strlen(user_env->cur_path)-1] == '/')
			last_slash = 1;
		while ( (dir_entry = readdir(dir_info)) != NULL)
		{
			struct stat s;
			if (dir_entry->d_name[0] == '.')
				continue;
			if( last_slash )
				snprintf(dir, MAX_PATH, "%s%s", user_env->cur_path, dir_entry->d_name);
			else
				snprintf(dir, MAX_PATH, "%s/%s", user_env->cur_path, dir_entry->d_name);
			if (stat(dir, &s) == 0){
				strftime(tmp, sizeof(tmp), "%b %d  %Y\0", localtime(&s.st_mtime));
				if((s.st_mode&S_IFMT) == S_IFDIR)
					len = sprintf(buf, "drwxrwxrwx %04u %8s %8s %8lu %s %s\r\n", 
						1, "0", "0", s.st_size, tmp, dir_entry->d_name);
				else
					len = sprintf(buf, "-rwxrwxrwx %04u %8s %8s %8lu %s %s\r\n", 
						1, "0", "0", s.st_size, tmp, dir_entry->d_name);
				if (!sbftp_send_client_msg(user_env->conn_fd, buf)) {
					sbftp_print_info(LOG_INFO, "Write Data To Client Error!");
					 goto end;
				}
			}
		} // while
		closedir(dir_info);
	}else{
		goto end;
	}

end:
	if (!sbftp_send_client_msg(user_env->conn_fd, ftp_send_msg[FTP_LO])) {
		sbftp_print_info(LOG_INFO, "Write Data To Client Error!");
		return -1;
	}
	
	return 0;
}

// 列出当前目录和子目录的名字
int do_nlst(user_env_t *user_env, ftp_cmd_t *cmd)
{
	return 2;
}

// 不支持该方式，仅支持 PASV
int do_port(user_env_t *user_env, ftp_cmd_t *cmd)
{
	return 2;
}

// 设置数据交换的类型 {A(SCII),E(BCDIC),I(mage),L(ocal byte size)}
int do_type(user_env_t *user_env, ftp_cmd_t *cmd)
{
	return 0;
}

int do_pasv(user_env_t *user_env, ftp_cmd_t *cmd)
{
	return 0;
}

// 下载
int do_retr(user_env_t *user_env, ftp_cmd_t *cmd)
{
	return 0;
}

// 覆盖式上传
int do_stor(user_env_t *user_env, ftp_cmd_t *cmd)
{
	return 0;
}

// 追加式上传(不准备支持)
int do_appe(user_env_t *user_env, ftp_cmd_t *cmd)
{
	return 2;
}

// 显示帮助(不准备实现)
int do_help(user_env_t *user_env, ftp_cmd_t *cmd)
{
	return 2;
}

// 服务器反馈收到即可
int do_noop(user_env_t *user_env, ftp_cmd_t *cmd)
{
	return 1;
}

// 命令解析函数指针的数组
p_do_cmd_func do_cmd_func[MAX_FTP_COMMAND] = {
			do_user, do_pass, do_quit, do_rein,
			do_cwd, do_cdup, do_rnfr, do_rnto,
			do_abor, do_dele, do_rmd, do_mkd,
			do_pwd, do_list, do_nlst, do_port,
			do_type, do_pasv, do_retr, do_stor,
			do_appe, do_help, do_noop
};
