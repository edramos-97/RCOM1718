#ifndef _UTILS_H_
#define _UTILS_H_

#include <sys/stat.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 		0
#define TRUE 		1

#define TRANSMITTER	0
#define RECEIVER	1

#define SUPERVISION_SIZE	5
#define PACKING_HEADER_SIZE 4
#define INFO_HEADER_SIZE	4
#define INFO_TAIL_SIZE		2

#define FLAG 		0x7E
#define A 			0x03
#define C_SET 		0x03
#define C_UA 		0x07

#define C_INFO(x) (0b00000000 + ((x) << 6))
#define C_RR(x) (0b00000101 + ((x) << 7))
#define C_RJ(x) (0b00000001 + ((x) << 7))

#define TIMEOUT		3
#define NUMTRIES	3

unsigned char* lastPackage;
unsigned int lastPackageSize;
unsigned char sucessLastPackage = FALSE;
unsigned char sequenceNumber = 0;
unsigned char numberTries = 0;

int flag_recebeu = 0;
int fd;

void writeTimeOut()
{
  if(flag_recebeu == 0 && numberTries < 4){
    printf("chamada # %d acabou\n", numberTries);
    numberTries++;
    write(fd,lastPackage,lastPackageSize);
    alarm(3);
  }
  if (numberTries == 3){
    printf("FALHOU\n");
    exit(-1);
  }
}

int fsize(FILE* file) {
    long int currPos = ftell(file);

    if (fseek(file, 0, SEEK_END) == -1){
        perror("file size: ");
		return -1;
	}

	// saving file size
	long int size = ftell(file);

	// seeking to the previously saved position
	fseek(file, 0, currPos);

	// returning size
return size;
}
#endif