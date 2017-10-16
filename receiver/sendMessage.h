#ifndef _SEND_MESSAGE_H_
#define _SEND_MESSAGE_H_

int sendMessage(int fd, unsigned char* message) {

	int bytesWritten = write(fd, message, 5);
	
	if(bytesWritten != 5){
		perror("sendMessage: failed\n");
	 	return -1;
	}
	return 0;
}

#endif