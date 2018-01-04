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



#ifndef df_CFWDIPC_H_
#define df_CFWDIPC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define CFWDIPC_SERVICE_MAIN "main_service"
#define CFWDIPC_SERVICE_ID_MAIN 1

#define CFWDIPC_MESSAGE_TYPE_START 1
#define CFWDIPC_MESSAGE_TYPE_STOP 2


typedef struct cfwdipc_message {
	int type;
	int sub_type;
	int sender_id;
	int param1;
	int param2;
	char data[2048];
}CFWDIPC_MESSAGE;

void* cfwdipc_start_service(char* name);
void cfwdipc_stop_service(void* h);
int cfwdipc_get_message(void* h,CFWDIPC_MESSAGE* m);
int cfwdipc_is_message(void* h);


void* cfwdipc_find_service(char* name);
void cfwdipc_free_service(void*);
int cfwdipc_send_message(void* h,CFWDIPC_MESSAGE* m);

#ifdef __cplusplus
}
#endif


#endif /* CFWDIPC */
	




