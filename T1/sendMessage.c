#include "sendMessage.h"

unsigned int sentPackets = 0;

int sendMessage(int fd, unsigned char* message, int length) {

	int bytesWritten = write(fd, message, length);
	if(bytesWritten != length){
		perror("sendMessage: failed\n");
	 	return -1;
	}

	lastPackage = message;
	lastPackageSize = length;
	sentPackets++;
	alarm(3);

	return 0;
}
