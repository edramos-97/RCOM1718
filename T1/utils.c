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
    printf("\nChamada # %d acabou\n", numberTries);
    numberTries++;
    if(write(fd,lastPackage,lastPackageSize) < 0){
        printf("Write timed out and can't write to FD! Exiting...\n");
    }
    alarm(3);
  }
  if (numberTries == 3){
    printf("\nFALHOU");
    exit(-1);
  }
}
