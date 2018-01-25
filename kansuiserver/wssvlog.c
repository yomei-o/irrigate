#include<stdio.h>
#include<string.h>
#include<time.h>
#include"wssvlog.h"

#if defined(_WIN32) && !defined(__GNUC__)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif


#ifdef _MSC_VER
#if _MSC_VER >= 1400
#pragma warning( disable : 4996 )
#pragma warning( disable : 4819 )
#endif
#endif



#define SVLOG_FILE_NAME "access_log_wsd.txt"

void wssvlog_add(const char* str,const char* str2)
{
	char buf[256];
	time_t t;
	struct tm tt;
	FILE* fp;

	if (str == NULL || strlen(str) >= 50)return;
	if (str2 == NULL || strlen(str2) >= 150)return;

	t = time(NULL);
	tt = *localtime(&t);

	sprintf(buf, "%04d-%02d-%02d  %02d:%02d:%02d  %s  %s",
		tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday,
		tt.tm_hour, tt.tm_min, tt.tm_sec, str,str2);

	if (strlen(buf) >= 200)return;

	fp = fopen(SVLOG_FILE_NAME, "at");
	if (fp == NULL)return;
	fprintf(fp,"%s\n",buf);
	fclose(fp);
}

