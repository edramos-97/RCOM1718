#ifndef _LLREAD_H_
#define _LLREAD_H_

#include "utils.h"

unsigned char* byteDestuffing(unsigned char* buffer, unsigned int* length);
int stateMachineRead(int fd);

char duplicate_flag = FALSE;
char sequence_number_read = 1;

int llread(int fd, unsigned char* buffer) {

	stateMachineRead(fd);

	printf("passou state\n");
	
	buffer = realloc(buffer,512);
	unsigned int i = 0;
	while(read(fd, &buffer[i], 1)) {
		if(buffer[i] == FLAG)
			break;
		i++;
	}

	 if(i < 2){
	 	printf("Error: no data in package!");
	 	return -1;
	 }
	unsigned int length = i - 1;

	for(i =0; i<length; i++)
    	printf("buffer no DESTUFF: %x\n",buffer[i]);

	char bcc_received = buffer[length];

	buffer = byteDestuffing(buffer, &length);

	for(i =0; i<length; i++)
    	printf("buffer: %x\n",buffer[i]);

	unsigned char bcc = 0;
	 for(i = 0; i < length; i++) {
		bcc ^= buffer[i];
	 }

	printf("bcc = %x", bcc_received);

	 if(bcc == bcc_received){
	 	if(duplicate_flag){
			printf("bcc ok! duplicate tho!\n");
			//enviar RR
			return 0;
		}
		printf("bcc ok!\n");
		//enviar RR
		return length;
	 }
	 else {
		if(duplicate_flag){
			printf("bcc not ok! duplicate tho!\n");
			//enviar RR
			return 0;
		}
	 	//enviar REJ
		printf("bcc not ok!\n");
	 }




	return 0;
}

unsigned char* byteDestuffing(unsigned char* buffer,unsigned int* length){
	unsigned char* buff = malloc(*length);
	int new_length = *length;
	unsigned int i, j=0;

	//buff[0] = buffer[0];

	for(i = 0; i<(*length)-1; i++, j++){
		if(buffer[i] == 0x7D){
			i++;
			if(buffer[i]==0x5E){
				buff[j] = 0x7E;
				new_length--;
			}
			if(buffer[i]==0x5D){
				buff[j]= 0x7D;
				new_length--;
			}
		}else{
			buff[j] = buffer[i];
		}
	}

	buff[j] = buffer[(*length)-1];

/*
	printf("byteDestuffingFunction: \n");
	for(i =0; i<length; i++)
		printf("buffer: %x\n",buffer[i]);

	for(i =0; i<new_length; i++)
		printf("buff: %x\n",buff[i]);*/
	*length = new_length;

	return buff;
}

int stateMachineRead(int fd) {
	
	unsigned char received, received_A, received_C;
	int state = 0;
	
	while(state != 4) {
		read(fd, &received, 1);
		printf("state %d : 0x%x\n", state, received);
		
		switch(state) {
		      case 0:
		      if(received == FLAG) state = 1;
		      break;

		      case 1:
		      if(received == A) {
			state = 2;
			received_A = received;
		      }
		      else if (received == FLAG) state = 1;
		      else state = 0;
		      break;

		      case 2:
			if(received == C_INFO(0) || received == C_INFO(1)) {			
		      	state = 3;
			received_C = received;}
		      else if (received == FLAG) state = 1;
		      else state = 0;
		      break;

		      case 3:
		      if(received == (received_A ^ received_C)) state = 4;
		      else state = 0;
		      break;
		    }
		  }

	if(received_C == C_INFO(sequence_number_read)){
		printf("package duplicado\n");
		duplicate_flag = TRUE;}
	else{
		if(sequence_number_read == 0){
			sequence_number_read = 1;
		}
		else{
			sequence_number_read = 0;
		}
		duplicate_flag = FALSE;
	}

  return 0;
}
 


#endif
