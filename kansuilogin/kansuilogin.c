#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#ifdef WIN32
#include<fcntl.h>
#include <io.h>
#endif

#ifdef _MSC_VER
#if _MSC_VER >= 1400
#pragma warning( disable : 4996 )
#pragma warning( disable : 4819 )
#endif
#endif

#if defined(_WIN32) && !defined(__GNUC__)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif


#include "csvsql.h"
#include "kanji.h"


//
//  binarymode
//
static void setstdoutmode()
{
#ifdef WIN32
	_setmode(_fileno(stdout), _O_BINARY);
#endif

}


//
// util
//


void urldecode2(char *dst, const char *src)
{
	char a, b;
	while (*src) {
		if ((*src == '%') &&
			((a = src[1]) && (b = src[2])) &&
			(isxdigit(a) && isxdigit(b))) {
			if (a >= 'a')
				a -= 'a' - 'A';
			if (a >= 'A')
				a -= ('A' - 10);
			else
				a -= '0';
			if (b >= 'a')
				b -= 'a' - 'A';
			if (b >= 'A')
				b -= ('A' - 10);
			else
				b -= '0';
			*dst++ = 16 * a + b;
			src += 3;
		}
		else if (*src == '+') {
			*dst++ = ' ';
			src++;
		}
		else {
			*dst++ = *src++;
		}
	}
	*dst++ = '\0';
}

void mygetenv(char* buf, const char* moji, int sz)
{
	char *p;
	if (buf == NULL || moji==NULL || sz<1)return;
	p = getenv(moji);
	if (p == NULL)p = "";

	buf[0] = 0;
	strncpy(buf, p, sz);
	buf[sz-1] = 0;

}
int mymin(int a, int b)
{
	if (a < b)return a;
	return b;
}


//
// print
//
static void print(char* p)
{
	if (p == NULL)return;
	printf("%s\r\n", p);
}

static void printheader(const char* p)
{
	char buf[256];
	if (p == NULL)return;
	sprintf(buf, "Content-Type: %s", p);
	print(buf);
	print("");
}


//
// query
//


int get_query_val_int(const char* query, const char* str)
{
	char* p;
	int ret = -1;
	char tmp[128];
	int len;

	if (query == NULL || str == NULL)return ret;
	len = strlen(str);
	if (len > 100)return ret;

	strcpy(tmp, str);
	strcat(tmp, "=");
	p = strstr(query, tmp);
	if (p != query ) {
		strcpy(tmp, "&");
		strcat(tmp, str);
		strcat(tmp, "=");
		p = strstr(query, tmp);
	}
	if (p) {
		sscanf(p + strlen(tmp), "%d", &ret);
		return ret;
	}
	return ret;
}

void get_query_val_string(const char* query, const char* str,char* buf,int sz)
{
	char* p;
	int ret = -1;
	char tmp[128];
	int len;
	if (query == NULL || str == NULL || buf == NULL || sz<1)return;
	len = strlen(str);
	buf[0] = 0;
	if (len> 100)return;

	strcpy(tmp, str);
	strcat(tmp, "=");
	p = strstr(query, tmp);
	if (p != query) {
		strcpy(tmp, "&");
		strcat(tmp, str);
		strcat(tmp, "=");
		p = strstr(query, tmp);
	}
	if (p){
		strncpy(buf, p + strlen(tmp), sz);
		buf[sz - 1] = 0;
	}
	p = strstr(buf, "&");
	if (p)*p = 0;


	urldecode2(buf, buf);
}




//
// check
//

static int checkstr_is_mail(const char* str){
	if (str == NULL)return -1;
	while (*str){
		if (*str >= '0' && *str <= '9'){
			str++;
			continue;
		}
		if (*str >= 'A' && *str <= 'Z'){
			str++;
			continue;
		}
		if (*str >= 'a' && *str <= 'z'){
			str++;
			continue;
		}
		if (*str == '@' || *str == '.'){
			str++;
			continue;

		}
		if (*str == '-' || *str == '_'){
			str++;
			continue;

		}
		return -1;
	}
	return 0;
}
static int checkstr_is_password(const char* str){
	if (str == NULL)return -1;
	while (*str){
		if (*str >= '0' && *str <= '9'){
			str++;
			continue;
		}
		if (*str >= 'A' && *str <= 'Z'){
			str++;
			continue;
		}
		if (*str >= 'a' && *str <= 'z'){
			str++;
			continue;
		}
		// support for STAYLAS
		if (*str == '@' || *str == '+'){
			str++;
			continue;
		}
		// support for STAYLAS
		if (*str == '-' || *str == '_' || *str == '='){
			str++;
			continue;
		}
		return -1;
	}
	return 0;
}


