#ifndef DF_EASYHTTPCLIENT_H_
#define DF_EASYHTTPCLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


void myhttp_init();
void myhttp_done();
void myhttp_free_data(void* vp);
int myhttp_get_data(const char* url, void** data, int* sz, int* rescode);


#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif




