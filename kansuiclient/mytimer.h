#ifndef MYTIMER_H_
#define MYTIMER_H_

#ifdef __cplusplus
extern "C"
{
#endif

int hello(int argc, char* argv[]);
int version(int argc, char* argv[]);
	
int gettm(int argc, char* argv[]);
int settm(int argc,char* argv[]);
int deltm(int argc, char* argv[]);
int deltmall(int argc, char* argv[]);
int gettmnum(int argc, char* argv[]);
int gettmstate(int argc, char* argv[]);

int getdate(int argc, char* argv[]);
int setdate(int argc, char* argv[]);

int mytimer_init();
int mytimer_done();
int mytimer_reset();

void  mytimer_set_onfunc(void  (*pfunc)(int,void*), void* vp);
void  mytimer_set_offunc(void  (*pfunc)(int,void*), void* vp);

void mytimer_execute_command(const char* cmd);
const char* mychkcmd_get_print_buffer();

#ifdef __cplusplus
}
#endif



#endif
