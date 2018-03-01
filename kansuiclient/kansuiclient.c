#define I_CHECK_CMD
//#define I_DEBUG_DIRECT_CONNECT

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>


#ifdef _WIN32
#include <windows.h>
#endif	/* _WIN32 */

#if defined(unix) || defined(ANDROID_NDK) || defined(__APPLE__)
#include <unistd.h>
#endif	/* unix */

#if defined(_WIN32) && !defined(__GNUC__)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "wscclient.h"
#include "readcfg.h"
#include "mytimer.h"



#ifdef _WIN32
#define POPEN_READMODE "rb"
#define popen _popen
#define pclose _pclose
#else
#define POPEN_READMODE "r"
#endif

#ifdef _MSC_VER
#if _MSC_VER >= 1400
#pragma warning( disable : 4996 )
#pragma warning( disable : 4819 )
#endif
#endif
#if defined(_WIN32) && !defined(__GNUC__)
#pragma comment( lib, "ws2_32.lib" )
#endif



//
//
//




//
// ON/OFF Functions
//


static void func_on(int id, void *vp)
{

	printf("func_on(ch=%d)\n", id);

	switch (id) {
	case 0:
		system("gpio -g mode 21 out");
		system("gpio -g write 21 1");
		break;
	case 1:
		system("gpio -g mode 20 out");
		system("gpio -g write 20 1");
		break;
	case 2:
		system("gpio -g mode 16 out");
		system("gpio -g write 16 1");
		break;
	case 3:
	system("gpio -g mode 19 out");
	system("gpio -g write 19 1");
		break;
	case 4:
		break;
	}
}

static void func_off(int id, void *vp)
{

	printf("func_off(ch=%d)\n", id);

	switch (id) {
	case 0:
		system("gpio -g mode 21 out");
		system("gpio -g write 21 0");
		break;
	case 1:
		system("gpio -g mode 20 out");
		system("gpio -g write 20 0");
		break;
	case 2:
		system("gpio -g mode 16 out");
		system("gpio -g write 16 0");
		break;
	case 3:
	system("gpio -g mode 19 out");
	system("gpio -g write 19 0");
		break;
	case 4:
		break;
	}
	//system(buf);
}



//
// sleep
//

static void
ysleep(unsigned int s)
{
#if defined(unix) || defined(__APPLE__)
	sleep(s);
#else
#ifdef _WIN32
	Sleep(s * 1000);
#endif
#endif
	return;
}



#if 1

static char cmdbuf[1024];
static char tmpbuf[1024];

static void nakusu_sjis(char* p)
{
	if (p == NULL)return;
	while (*p){
		if (*p<0 || *p>0x80){
			*p = '-';
			if (*(p + 1))*(p + 1) = '-';
		}
		p++;
	}
}


char* ok_cmd[] = {
	"ls",
	NULL
};

static int check_cmd(const char* buf)
{
	int i;
	char tmp[1024];
	if (buf == NULL)return 0;
	if (strlen(buf) > 1000)return 0;
	tmp[0] = 0;
	sscanf(buf,"%s", tmp);
	for (i = 0; i < 10000; i++) {
		if (ok_cmd[i] == NULL)break;
		if (strcmp(tmp, ok_cmd[i]) == 0)return 1;
	}
	return 0;
}

static int exec_command(void*vp, const char* cmd)
{
	FILE* fp;
	char buf[1024];
	int s;
	s = (int)vp;
#ifdef  I_CHECK_CMD
	if (check_cmd(cmd) == 0) {
		//strcpy(buf, "error command check\n");
		//mywebsocket_send_string(s, buf);
		return -1;
	}
#endif
	fp = popen(cmdbuf, POPEN_READMODE);
	if (fp == NULL)return 0;
	for (;;){
		buf[0] = 0;
		fgets(buf, sizeof(buf), fp);
		if (buf[0] == 0)break;
		nakusu_sjis(buf);
		printf("sendstring>>%s<<\n", buf);
		mywebsocket_send_string(s, buf);
	}
	fclose(fp);
	return 0;
}
static int exec_command2(void*vp, const char* cmd)
{
	char* p;
	int s;
	s = (int)vp;
	p = mychkcmd_get_print_buffer();
	mytimer_execute_command(cmd);
	mywebsocket_send_string(s, p);
	return 0;
}

static int callback_func(void* vp,int type, void* data, int sz)
{
	char* p;
	int s;
	int ret;

	s = (int)vp;

	if ((type & 0xf) == 9){
		return 0;
	}
	if (strlen(data) > 255){
		cmdbuf[0] = 0;
		return 0;
	}

	strcat(cmdbuf, data);
	if (strlen(data) > 255){
		cmdbuf[0] = 0;
		return 0;
	}

	for (;;){
		p = strchr(cmdbuf, '\n');
		if (p == NULL)break;
		*p = 0;
		p++;
		strcpy(tmpbuf, p);
		printf("cmd=>>%s<<\n",cmdbuf);
		//printf("tmp=>>%s<<\n", tmpbuf);
		ret=exec_command(vp, cmdbuf);
		if (ret == -1) {
			exec_command2(vp, cmdbuf);
		}
		strcpy(cmdbuf, tmpbuf);

	}
	//mywebsocket_send_string(s, "++++++++++end++++++++++\n");
	return 0;
}

int main()
{
	int s;
	int ct = 0;
	char buf[1024];
	int tm = 0;
#if defined(_WIN32) && !defined(__GNUC__)
	//	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_WNDW);
	//	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
	//	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_WNDW);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif


	mywebsocket_init();
	mytimer_init();
	mytimer_set_onfunc(func_on, NULL);
	mytimer_set_offunc(func_off, NULL);

	for (;;) {
		tm = 0;
		printf("wsclient connecting...\n");
		buf[0] = 0;
		get_ws_url(buf, sizeof(buf));

#ifdef I_DEBUG_DIRECT_CONNECT
		strcpy(buf, "ws://127.0.0.1:12345/websocket/chat/1?ki=1&kp=admin");
		strcpy(buf, "ws://irrigate.sensprout.net:12345/websocket/chat/12345?ki=1&kp=admin");
#endif

		printf(">>%s<<\n", buf);


		//s = mywebsocket_connect("ws://127.0.0.1:12345/websocket/chat/12345?ki=1&kp=admin");
		//s = mywebsocket_connect("ws://irrigate.sensprout.net:12345/websocket/chat/12345?ki=1&kp=admin");
		s = mywebsocket_connect(buf);
		if (s != -1) {
			int r;
			printf("wsclient waiting mesages\n");
			while (1) {
				tm++;
				r = mywebsocket_wait(s, callback_func, (void*)s);
				if (r > 0)tm = 0;
				if (tm > 20)break;
				//printf("r=%d  tm=%d  \n",r,tm);
				if (r < 0)break;
				ct++;
				if (ct % 10 == 0)printf(".");
				if (ct % 600 == 0)printf("\n");
			}
			printf("wsclient disconnected\n");
			mywebsocket_close(s);
		}
		printf("wsclient sleeping...\n");
		ysleep(20);
	}
	mytimer_done();
	mywebsocket_done();

	return 0;
}

#endif
