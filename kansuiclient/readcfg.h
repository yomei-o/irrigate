#ifndef DF_READ_CFG_H_
#define DF_READ_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


void readcfg(const char*file, const char* defstr, char* out, int osz);
void get_ws_url(char* out, int osz);

#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif




