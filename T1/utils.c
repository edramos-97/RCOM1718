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
    printf("\nChamada # %d acabou", numberTries);
    numberTries++;
    write(fd,lastPackage,lastPackageSize);
    alarm(3);
  }
  if (numberTries == 3){
    printf("\nFALHOU");
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
