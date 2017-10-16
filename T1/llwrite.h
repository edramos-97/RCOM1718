#ifndef _LLWRITE_H_
#define _LLWRITE_H_

#include "utils.h"
#include "sendMessage.h"

int llwrite(int fd, unsigned char* buffer, int length);
unsigned char* createTail(unsigned char* buffer, int length);
unsigned char* dataPackaging(unsigned char* buffer, int length);
unsigned char* createHeader(char c);
unsigned char* byteStuffing(unsigned char* buffer, int* length);
int stateMachineReadAnswer(int fd);
void verifyConditions(unsigned char received_C);


#endif
