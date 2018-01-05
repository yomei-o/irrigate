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

#ifndef JAVASCRIPT
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif
#ifdef unix
#include <unistd.h>
#endif

#include "variant.h"
#include "websocket.h"
#include "easywebsocketclient.h"

#endif

#ifndef JAVASCRIPT
#define true 1
#define false 0
#endif

#ifdef JAVASCRIPT 
#define BEGIN_TRY() try{
#define END_TRY() }catch(e){}
#else
#define BEGIN_TRY()
#define END_TRY()
#endif

#ifdef JAVASCRIPT 
#define _delete(a,b) delete a[b]
#else
#define _delete(a,b) a.delete_(b)
#endif

#define WS_STATE_CLOSED 0
#define WS_STATE_REQOPEN 1
#define WS_STATE_CONNECTED 2
#define WS_STATE_ERROR 9



//
// debug func
//

#ifndef JAVASCRIPT
static void mysleep(int s)
{
#ifdef _WIN32
	::Sleep(s * 1000);
#endif
#ifdef unix
	sleep(s);
#endif
}
#endif

#ifdef JAVASCRIPT
println = function(str)
{
	alert(str);
}
#else
static void println(var str)
{
	printf("%s\n", str.m_str.c_str());
}
#endif

#ifdef JAVASCRIPT
strlength = function(src)
#else
static var strlength(var src)
#endif
{
	var ret = 0;
	if (src == null || src == "")return ret;
#ifdef JAVASCRIPT
	ret = src.length;
#else
	ret = src.length();
#endif
	return ret;
}

#ifndef JAVASCRIPT
static void strlength(const char* a)
{
	var aa;
	aa = a;
	strlength(aa);
}
#endif


//
// statics 
//


#define MAX_WEBSOCKET 20

var ws_handle = new Array();
var ws_url = new Array();

var ws_onopen = new Array();
var ws_onerror = new Array();
var ws_onclose = new Array();
var ws_onmessage = new Array();

var ws_stat = new Array();
#ifndef JAVASCRIPT
static int ws_socket[MAX_WEBSOCKET];
static int ws_idle[MAX_WEBSOCKET];
#endif


//
// functions
//

#ifdef JAVASCRIPT
websocket_isusing=function(n)
#else
var websocket_isusing(var n)
#endif
{
	var ret = false;

	if (n<0 || n>=MAX_WEBSOCKET)return ret;
	if (ws_handle[n] == null)return ret;

	ret = true;
	return ret;

}

#ifdef JAVASCRIPT
websocket_open=function(n,str)
#else
var websocket_open(var n, var str)
#endif
{
	var n_ = n;

	if (n<0 || n>=MAX_WEBSOCKET)return -1;
	if (ws_handle[n]!=null)return -1;

	ws_handle[n] = null;
	ws_url[n] = str;

	ws_onopen[n] = null;
	ws_onerror[n] = null;
	ws_onclose[n] = null;
	ws_onmessage[n] = null;

	ws_stat[n]=0;

#ifdef JAVASCRIPT
	try{
		ws_handle[n] = new WebSocket(str);
	}catch (e){}
#else
	ws_handle[n]=1;
	ws_stat[n] = WS_STATE_REQOPEN;
	ws_idle[n] = 0;
#endif
	if (ws_handle[n] == null){
		return -1;
	}

#ifdef JAVASCRIPT
	ws_handle[n].onmessage = function(evt){
		if (ws_onmessage[n_] != null)ws_onmessage[n_](n_, evt.data);
	}
	ws_handle[n].onopen = function(evt){
		if (ws_onopen[n_] != null)ws_onopen[n_](n_);
	}
	ws_handle[n].onerror = function(evt){
		if (ws_onerror[n_] != null)ws_onerror[n_](n_);
	}
	ws_handle[n].onclose = function(evt){
		if (ws_onclose[n_] != null)ws_onclose[n_](n_);
	}
#endif
	return 0;
}

#ifdef JAVASCRIPT
websocket_close = function(n)
#else
var websocket_close(var n)
#endif
{
	if (n < 0 || n >= MAX_WEBSOCKET || ws_handle[n] == null)return -1;

#ifdef JAVASCRIPT
	try{
		ws_handle[n].close();
	}catch (e){}
#else
	if (ws_socket[n] != -1){
		mywebsocket_close(ws_socket[n]);
		ws_socket[n] = -1;
		ws_idle[n] = 0;

		//if (ws_onclose[n] != null){
		//	if (ws_onclose[n].m_func3)ws_onclose[n].m_func3(n);
		//	if (ws_onclose[n].m_func4)ws_onclose[n].m_func4(n);
		//}
	}
#endif

	ws_onopen[n] = null;
	ws_onerror[n] = null;
	ws_onmessage[n] = null;
	ws_onclose[n] = null;

	ws_handle[n] = null;
	ws_url[n] = null;
	ws_stat[n] = 0;

	return 0;
}

#ifdef JAVASCRIPT
websocket_send = function(n, str)
#else
var websocket_send(var n, var str)
#endif
{
	var ret = -1;
	if (n < 0 || n >= MAX_WEBSOCKET || ws_handle[n] == null)return -1;
	if (strlength(str) < 1)return -1;

#ifdef JAVASCRIPT
	try{
		ws_handle[n].send(str);
		ret=0;
	}
	catch (e){}
#else
	if (ws_socket[n] != -1){
		mywebsocket_send_string(ws_socket[n], str.m_str.c_str());
		ret = 0;
	}
#endif
	return ret;
}
//
//
//

