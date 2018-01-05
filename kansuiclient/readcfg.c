#include<stdio.h>
#include<string.h>
#include<ctype.h>

#include "readcfg.h"
#include "easyhttpclient.h"

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


#define DEFAULT_URL "http://irrigate.sensprout.net/htdocs/kansuilogin.exe?ki=1&kp=admin"
#define SERVER_FILE  "server.txt"

#if defined(_WIN32)
#define CFG_DIR "."
#elif defined(RASPI)
#define CFG_DIR "."
#else
#define CFG_DIR "."
#endif

static void trim(char *s) {
	int i, j;

	for (i = strlen(s) - 1; i >= 0 && isspace(s[i]); i--);
	s[i + 1] = '\0';
	for (i = 0; isspace(s[i]); i++);
	if (i > 0) {
		j = 0;
		while (s[i]) s[j++] = s[i++];
		s[j] = '\0';
	}
}

void readcfg(const char*file, const char* defstr, char* out, int osz)
{
	FILE* fp;
	char buf[1024];
	if (out == NULL || osz < 1)return;
	out[0] = 0;
	if (file == NULL)goto err;
	strcpy(buf, CFG_DIR);
	strcat(buf, "/");
	strcat(buf, file);

	fp = fopen(buf, "rb");
	if (fp == NULL)goto err;
	while (1) {
		buf[0] = 0;
		fgets(buf, sizeof(buf), fp);
		//printf("buf=>>%s<<\n", buf);
		if (buf[0] == 0)break;
		if (buf[0] == '#')continue;
		break;
	}
	trim(buf);
	if (buf[0] == 0)goto err;
	strncpy(out, buf, osz);
	out[osz - 1] = 0;
	return;

err:
	{
		strncpy(out, defstr, osz);
		out[osz - 1] = 0;
		return;
	}
}

void get_ws_url(char* out, int osz)
{
	void* data;
	int sz, ret, res;
	char buf[1024];

	if (out == NULL || osz < 1)return;
	out[0] = 0;
	buf[0] = 0;
	readcfg(SERVER_FILE, DEFAULT_URL, buf, sizeof(buf));

	ret = myhttp_get_data(buf, &data, &sz, &res);
	if (data == NULL)return;
	if (res < 200 || res >= 300)goto end;
	trim(data);
	strncpy(out, data, osz);
	out[osz - 1] = 0;
	//printf(">>%s<<\n", (char*)data);
end:
	myhttp_free_data(data);
}


#if 0

int main()
{
	char buf[256];
	strcpy(buf, "\r\n  sdsd  \r\n sdsdsdsd\r\n    \r\n");
	trim(buf);
	printf("buf=>>%s<<\n",buf);
	return 0;
}

#endif

#if 0

int main()
{
	char buf[256];
	readcfg("test.txt", DEFAULT_URL, buf, 20);
	trim(buf);
	printf("buf=>>%s<<\n", buf);
	return 0;
}

#endif





