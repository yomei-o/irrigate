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

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif
#ifdef unix
#include <unistd.h>
#endif

#include "variant.h"
#include "settimeout.h"

static void mysleep(int s)
{
#ifdef _WIN32
	::Sleep(s * 1000);
#endif
#ifdef unix
	sleep(s);
#endif
}

static void println(var str)
{
	printf("%s\n",str.m_str.c_str());
}


static var tmlist;
static var tmtime;
static var tminf;
static int tmcnt = 0;
static int tmfinfunc = 0;

void setTimeout(var func, int tm)
{
	int i;
	for (i = 0; i < tmcnt; i++){
		if (tmlist[i] == null){
			//printf("settimeout %d\n", i);
			tmlist[i] = func;
			tmtime[i] = (int)tm;
			tminf[i] = tmfinfunc;
			return;
		}
	}
	//printf("settimeout %d\n",tmcnt);
	tmlist[tmcnt] = func;
	tmtime[tmcnt] = (int)tm;
	tmcnt++;

}

void loopTimeout(int tm)
{
	int i;
	var kore;

	for (i = 0; i < tmcnt; i++){
		if (tmlist[i] == null)continue;
		if (tminf[i]!=0)continue;
		tmtime[i] -= tm;
		if (tmtime[i]>0)continue;

		tmfinfunc = 1;
		kore = tmlist[i];
		if (kore.m_func1)kore.m_func1();
		if (kore.m_func2)kore.m_func2();
		if (kore.m_func3)kore.m_func3(null);
		if (kore.m_func4)kore.m_func4(null);
		tmfinfunc = 0;

		tmlist[i] = null;
		tmtime[i] = 0;
	}
	for (i = 0; i < tmcnt; i++){
		tminf[i] = 0;
	}
}

void sleepTimeout(int tm)
{
	mysleep(tm / 1000);
}


#if 0

var cnt = 0;
void func(){
	println("func()");
	cnt++;
	if (cnt > 10)return;
	setTimeout(func, 1000);
}


int main()
{
	setTimeout(func, 1000);
	for (;;){
		loopTimeout(1000);
		sleepTimeout(1000);
	}
}

#endif
