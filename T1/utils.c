#include "utils.h"

 unsigned char* lastPackage;
 unsigned int lastPackageSize;
 unsigned char sequenceNumber = 0;
 unsigned char numberTries = 0;
 int fd;

int flag_recebeu = 0;

void writeTimeOut()
{
  if(flag_recebeu == 0 && numberTries < 4){
    printf("\nPing # %d is over\n", numberTries);
    numberTries++;
    if(write(fd,lastPackage,lastPackageSize) < 0){
        printf("Write timed out and can't write to FD! Exiting...\n");
    }
    alarm(3);
  }
  if (numberTries == 3){
    printf("\nFAILURE\n\n");
    exit(-1);
  }
}
