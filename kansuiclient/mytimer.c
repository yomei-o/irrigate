﻿#define I_USE_FACTORY_COMMAND

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "mytimer.h"
#include "chkcmd.h"
#include "mythread.h"
#include "ysleep.h"
#include "cfwdipc.h"

#ifdef __arm__
#define RASPI
#endif

#ifdef _WIN32
#define RASPI
#endif

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

#ifdef RASPI
#define TIMER_BASE_DIR "/home/admin/irrigate/kansuiclient"
#else
#define TIMER_BASE_DIR "."
#endif

#if defined(RASPI)
#define CFG_DIR "/boot"
#elif defined(_WIN32)
#define CFG_DIR "."
#else
#define CFG_DIR "."
#endif

#define MAX_NUM_OF_TIMER 20
#define MYTIMER_FILE_NAME "mytimer.csv"
#define OFFSET_FILE_NAME "mytm_offset.txt"
#define MAX_NUM_OF_CH 5

#define MAX_NUM_OF_LOG 100
#define VERSION_STR "0.0.1"

#define GOTO 3
//#define I_USE_HEARTBEET

// タイマーのテストをするときはこれを有効にする
//#define I_USE_TEST_TIME_PTN 0

//
// util
//


static void get_timer_base_dir(char* buf, int sz)
{
	strncpy(buf, TIMER_BASE_DIR, sz);
}



//
// タイマー情報の読み書き
//


struct mytimer{
	int ch;
	int hour;
	int min;
	int sec;
	//
	int year;
	int mon;
	int day;
	//
	int duration;
	int status;
	int flag;
};


//タイマーリストコマンド用
static struct  mytimer mytimer_list[MAX_NUM_OF_CH][MAX_NUM_OF_TIMER];
//タイマーリストループ読み込み用
static struct mytimer mytimer_list2[MAX_NUM_OF_CH][MAX_NUM_OF_TIMER];

static char s_log[MAX_NUM_OF_LOG][80];


//
//
//

void add_log(const char* buf_)
{
	int i;
	char buf[256];
	time_t t;
	struct tm tt;

	if (buf_ == NULL || strlen(buf_) >= 80)return;

	t = time(NULL);
	tt = *localtime(&t);

	sprintf(buf, "%04d-%02d-%02d  %02d:%02d:%02d  %s",
		tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday,
		tt.tm_hour, tt.tm_min, tt.tm_sec, buf_);

	if (strlen(buf) >= 80)return;

	for (i = MAX_NUM_OF_LOG - 1; i >=1; i--)strcpy(s_log[i], s_log[i - 1]);
	strcpy(s_log[0], buf);
}

void print_log()
{
	int i;
	for (i = 0; i < MAX_NUM_OF_LOG; i++) {
		if (s_log[i][0] != 0) {
			printf("log: %d  %s\n", i, s_log[i]);
		}
	}
	printf("\n");
}

static void log_onoff(int on,int ch)
{
	char buf[256];
	if (on) {
		sprintf(buf, "PORT ON   ch=%d",ch);
	}
	else {
		sprintf(buf, "PORT OFF  ch=%d", ch);
	}
	add_log(buf);
	//print_log();
}
//
//
//
static void print_mytimer(void)
{
	int i, ch;

	printf("mytimer\n");
	for (ch = 0; ch < MAX_NUM_OF_CH; ch++) {
		for (i = 0; i < MAX_NUM_OF_TIMER; i++) {
			if (mytimer_list[ch][i].flag == 0)continue;
			printf("ch=%d  no=%d  %d:%d:%d %d  %d-%d-%d\n", ch, i,
				mytimer_list[ch][i].hour, mytimer_list[ch][i].min, mytimer_list[ch][i].sec,
				mytimer_list[ch][i].duration,
				mytimer_list[ch][i].year, mytimer_list[ch][i].mon, mytimer_list[ch][i].day);
		}
	}
	printf("");
	printf("mytimer2\n");
	for (ch = 0; ch < MAX_NUM_OF_CH; ch++) {
		for (i = 0; i < MAX_NUM_OF_TIMER; i++) {
			if (mytimer_list2[ch][i].flag == 0)continue;
			printf("ch=%d  no=%d  %d:%d:%d %d  %d-%d-%d\n", ch, i,
				mytimer_list2[ch][i].hour, mytimer_list2[ch][i].min, mytimer_list2[ch][i].sec,
				mytimer_list2[ch][i].duration,
				mytimer_list2[ch][i].year, mytimer_list2[ch][i].mon, mytimer_list2[ch][i].day);
		}
	}
	printf("");
}



