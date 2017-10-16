#ifndef _LLREAD_H_
#define _LLREAD_H_

#include "utils.h"
#include "llwrite.h"

unsigned char* byteDestuffing(unsigned char* buffer, unsigned int* length);
int stateMachineRead(int fd);
int sendHeader(unsigned char c);
int llread(int fd, unsigned char* buffer);






#endif
