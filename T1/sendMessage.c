#include "sendMessage.h"

int sendMessage(int fd, unsigned char* message, int length) {

	int bytesWritten = write(fd, message, length);
	if(bytesWritten != length){
		perror("sendMessage: failed\n");
	 	return -1;
	}

	lastPackage = message;
	lastPackageSize = length;
	alarm(3);
	
	return 0;
}