static int read_timer_file()
{
	char filename[256];
	FILE* fp;

	char buf[256];
	int ct,ch;
	struct mytimer tt;

	memset(mytimer_list, 0, sizeof(mytimer_list));

	for (ch = 0; ch < MAX_NUM_OF_CH; ch++) {

		get_timer_base_dir(filename, 256);
		strcat(filename, "/");
		strcat(filename, MYTIMER_FILE_NAME);
		fp = fopen(filename, "rt");
		if (fp == NULL)return -1;

		ct = 0;
		while (1) {
			if (ct >= MAX_NUM_OF_TIMER)break;
			buf[0] = 0;
			fgets(buf, sizeof(buf), fp);
			if (buf[0] == 0)break;
			memset(&tt, 0, sizeof(tt));
			sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d", &tt.ch, &tt.hour, &tt.min, &tt.sec, &tt.duration,
			&tt.year,&tt.mon,&tt.day);
			if (tt.ch != ch)continue;
			tt.flag = 1;
			mytimer_list[ch][ct] = tt;
			ct++;
		}

		fclose(fp);
	}
	return 0;
}

static int read_timer_file2()
{
	char filename[256];
	FILE* fp;

	char buf[256];
	int ct,ch;
	struct mytimer tt;

	memset(mytimer_list2, 0, sizeof(mytimer_list2));

	for (ch = 0; ch < MAX_NUM_OF_CH; ch++) {
		get_timer_base_dir(filename, 256);
		strcat(filename, "/");
		strcat(filename, MYTIMER_FILE_NAME);
		fp = fopen(filename, "rt");
		if (fp == NULL)return -1;


		ct = 0;
		while (1) {
			if (ct >= MAX_NUM_OF_TIMER)break;
			buf[0] = 0;
			fgets(buf, sizeof(buf), fp);
			if (buf[0] == 0)break;
			memset(&tt, 0, sizeof(tt));
			sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d", &tt.ch, &tt.hour, &tt.min, &tt.sec, &tt.duration,
				&tt.year, &tt.mon, &tt.day);
			if (tt.ch != ch)continue;
			tt.flag = 1;
			mytimer_list2[ch][ct] = tt;
			ct++;
		}

		fclose(fp);
	}
	return 0;
}

static int write_timer_file()
{
	char filename[256];
	FILE* fp;
	int ch,i;

	get_timer_base_dir(filename, 256);
	strcat(filename, "/");
	strcat(filename, MYTIMER_FILE_NAME);
	fp = fopen(filename, "wt");
	if (fp == NULL)return -1;
	for (ch = 0; ch < MAX_NUM_OF_CH; ch++) {
		for (i = 0; i < MAX_NUM_OF_TIMER;i++) {
			if (mytimer_list[ch][i].flag == 0)continue;
			fprintf(fp, "%d,%d,%d,%d,%d,%d,%d,%d\n", mytimer_list[ch][i].ch, mytimer_list[ch][i].hour,
				mytimer_list[ch][i].min, mytimer_list[ch][i].sec, mytimer_list[ch][i].duration,
				mytimer_list[ch][i].year, mytimer_list[ch][i].mon, mytimer_list[ch][i].day);
		}
	}
	fclose(fp);
	return 0;
}

//
//
//