#ifdef JAVASCRIPT
websocket_onmessage = function(n, func)
#else
void websocket_onmessage(var n,var func)
#endif
{
	if (n<0 || n >= MAX_WEBSOCKET || ws_handle[n] == null)return;
	ws_onmessage[n] = func;
}

#ifdef JAVASCRIPT
websocket_onclose = function(n, func)
#else
void websocket_onclose(var n, var func)
#endif
{
	if (n<0 || n >= MAX_WEBSOCKET || ws_handle[n] == null)return;
	ws_onclose[n] = func;
}

#ifdef JAVASCRIPT
websocket_onopen = function(n, func)
#else
void websocket_onopen(var n, var func)
#endif
{
	if (n<0 || n >= MAX_WEBSOCKET || ws_handle[n] == null)return;
	ws_onopen[n] = func;
}

#ifdef JAVASCRIPT
websocket_onerror = function(n, func)
#else
void websocket_onerror(var n, var func)
#endif
{
	if (n<0 || n >= MAX_WEBSOCKET || ws_handle[n] == null)return;
	ws_onerror[n] = func;
}



//
//
//

#ifndef JAVASCRIPT
static int websocket_onrecv(void* vp, int type, void* data, int sz)
{
	int n = (int)vp;
	char tmp[1024];
	var str = "";

	//printf("\n");
	//printf("opcode=%d\n", type & 0xf);
	//printf("size=%d\n", sz);
	//printf("data=>>%s<<\n", data);

	ws_idle[n] = 0;

	if ((type&0xf) == 9){
		return 0;
	}
	if (ws_handle[n] == null)return 0;

	if (sz > 1000)return 0;
	memset(tmp, 0, sizeof(tmp));
	strncpy(tmp,(char*)data,1000);
	tmp[1000] = 0;
	str = tmp;

	if (ws_onmessage[n] != null){
		if (ws_onmessage[n].m_func5)ws_onmessage[n].m_func5(n,str);
		if (ws_onmessage[n].m_func6)ws_onmessage[n].m_func6(n,str);
	}
	return 0;

}

#endif

#ifndef JAVASCRIPT
void websocket_loop(int tm)
{
	int i;
	for (i = 0; i < MAX_WEBSOCKET; i++)
	{
		//if (i == 0){
		//	printf("wa_stat[0]=%d\n",(int)(ws_stat[i]));
		//	printf("wa_socket[0]=%d\n", (int)(ws_socket[i]));
		//}
		if (ws_stat[i] == 0)continue;
		if (ws_stat[i] == WS_STATE_REQOPEN){
			ws_socket[i] = mywebsocket_connect(ws_url[i].m_str.c_str());
			if (ws_socket[i] == -1){
				ws_stat[i] = WS_STATE_ERROR;
				if (ws_onerror[i] != null){
					if (ws_onerror[i].m_func3)ws_onerror[i].m_func3(i);
					if (ws_onerror[i].m_func4)ws_onerror[i].m_func4(i);
				}
			}
			else{
				ws_stat[i] = WS_STATE_CONNECTED;
				if (ws_onopen[i] != null){
					if (ws_onopen[i].m_func3)ws_onopen[i].m_func3(i);
					if (ws_onopen[i].m_func4)ws_onopen[i].m_func4(i);
				}

			}
			continue;
		}
		if (ws_socket[i] == -1)continue;
		if (ws_stat[i] == WS_STATE_CONNECTED){
			int ret;
			ret = mywebsocket_wait_time(ws_socket[i], 0, websocket_onrecv, (void*)i);
			if (ret == 0){
				ws_idle[i] += tm;
			}
			if (ret == -1 || ws_idle[i]>1000*30){
				mywebsocket_close(ws_socket[i]);
				ws_socket[i] = -1;
				ws_idle[i] = 0;
				ws_stat[i] = WS_STATE_ERROR;
				if (ws_onerror[i] != null){
					if (ws_onclose[i].m_func3)ws_onclose[i].m_func3(i);
					if (ws_onclose[i].m_func4)ws_onclose[i].m_func4(i);
				}
			}
		}
	}
}
#endif

#ifndef JAVASCRIPT
void websocket_sleep(int tm)
{
	mysleep(tm / 1000);
}
#endif


//
//
//
#ifdef JAVASCRIPT
#else
void websocket_init()
{
	int i;
	for (i = 0; i < MAX_WEBSOCKET; i++){
		ws_socket[i] = -1;
	}
	mywebsocket_init();
}
#endif

#ifdef JAVASCRIPT
#else
void websocket_fini()
{
	int i;
	for (i = 0; i < MAX_WEBSOCKET; i++){
		if(ws_socket[i] != -1)websocket_close(ws_socket[i]);
		ws_socket[i] = -1;
	}
	mywebsocket_done();
}
#endif


#ifdef JAVASCRIPT
#else
class websocket___init{
public:
	websocket___init();
	virtual ~websocket___init();
};

websocket___init::websocket___init()
{
	websocket_init();
}

websocket___init::~websocket___init()
{
	websocket_fini();
}

static websocket___init s_ws;


#endif




