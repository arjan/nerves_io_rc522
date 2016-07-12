/*
 * config.c
 *
 *  Created on: 05.09.2013
 *      Author: alexs
 */

#include "config.h"

char config_file[255]="/etc/RC522.conf";
FILE *fdconfig=NULL;
char str[255];

void reload_config (int sugnum) {

}
int read_conf_uid(uid_t * ruid) {
	char user[5];
	uid_t uid;
	gid_t gid;

	if (find_config_param("UID=",user,sizeof(user),0)<=0) {
		if  (getuid()<100) {
			syslog(LOG_DAEMON|LOG_ERR,"UID must be set!\n");
			return -1;
		}
	}else{
		uid=(int)strtol(user,NULL,10);
		if (uid<100) {
			syslog(LOG_DAEMON|LOG_ERR,"Invalid UID: %s\n",user);
			return -1;
		}
		*ruid=uid;
	}
	if (find_config_param("GID=",user,sizeof(user),0)==1) {
		gid=(int)strtol(user,NULL,10);
		setgid(gid);
	}
	return 0;
}

int open_config_file(char * conffile) {
	if (fdconfig==NULL) {
		if (access(conffile,R_OK)!=0) return -1;
		if ((fdconfig=fopen(conffile,"r"))==NULL) return -1;
	}
	return 0;
}

void close_config_file() {
	fclose(fdconfig);
}

int find_config_param(char * param_name, char * param_val, int val_len, int log) {
	int param_found=0;
	char * pstr;

	if (fseek(fdconfig, 0L, SEEK_SET)!=0) return -1;
	while (fgets(str,sizeof(str)-1,fdconfig)!=NULL) {
		if ((pstr=strchr(str,'#'))!=NULL) *pstr=0; //Заменим # на конец строки.
		if ((pstr=strstr(str,param_name))!=NULL) {
			param_found=1;
			if (log) syslog(LOG_DAEMON|LOG_INFO,"Found param. %s",str);
			pstr+=strlen(param_name);
			while (isspace(*pstr)) pstr++;
			while (isspace(pstr[strlen(pstr)-1])) pstr[strlen(pstr)-1]=0;
			strncpy(param_val,pstr,val_len);
#if DEBUG==1
			printf("Debug:%s\n",param_val);
#endif
			break;
		}
	}
	//	fclose(fdconfig);
	return param_found;
}