static int read_offset()
{
	char filename[256];
	FILE* fp;
	int ret = 0;

	char buf[256];
	get_timer_base_dir(filename, 256);
	strcat(filename, "/");
	strcat(filename, OFFSET_FILE_NAME);
	fp = fopen(filename, "rt");
	if (fp == NULL)return 0;

	buf[0] = 0;
	fgets(buf, sizeof(buf), fp);
	fclose(fp);
	if (buf[0] == 0)return 0;
	sscanf(buf, "%d", &ret);
	return ret;
}


static void write_offset(int offs)
{
	char filename[256];
	FILE* fp;

	get_timer_base_dir(filename, 256);
	strcat(filename, "/");
	strcat(filename, OFFSET_FILE_NAME);
	fp = fopen(filename, "wt");
	if (fp == NULL)return;
	fprintf(fp, "%d\n", offs);
	fclose(fp);
	return;
}


//
// タイマーのコールバック関数
//

int ch_state[MAX_NUM_OF_CH];

static void(*pfunc_on)(int,void*) = NULL;
static void* data_on = NULL;
static int timer_on(int id)
{
	if (id < 0 || id >= MAX_NUM_OF_CH)return 0;
	if (ch_state[id] == 0)return 0;
	ch_state[id] = 0;

	if (pfunc_on)pfunc_on(id,data_on);
	return 0;
}

void  mytimer_set_onfunc(void  (*pfunc)(int,void*), void* vp)
{
	pfunc_on = pfunc;
	data_on = vp;
}

static void (*pfunc_off)(int,void*) = NULL;
static void* data_off = NULL;
static int timer_off(int id)
{
	if (id < 0 || id >= MAX_NUM_OF_CH)return 0;
	if (ch_state[id] == 1)return 0;
	ch_state[id] = 1;

	if (pfunc_off)pfunc_off(id,data_off);
	return 0;
}

void  mytimer_set_offunc(void  (*pfunc)(int,void*), void* vp)
{
	pfunc_off = pfunc;
	data_off = vp;
}


//
// タイマー設定コマンド
//

int hello(int argc, char *argv[])
{
	mychkcmd_print("hello\n");
	return 0;
}

int version(int argc, char *argv[])
{
	mychkcmd_print(VERSION_STR "\n");
	return 0;
}

static int update(void)
{
	void* ipc;
	CFWDIPC_MESSAGE m;

	ipc = cfwdipc_find_service("mytimer");
	if (ipc) {
		memset(&m, 0, sizeof(m));
		m.param1 = 2;
		cfwdipc_send_message(ipc, &m);
		cfwdipc_free_service(ipc);
	}
	return 0;
}


int gettm(int argc, char *argv[])
{
	int a = -1, ch = -1;
	struct mytimer tt;
	char buf[256];
	memset(&tt, 0, sizeof(tt));

	if (argc < 3) {
		goto next;
	}
	sscanf(argv[1], "%d", &ch);
	if (ch < 0 || ch >= MAX_NUM_OF_CH) {
		goto next;
}
	sscanf(argv[2], "%d", &a);
	if (a < 0 || a >= MAX_NUM_OF_TIMER) {
		goto next;
	}
	read_timer_file();
	tt = mytimer_list[ch][a];
next:
	sprintf(buf,"gettm_res %d %d %d %d %d %d %d %d\n",tt.ch,tt.hour,tt.min,tt.sec,tt.duration,
		tt.year,tt.mon,tt.day);
	mychkcmd_print(buf);
	return 0;
}

