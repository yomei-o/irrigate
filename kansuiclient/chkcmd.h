#ifndef CHKCMD_H_
#define CHKCMD_H_

#ifdef __cplusplus
extern "C"
{
#endif

void mychkcmd_print(const char* buf);
void mychkcmd_clear_print_buffer();
const char* mychkcmd_get_print_buffer();
void mychkcmd_execute_command(const char* cmd);


#ifdef __cplusplus
}
#endif



#endif
