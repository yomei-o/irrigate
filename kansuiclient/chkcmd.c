#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define I_USE_MYTIMER

#ifdef _MSC_VER
#if _MSC_VER >= 1400
#pragma warning( disable : 4996 )
#pragma warning( disable : 4819 )
#endif
#endif

#ifdef _WIN32
#define snprintf _snprintf 
#endif

#include "chkcmd.h"

#ifdef I_USE_MYTIMER
#include "mytimer.h"
#endif
//
// output
//

static char print_buffer[256];
void mychkcmd_print(const char* buf)
{
	snprintf(print_buffer,256,"%s",buf);
}

void mychkcmd_clear_print_buffrt()
{
	print_buffer[0] = 0;
}

const char* mychkcmd_get_print_buffer()
{
	return print_buffer;
}

//
//
//

struct mychkcmd_list{
	char* name;
	int (*pfunc)(int argc, char *argv[]);
};

static int dummy(int argc, char* argv[])
{
	int i;
	printf("argc=%d\n",argc);
	printf("cmd=>>%s<<\n",argv[0]);
	for (i = 1; i < argc; i++){
		printf("argv[%d]=>>%s<<\n", i,argv[i]);
	}
	printf("\n");
	mychkcmd_print("command not found!\n");
	return 0;
}

struct mychkcmd_list cmd_list[] = {
#ifdef I_USE_MYTIMER
	{ "hello", hello },
	{ "version", version },

	{ "gettmnum", gettmnum },
	{ "gettm", gettm },
	{ "settm", settm },
	{ "deltm", deltm },
	{ "deltmall", deltmall },
	{ "gettmstate", gettmstate },
	{ "getdate", getdate },
	{ "setdate", setdate },
#endif
	{ "*", dummy },
	{ NULL,NULL},
};


void mychkcmd_execute_command(const char* cmd)
{
	char arg[10][128];
	char* arglist[10];

	int i,n;
	for (i = 0; i < 10; i++){
		arg[i][0] = 0;
		arglist[i] = arg[i];
	}
	mychkcmd_clear_print_buffrt();
	if (cmd == NULL || strlen(cmd) >= 128){
		mychkcmd_print("error(100)");
		return;
	}
	n=sscanf(cmd,"%s %s %s %s %s %s %s %s %s %s",
		arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9]);
	if (n < 1){
		mychkcmd_print("error(101)");
		return;
	}
	for (i = 0; i < 10000; i++){
		if (cmd_list[i].name == NULL)break;
		if (strcmp(cmd_list[i].name, "*") == 0){
			(cmd_list[i].pfunc)(n, arglist);
			return;
		}
		if (strcmp(cmd_list[i].name, arg[0]) == 0){
			(cmd_list[i].pfunc)(n, arglist);
			return;
		}
	}
	mychkcmd_print("error(102)\n");
}