int csvread_check_kiki(const char* no, const char* pass,char* url,int ulen)
{
	HCSVSQL conn = NULL;
	HCSCOL col = NULL;

	char *sql_serv = "localhost";
	char *user = "root";
	char *passwd = "wanted";
	char *db_name = "excel";
	int ret = -1;
	int iidd = 0;


	int db_id = 0, db_en = 0;
	char buf[1024];
	char db_pass[128];
	char* p;

	if (no == NULL || no[0] == 0 || pass == NULL || pass[0] == 0)return ret;
	if (url == NULL || ulen < 1)return ret;

	if (checkstr_is_mail(no) || checkstr_is_password(pass))return ret;
	if (strlen(no) > 100 || strlen(pass) > 100)return ret;

	conn = csvsql_connect(sql_serv, user, passwd, db_name);
	if (conn == NULL)goto end;

	if (strchr(no, '@') || strchr(no, '.'))goto next;
	sscanf(no, "%d", &iidd);
	if (iidd < 1)goto next;

	sprintf(buf, "SELECT * from kiki_list where kikiid=%d", iidd);
	col = csvsql_prepare(conn, buf);
	if (col == NULL)goto next;
	//exit(-1);


	do{
		if (csvsql_step(col) == 0){
			csvsql_free_result(col);
			col = NULL;
			goto next;
		}
		db_id = csvsql_column_int(col, 0);
		db_en = csvsql_column_int(col, 2);
		if (db_en < 1)continue;

		db_pass[0] = 0;
		p = (char*)csvsql_column_char(col, 1);
		if (p == NULL || strlen(p) > 120)continue;

		strcpy(db_pass, p);

		if (strcmp(db_pass, pass) != 0)continue;

		p = csvsql_column_char(col, 6);
		if (p == NULL || strlen(p) > 256)continue;
		strcpy(url, p);


		csvsql_free_result(col);
		col = NULL;

		ret = db_id;
		goto end;

	} while (1);

next:
end:
	if (col != NULL)csvsql_free_result(col);
	if (conn != NULL)csvsql_close(conn);
	return ret;

}


//
// main
//
int main(int argc, char *argv[])
{
	char query[1024*4];
	char buf[1024*4];
	char buf2[1024 * 4];
	char pass[1024];
	char mail[1024];
	char type[256];
	int f_error = 0;
	int f_type = 0;

#if defined(_WIN32) && !defined(__GNUC__)
	//	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_WNDW);
	//	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
	//	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_WNDW);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	setstdoutmode();

	mygetenv(query, "QUERY_STRING", sizeof(query));

	mail[0] = 0;
	get_query_val_string(query, "ki",mail,sizeof(mail));

	pass[0] = 0;
	get_query_val_string(query, "kp", pass, sizeof(pass));

	type[0] = 0;
	get_query_val_string(query, "t", type, sizeof(type));

	buf[0] = 0;

	if (strcmp(type, "json")==0) {
		f_type = 1;
	}

	if (mail[0] != 0 && pass[0] != 0){
		csvread_check_kiki(mail, pass,buf,1024);
	}
	if (buf[0] == 0) {
		f_error = 1;
		strcpy(buf, "error");
	}
	printheader("text/javascript");

	if (strlen(query) < 1024 && f_error==0) {
		strcat(buf, "?");
		strcat(buf, query);
	}

	if (f_type == 0) {
		print(buf);
	}
	else {
		buf2[0] = 0;
		sprintf(buf2, "ws_url=\"%s\";",buf);
		print("declare_var_begin(ws_url = null, ws_url);");
		print(buf2);
		print("declare_var_end(ws_url);");
	}

	return 0;
}