int settm(int argc, char *argv[])
{
	int h = 0, ch =0,m=0,s=0,d=0,i;
	int yy = 0, mm = 0, dd = 0;
	struct mytimer tt;
	memset(&tt, 0, sizeof(tt));

	if (argc < 6) {
		goto next;
	}
	sscanf(argv[1], "%d", &ch);
	if (ch < 0 || ch >= MAX_NUM_OF_CH) {
		goto next;
	}
	sscanf(argv[2], "%d", &h);
	sscanf(argv[3], "%d", &m);
	sscanf(argv[4], "%d", &s);
	sscanf(argv[5], "%d", &d);
	if (argc >= 9) {
		sscanf(argv[6], "%d", &yy);
		sscanf(argv[7], "%d", &mm);
		sscanf(argv[8], "%d", &dd);
	}

	tt.ch = ch;
	tt.hour = h;
	tt.min = m;
	tt.sec = s;

	tt.year = yy;
	tt.mon = mm;
	tt.day = dd;

	tt.duration = d;
	tt.flag = 1;

	read_timer_file();
	for(i =0 ; i < MAX_NUM_OF_TIMER; i++) {
		if (mytimer_list[ch][i].flag == 0) {
			mytimer_list[ch][i] = tt;
			break;
		}
	}
	write_timer_file();
next:
	mychkcmd_print("settm_res ok\n");
	update();

	return 0;
}

int deltm(int argc, char *argv[])
{
	int a = -1, ch = -1;
	struct mytimer tt;

	if (argc < 3) {
		goto next;
	}
	sscanf(argv[1], "%d", &ch);
	if (ch < 0 || ch >= MAX_NUM_OF_CH) {
		goto next;
	}
	sscanf(argv[2], "%d", &a);
	if (a < 0 || a >= MAX_NUM_OF_TIMER) {
		goto next;
	}
	read_timer_file();
	memset(&tt, 0, sizeof(tt));
	mytimer_list[ch][a] = tt;
	write_timer_file();

next:
	mychkcmd_print("deltm_res ok\n");
	update();
	return 0;
}

int deltmall(int argc, char *argv[])
{
	read_timer_file();
	memset(mytimer_list, 0, sizeof(mytimer_list));
	//print_mytimer();
	write_timer_file();
	mychkcmd_print("deltmall_res ok\n");
	update();
	return 0;
}

int gettmnum(int argc, char *argv[])
{
	int a=-1,ch=-1,i;
	char buf[256];
	if (argc < 2) {
		goto next;
	}
	sscanf(argv[1], "%d", &ch);
	if (ch < 0 || ch >= MAX_NUM_OF_CH) {
		goto next;
	}
	a = 0;
	read_timer_file();
	for (i = 0; i < MAX_NUM_OF_TIMER;i++) {
		if (mytimer_list[ch][i].flag)a++;
	}

next:
	sprintf(buf, "gettmnum_res %d\n", a);
	mychkcmd_print(buf);
	return 0;
}


int gettmstate(int argc, char *argv[])
{
	int i;
	char buf[256];
	char tmp[256];

	buf[0] = 0;
	strcat(buf, "gettmstate_res");

	for (i = 0; i < MAX_NUM_OF_CH;i++) {
		sprintf(tmp, " %d", 1-ch_state[i]);
		strcat(buf, tmp);
	}
	strcat(buf,"\n");
	mychkcmd_print(buf);
	return 0;
}

//
// 時刻設定系
//
int getdate(int argc, char *argv[])
{
	int offs;
	char buf[256];
	struct tm *ptt;
	struct tm tt;
	time_t t;

	offs = read_offset();
	t=offs + time(NULL);

	memset(&tt, 0, sizeof(tt));
	ptt = localtime(&t);
	if (ptt)tt = *ptt;

	sprintf(buf, "getdate_res %d %d %d %d %d %d\n",
		tt.tm_year+1900,tt.tm_mon+1,tt.tm_mday,
		tt.tm_hour,tt.tm_min,tt.tm_sec);
	mychkcmd_print(buf);
	return 0;
}

