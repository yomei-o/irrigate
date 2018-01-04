/*

Copyright (c) 2016, Yomei Otani <yomei.otani@gmai.com>
All rights reserved.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfwdipc.h"
#include "myfifo.h"

#if defined(_WIN32) && !defined(__GNUC__)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif


void* cfwdipc_start_service(char* name)
{
	return myfifo_open_read(name);
}
void cfwdipc_stop_service(void* h)
{
	myfifo_close(h);
}
int cfwdipc_get_message(void* h,CFWDIPC_MESSAGE* m)
{
	return myfifo_read(h,m,sizeof(CFWDIPC_MESSAGE));
}

int cfwdipc_is_message(void* h)
{
	return myfifo_ismessage(h);
}


void* cfwdipc_find_service(char* name)
{
	return myfifo_open_write(name);
}
void cfwdipc_free_service(void* h)
{
	myfifo_close(h);
}
int cfwdipc_send_message(void* h,CFWDIPC_MESSAGE* m)
{
	return myfifo_write(h,m,sizeof(CFWDIPC_MESSAGE));
}




//
//
// test code
//
//

#if 0

void do_service(void)
{
	void* h;

	printf("starting service mode\n");

	h=cfwdipc_start_service(CFWDIPC_SERVICE_MAIN);
	if(h==NULL){
		printf("stat service error\n");
		return;
	}
	while(1){
		CFWDIPC_MESSAGE m;
		int r;
		r=cfwdipc_get_message(h,&m);
		printf(".");
		fflush(stdout);
		if(r>0){
			if(m.type==CFWDIPC_MESSAGE_TYPE_STOP){
				printf("\n");
				printf("STOP request was received\n");
				break;
			}
		}
	}
	if(h)cfwdipc_stop_service(h);
	printf("stopped service \n");
}

void do_client(void)
{
	void* h;
	CFWDIPC_MESSAGE m;

	printf("starting client mode\n");

	h=cfwdipc_find_service(CFWDIPC_SERVICE_MAIN);
	if(h==NULL){
		printf("find service error\n");
		return;
	}
	memset(&m,0,sizeof(m));
	m.type=CFWDIPC_MESSAGE_TYPE_STOP;
	cfwdipc_send_message(h,&m);

	cfwdipc_free_service(h);
}


int main(int argc,char* argv[])
{
	int i;
	int service=0;

#if defined(_WIN32) && !defined(__GNUC__)
//	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_WNDW);
//	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
//	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_WNDW);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
#endif

	for(i=1;i<argc;i++)if(strstr(argv[i],"-s"))service=1;
	for(i=1;i<argc;i++)if(strstr(argv[i],"-d"))service=1;

	if(service){
		do_service();
	}else{
		do_client();
	}

	return 0;
}


#endif
