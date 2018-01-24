/*
Copyright (c) 2016, Yomei Otani <yomei.otani@gmai.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.
*/


//
//    Websocket Chat Sample
//
//    ws://ipaddress/websocket/chat/[userid]
//
//    userid: integral number
//    forward: 0 client
//    forward: 1 server
//
//    example:   ws://127.0.0.1/websocket/chat/12345
//
//    Websocket Direction is Disabled by Specs.
//
#define I_CHECK_KID

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "websocketcmd.h"
#include "cfwdipc.h"
#ifdef I_CHECK_KID
#include "kansuicheck.h"
#endif
#include "wssvlog.h"


#ifdef _MSC_VER
#if _MSC_VER >= 1400
#pragma warning( disable : 4996 )
#pragma warning( disable : 4819 )
#endif
#endif

#define MAX_STRING 256
#define MAX_WS_N 3


struct st_wscmd{
	const char* url;
	int(*onopen)(const char* utl,void** vpp);
	int(*onclose)(const char* utl,void* vp);
	int(*onidle)(const char* utl, void* vp, char* out_str, int out_sz);
	int(*ondata)(const char* utl, void* vp, const char* in_str);
};

struct sw_user_chat{
	int share_id;
	int forward;
	char name1[64];
	char name2[64];
	void* h1;
	void* h2;
};


//
// following is test version1
//

static int onopen_chat(const char* url, void** vpp)
{
	struct sw_user_chat* suc;
	char *p;
	int a = 0, b = 0;

	//printf("onopen_chat()\n");

	if (vpp == NULL)return -1;
	*vpp = malloc(sizeof(struct sw_user_chat));
	if (*vpp == NULL)return -1;
	memset(*vpp, 0, sizeof(struct sw_user_chat));
	suc = (struct sw_user_chat*)(*vpp);

	p = strchr(url, '/');
	if (p == NULL)goto next;
	p++;
	p = strchr(p + 1, '/');
	if (p == NULL)goto next;
	p++;
	p = strchr(p + 1, '/');
	if (p == NULL)goto next;
	p++;
	sscanf(p, "%d/%d", &a, &b);
	if (b != 0)b = 1;

	suc->share_id = a;
	suc->forward = b;

	sprintf(suc->name1, "%d_%d", a, b);
	sprintf(suc->name2, "%d_%d", a, 1 - b);

	//printf("onopen_chat file=%s \n",suc->name1);
	suc->h1 = cfwdipc_start_service(suc->name1);
next:
	return 0;
}


static int onclose_chat(const char* utl, void* vp)
{
	struct sw_user_chat* suc;
	suc = (struct sw_user_chat*)(vp);
	//printf("onclose_chat()\n");

	if (suc && suc->h1){
		cfwdipc_stop_service(suc->h1);
	}
	if (suc)free(suc);

	//smplws_server_stop();

	return 0;

}

static int onidle_chat(const char* utl, void* vp, char* out_str, int out_sz)
{
	int len;
	struct sw_user_chat* suc;
	suc = (struct sw_user_chat*)(vp);
	if (suc->h1){
		CFWDIPC_MESSAGE msg;
		memset(&msg, 0, sizeof(msg));

		//printf("ondata_chat ismessage=%s \n", suc->name1);
		if (cfwdipc_is_message(suc->h1) == 0)return 0;
		//printf("ondata_chat read=%s \n", suc->name1);
		cfwdipc_get_message(suc->h1, &msg);
		len = strlen(msg.data);
		if (len < 1 || len >= MAX_STRING)return 0;

		strncpy(out_str, msg.data, out_sz);
		out_str[out_sz - 1] = 0;
	}
	return 0;
}

static int ondata_chat(const char* utl, void* vp, const char* in_str)
{
	struct sw_user_chat* suc;
	suc = (struct sw_user_chat*)(vp);

	//printf("ondata_chat()\n");

	suc->h2 = cfwdipc_find_service(suc->name2);
	if (suc->h2){
		CFWDIPC_MESSAGE msg;
		memset(&msg, 0, sizeof(msg));
		strncpy(msg.data, in_str, MAX_STRING);
		msg.data[MAX_STRING - 1] = 0;

		cfwdipc_send_message(suc->h2, &msg);
		cfwdipc_free_service(suc->h2);


		//printf("ondata_chat write=%s \n", suc->name2);
	}

	suc->h2 = NULL;
	//strncpy(out_str, in_str,out_sz);

	return 0;
}


//
// following is  version2
//

