#include "utils.h"

 unsigned char* lastPackage;
 unsigned int lastPackageSize;
 unsigned char sequenceNumber = 0;
 unsigned char numberTries = 0;
 int fd;

int flag_recebeu = 0;

double totalTime = 0;

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

float convertToMiliseconds(struct timespec t){
	float res;
	res =(t.tv_sec * 1000);
	res+=((float)t.tv_nsec / 1000000);
	return res;
}