int setdate(int argc, char *argv[])
{
	int yy = 0, mm = 0, dd = 0;
	int hh=0, ii=0, ss = 0;
	int offs;

	if (argc < 7) {
		goto end;
	}
	sscanf(argv[1], "%d", &yy);
	sscanf(argv[2], "%d", &mm);
	sscanf(argv[3], "%d", &dd);
	sscanf(argv[4], "%d", &hh);
	sscanf(argv[5], "%d", &ii);
	sscanf(argv[6], "%d", &ss);

	if (yy < 2000)yy += 2000;
	if (yy < 2000 || yy>2100)goto end;
	if (mm < 1 || mm>12)goto end;
	if (dd < 1 || dd>31)goto end;
	if (hh < 0 || hh>23)goto end;
	if (ii < 0 || ii>59)goto end;
	if (ss < 0 || ss>59)goto end;
	struct tm tt;
	memset(&tt, 0, sizeof(tt));
	tt.tm_year = yy - 1900;
	tt.tm_mon = mm - 1;
	tt.tm_mday = dd;
	tt.tm_hour = hh;
	tt.tm_min = ii;
	tt.tm_sec = ss;
	offs=(int)mktime(&tt);
	offs = (int)(offs - time(NULL));
	printf("offs=%d\n",offs);
	write_offset(offs);
	mychkcmd_print("setdate_res ok\n");
	return 0;
end:
	mychkcmd_print("setdate_res error\n");
	return 0;
}

//
// logging command
//


int getlog(int argc, char *argv[])
{
	int n = 0;
	char buf[256];

	if (argc < 2) {
		goto next;
	}
	sscanf(argv[1], "%d", &n);
	if (n < 0 || n >= MAX_NUM_OF_LOG) {
		n = 0;
	}
next:
	sprintf(buf, "getlog_res %s\n", s_log[n]);
	mychkcmd_print(buf);
	return 0;
}


//
// Factory command
//


#ifdef I_USE_FACTORY_COMMAND
int do_sync(int argc, char *argv[])
{
	char buf[256];
	system("sync");
	system("sync");
	system("sync");
	sprintf(buf, "sync_res ok\n");
	mychkcmd_print(buf);
	return 0;
}

#endif

#ifdef I_USE_FACTORY_COMMAND
int setfile(int argc, char *argv[])
{
	char fn[256];
	char fn2[256];
	char buf[256];
	char buf2[256];
	FILE* fp = 0;

	if (argc < 3) {
		goto next;
	}
	fn[0] = 0;
	buf[0] = 0;
	sscanf(argv[1], "%s", fn);
	sscanf(argv[2], "%s", buf);


	if (strstr(fn, "\\") != NULL)goto next;
	if (strstr(fn, "/") != NULL)goto next;
	if (strstr(fn, ".txt") == NULL)goto next;

first:
	strcpy(fn2, CFG_DIR);
	strcat(fn2, "/");
	strcat(fn2, fn);

	fp = fopen(fn2, "wt");
	if (fp == NULL)goto next_2;
	fprintf(fp, "%s\n", buf);
	fclose(fp);
	goto next;

next_2:
	strcpy(fn2, TIMER_BASE_DIR);
	strcat(fn2, "/");
	strcat(fn2, fn);

	fp = fopen(fn2, "wt");
	if (fp == NULL)goto next_3;
	fprintf(fp, "%s\n", buf);
	fclose(fp);
	goto next;

next_3:
	strcpy(fn2, ".");
	strcat(fn2, "/");
	strcat(fn2, fn);

	fp = fopen(fn2, "wt");
	if (fp == NULL)goto next;
	fprintf(fp, "%s\n", buf);
	fclose(fp);
	goto next;


next:
	sprintf(buf2, "setfile_res ok\n");
	mychkcmd_print(buf2);
	return 0;
}

#endif

