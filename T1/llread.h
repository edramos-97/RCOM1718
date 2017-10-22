#ifndef _LLREAD_H_
#define _LLREAD_H_

#include "utils.h"
#include "llwrite.h"

/**
*	Destuffing the data buffer received.
**/
unsigned char* byteDestuffing(unsigned char* buffer, unsigned int* length);


/**
*	State Machine to validate fields
**/
int stateMachineRead(int fd);


/**
*	Send header with answer.
**/
int sendHeader(unsigned char c);


/**
*	Main function to read data.
**/
unsigned char* llread(int fd, unsigned char* buffer,unsigned int* length);


/**
*	Read data of control buffer.
**/
char* readControllPacket(unsigned char* controlBuff,char controll, unsigned int * filesize);


/**
*	Switch sequence number for next package.
**/
char switchSequenceNumber(char previous_num);


#endif