static int onopen_chat2(const char* url, void** vpp)
{
	struct sw_user_chat* suc;
	char *p;
	int a = 0;
	int i;
	//printf("onopen_chat()\n");

	if (vpp == NULL)return -1;
	*vpp = malloc(sizeof(struct sw_user_chat));
	if (*vpp == NULL)return -1;
	memset(*vpp, 0, sizeof(struct sw_user_chat));
	suc = (struct sw_user_chat*)(*vpp);

	p = strchr(url, '/');
	if (p == NULL)goto next;
	p++;
	p = strchr(p + 1, '/');
	if (p == NULL)goto next;
	p++;
	p = strchr(p + 1, '/');
	if (p == NULL)goto next;
	p++;
	sscanf(p, "%d", &a);
	suc->share_id = a;
#ifdef I_CHECK_KID
	{
		int kret = -1;
		printf("checking kid...\n");
		kret = kansuicheck(url);
		printf("ret=%d  url=>>%s<<\n",kret,url);
		if (kret == 0) {
			printf("Error kid \n");
			return -1;
		}
		printf("OK kid \n");
	}
#endif

	wssvlog_add("connect",url);

	for (i = 0; i < MAX_WS_N; i++){
		sprintf(suc->name1, "%d_%d",suc->share_id, i);
		suc->forward = i;
		suc->h1 = cfwdipc_find_service(suc->name1);
		if (suc->h1 != NULL){
			cfwdipc_free_service(suc->h1);
			continue;
		}
		suc->h1 = cfwdipc_start_service(suc->name1);
		if (suc->h1 != NULL){
			//printf("ws create OK >>%s<< \n",suc->name1);
			return 0;
		}
		break;
	}

	printf("too many same id  !!!! url=%s\n",url);
	wssvlog_add("too many same id", url);

next:
	free(suc);
	return -1;
}


static int onclose_chat2(const char* utl, void* vp)
{
	struct sw_user_chat* suc;
	suc = (struct sw_user_chat*)(vp);
	//printf("onclose_chat()\n");

	if (suc && suc->h1){
		cfwdipc_stop_service(suc->h1);
	}
	if (suc)free(suc);

	//smplws_server_stop();
	wssvlog_add("disconnect", utl);

	return 0;

}

static int onidle_chat2(const char* utl, void* vp, char* out_str, int out_sz)
{
	int len;
	struct sw_user_chat* suc;
	suc = (struct sw_user_chat*)(vp);
	if (suc->h1){
		CFWDIPC_MESSAGE msg;
		memset(&msg, 0, sizeof(msg));

		//printf("ondata_chat ismessage=%s \n", suc->name1);
		if (cfwdipc_is_message(suc->h1) == 0)return 0;
		//printf("ondata_chat read=%s \n", suc->name1);
		cfwdipc_get_message(suc->h1, &msg);
		len = strlen(msg.data);
		if (len < 1 || len >= MAX_STRING)return 0;

		strncpy(out_str, msg.data, out_sz);
		out_str[out_sz - 1] = 0;
	}
	return 0;
}

static int ondata_chat2(const char* utl, void* vp, const char* in_str)
{
	int i;
	struct sw_user_chat* suc;
	CFWDIPC_MESSAGE msg;

	//printf("ondata_chat()\n");

	suc = (struct sw_user_chat*)(vp);

	memset(&msg, 0, sizeof(msg));
	strncpy(msg.data, in_str, MAX_STRING);
	msg.data[MAX_STRING - 1] = 0;


	for (i = 0; i < MAX_WS_N; i++){
		if (i == suc->forward)continue;

		sprintf(suc->name2, "%d_%d", suc->share_id, i);
		suc->h2 = cfwdipc_find_service(suc->name2);
		if (suc->h2){

			cfwdipc_send_message(suc->h2, &msg);
			cfwdipc_free_service(suc->h2);


			//printf("ondata_chat write=%s \n", suc->name2);
		}
		suc->h2 = NULL;
	}
	//strncpy(out_str, in_str,out_sz);

	return 0;
}


//
//
//
struct st_wscmd a[] = {
	{"/websocket/chat/",onopen_chat2,onclose_chat2,onidle_chat2,ondata_chat2},
	{NULL}
};


int websocketcmd_open(char* url,void** vpp)
{
	int i;
	int ret = -1;
	for (i = 0; i < 10000; i++){
		if (a[i].url == NULL)return ret;
		if (strstr(url, a[i].url) == url){
			if (a[i].onopen)ret=a[i].onopen(url,vpp);
			return ret;
		}
	}
	return ret;
}

int websocketcmd_close(char* url,void* vp)
{
	int i;
	int ret = -1;
	for (i = 0; i < 10000; i++){
		if (a[i].url == NULL)return ret;
		if (strstr(url, a[i].url) == url){
			if (a[i].onclose)ret = a[i].onclose(url,vp);
			return ret;
		}
	}
	return ret;
}

int websocketcmd_data(char* url,void* vp,const char* in_str)
{
	int i;
	int ret = -1;
	for (i = 0; i < 10000; i++){
		if (a[i].url == NULL)return ret;
		if (strstr(url, a[i].url) == url){
			if (a[i].ondata)ret=a[i].ondata(url,vp,in_str);
			return ret;
		}
	}
	return ret;
}

int websocketcmd_idle(char* url, void* vp, char* out_str, int out_sz)
{
	int i;
	int ret = -1;
	for (i = 0; i < 10000; i++){
		if (a[i].url == NULL)return ret;
		if (strstr(url, a[i].url) == url){
			if (a[i].onidle)ret = a[i].onidle(url, vp, out_str, out_sz);
			return ret;
		}
	}
	return ret;
}