#ifdef I_USE_FACTORY_COMMAND
int getfile(int argc, char *argv[])
{
	char fn[256];
	char fn2[256];
	char buf[256];
	char buf2[256];
	FILE* fp = 0;

	if (argc < 2) {
		goto next;
	}
	fn[0] = 0;
	buf[0] = 0;
	sscanf(argv[1], "%s", fn);

	if (strstr(fn, "\\") != NULL)goto next;
	if (strstr(fn, "/") != NULL)goto next;
	if (strstr(fn, ".txt") == NULL)goto next;

first:
	strcpy(fn2, CFG_DIR);
	strcat(fn2, "/");
	strcat(fn2, fn);

	fp = fopen(fn2, "rt");
	if (fp == NULL)goto next_2;
	fscanf(fp, "%s", buf);
	fclose(fp);
	goto next;

next_2:
	strcpy(fn2, TIMER_BASE_DIR);
	strcat(fn2, "/");
	strcat(fn2, fn);

	fp = fopen(fn2, "rt");
	if (fp == NULL)goto next_3;
	fscanf(fp, "%s", buf);
	fclose(fp);
	goto next;

next_3:
	strcpy(fn2, ".");
	strcat(fn2, "/");
	strcat(fn2, fn);

	fp = fopen(fn2, "rt");
	if (fp == NULL)goto next;
	fscanf(fp, "%s", buf);
	fclose(fp);
	goto next;

 next:

	sprintf(buf2, "getfile_res %s\n", buf);
	mychkcmd_print(buf2);
	return 0;
}

#endif

#ifdef I_USE_FACTORY_COMMAND
int rmfile(int argc, char *argv[])
{
	char fn[256];
	char fn2[256];
	char buf[256];
	FILE* fp = 0;
	int ret;

	if (argc < 2) {
		goto next;
	}
	fn[0] = 0;
	buf[0] = 0;
	sscanf(argv[1], "%s", fn);

	if (strstr(fn, "\\") != NULL)goto next;
	if (strstr(fn, "/") != NULL)goto next;
	if (strstr(fn, ".txt") == NULL)goto next;
first:
	strcpy(fn2, CFG_DIR);
	strcat(fn2, "/");
	strcat(fn2, fn);

	ret=remove(fn2);
	if (ret == 0)goto next;

next_2:
	strcpy(fn2, TIMER_BASE_DIR);
	strcat(fn2, "/");
	strcat(fn2, fn);

	ret = remove(fn2);
	if (ret == 0)goto next;
next_3:
	strcpy(fn2, ".");
	strcat(fn2, "/");
	strcat(fn2, fn);

	ret = remove(fn2);
	if (ret == 0)goto next;

next:
	sprintf(buf, "rmfile_res ok\n");
	mychkcmd_print(buf);
	return 0;
}

#endif



//
// オンオフをするかどうか判定
//


static void check_active(int gsec,int yy,int mm,int dd)
{
	int i,ch,st, ed,ss;
	for (ch = 0; ch < MAX_NUM_OF_CH; ch++) {
		for (i = 0; i < MAX_NUM_OF_TIMER; i++) {
			if (mytimer_list2[ch][i].flag == 0)continue;
			st = mytimer_list2[ch][i].hour * 3600 + mytimer_list2[ch][i].min * 60 + mytimer_list2[ch][i].sec;
			ed = st + mytimer_list2[ch][i].duration;
			ss = mytimer_list2[ch][i].status;
			if (gsec >= st && gsec < ed &&ss == 0) {
				if (mytimer_list2[ch][i].year == 0) {
					log_onoff(1, ch);
					timer_on(ch);
					mytimer_list2[ch][i].status = 1;
				}
				else if(mytimer_list2[ch][i].year==yy && mytimer_list2[ch][i].mon==mm && mytimer_list2[ch][i].day==dd){
					log_onoff(1, ch);
					timer_on(ch);
					mytimer_list2[ch][i].status = 1;
				}
			}
			if ((gsec < st || gsec >= ed) && ss == 1) {
				log_onoff(0, ch);
				timer_off(ch);
				mytimer_list2[ch][i].status = 0;
			}
		}
	}
}


//
// タイマーのループ関数
//

static mythread_t* th = NULL;
static mythread_t* th2 = NULL;
static int stop_flag = 0;
static int stop_flag2 = 0;

void mytimer_loop2(void* vp)
{
	void* ipc;
	CFWDIPC_MESSAGE m;
	int ret;

	printf("mytimer_loop2() start\n");
	ipc = cfwdipc_start_service("mytimer");
	while (1) {
		if (stop_flag2)break;
		memset(&m, 0, sizeof(m));
		ret=cfwdipc_get_message(ipc, &m);
		//ysleep(1);
		if (ret == 0)continue;
		if (m.param1 == 2) {
			printf("mytimer_loop2 reset\n");
			mytimer_reset();
		}
	}
	if (ipc) {
		cfwdipc_stop_service(ipc);
	}
}

