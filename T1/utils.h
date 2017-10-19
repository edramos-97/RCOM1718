#ifndef _UTILS_H_
#define _UTILS_H_

#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 		0
#define TRUE 		1

#define TRANSMITTER	0
#define RECEIVER	1

//application
#define PACKING_HEADER_SIZE 4
#define C_START 0X02
#define C_END 0x03
#define C_DATA 0X00
#define MAX_SIZE 100
#define TIMEOUT		3
#define NUMTRIES	3

#define SUPERVISION_SIZE	5
#define INFO_HEADER_SIZE	4
#define INFO_TAIL_SIZE		2

#define FLAG 		0x7E
#define A 			0x03
#define C_SET 		0x03
#define C_UA 		0x07

#define C_INFO(x) (0b00000000 + ((x) << 6))
#define C_RR(x) (0b00000101 + ((x) << 7))
#define C_REJ(x) (0b00000001 + ((x) << 7))

extern unsigned char* lastPackage;
extern unsigned int lastPackageSize;
extern unsigned char sequenceNumber;
extern unsigned char numberTries;
extern int fd;

void writeTimeOut();
int fsize(FILE* file);

#endif
