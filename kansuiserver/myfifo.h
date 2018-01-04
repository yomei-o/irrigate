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


#ifndef df_MYFIFO_H_
#define df_MYFIFO_H_

#ifdef __cplusplus
extern "C"
{
#endif

void* myfifo_open_read(char* name);
void* myfifo_open_write(char* name);
void myfifo_close(void* vp);
int myfifo_write(void* vp,void* buf,int sz);
int myfifo_read(void* vp,void* buf,int sz);
void myfifo_sleep(int s);
int myfifo_ismessage(void* vp);

#ifdef __cplusplus
}
#endif


#endif
	