void mytimer_loop(void* vp)
{

	int i,ct = 0,offset=0;
	time_t t;
	struct tm* ptm;
	int hour,sec,min,gsec;
	int offs = 0;

	printf("mytimer_loop() start\n");

	read_timer_file2();
	offs = read_offset();

	print_mytimer();

	for (i = 0; i < MAX_NUM_OF_CH; i++) {
		log_onoff(0, i);
		timer_off(i);
	}
#ifdef 	I_USE_TEST_TIME_PTN
	offset = (int)(time(NULL)+3600*9-86400)+offs;
#endif

	while (1){
		ct++;
		if(stop_flag)break;
		ysleep(1);
		if (ct%GOTO != 0)continue;
#ifdef I_USE_HEARTBEET
		printf(".");
#endif
		t = time(NULL)+offs;
		t -= offset;

		ptm=localtime(&t);
		hour = ptm->tm_hour;
		min = ptm->tm_min;
		sec = ptm->tm_sec;
		gsec = hour * 3600 + min * 60 + sec;
#ifdef I_USE_HEARTBEET
		printf("## %d %d %d %d %d %d##\n",
			ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,
			ptm->tm_hour,ptm->tm_min,ptm->tm_sec);
#endif
		check_active(gsec,ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday);
	}
	for (i = 0; i < MAX_NUM_OF_CH; i++) {
		timer_off(i);
	}
	printf("\n");
	printf("mytimer_loop() stop\n");
}



int mytimer_init()
{
	printf("mytimer_init()\n");
	th = mythread_create(mytimer_loop,NULL);
	th2 = mythread_create(mytimer_loop2, NULL);
	return 0;
}

int mytimer_done()
{
	printf("mytimer_done()\n");
	stop_flag = 1;
	stop_flag2 = 1;
	if (th2){
		mythread_join(th2);
	}
	th2 = NULL;
	if (th) {
		mythread_join(th);
	}
	th = NULL;
	stop_flag = 0;
	stop_flag2 = 0;
	return 0;
}

int mytimer_reset()
{
	printf("mytimer_reset()\n");
	stop_flag = 1;
	if (th){
		mythread_join(th);
	}
	stop_flag = 0;
	th = mythread_create(mytimer_loop, NULL);
	return 0;
}



//
// interface
//
void mytimer_execute_command(const char* cmd)
{
	mychkcmd_execute_command(cmd);
}

const char* mytimer_get_print_buffer()
{
	return mychkcmd_get_print_buffer();
}


#if 0


static void func_on(int id, void *vp)
{
	printf("func_on(ch=%d)\n", id);
}

static void func_off(int id, void *vp)
{
	printf("func_off(ch=%d)\n", id);
}


int main()
{
	char buf[256];
	const char* p;

#if defined(_WIN32) && !defined(__GNUC__)
	//	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_WNDW);
	//	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
	//	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_WNDW);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	mytimer_init();

	p = mychkcmd_get_print_buffer();
	mytimer_set_onfunc(func_on, NULL);
	mytimer_set_offunc(func_off, NULL);
	while (1) {
		buf[0] = 0;
		fgets(buf, 256, stdin);
		if (buf[0] == 0)break;
		mytimer_execute_command(buf);
		printf("result=>>%s<<\n", p);
		printf("\n");
	}
	mytimer_done();
}


#endif
#if 0
int main(int argc, char* argv[])
{
	const char* p;
	char* p1[] = { "", "aaa.txt", "hogehoge" };
	p = mychkcmd_get_print_buffer();

	setfile(3, p1);
	printf("result=>>%s<<\n", p);
	getfile(3, p1);
	printf("result=>>%s<<\n", p);
	rmfile(3, p1);
	printf("result=>>%s<<\n", p);
}
#endif



