#ifndef DF_WSCLIENT_H_
#define DF_WSCLIENT_H_

#ifdef __cplusplus
extern "C"
{
#endif


void mywebsocket_init();
void mywebsocket_done();

int mywebsocket_connect(const char* url);
void mywebsocket_close(int s);
int mywebsocket_wait_time(int s,int tt, int(*f)(void* vp,int type, void* data, int sz),void* vp);
int mywebsocket_wait(int s, int(*f)(void* vp, int type, void* data, int sz), void* vp);
int mywebsocket_send_string(int fd, const char* data);



#ifdef __cplusplus
}
#endif

#endif


