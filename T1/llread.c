#include "llread.h"

char duplicate_flag = FALSE;
char sequence_number_read = 0;

unsigned char *  llread(int fd, unsigned char* buffer, unsigned int* length) {

	while(1){
	stateMachineRead(fd);

	printf("passou state\n");

	unsigned int i = 0;
	while(read(fd, &buffer[i], 1)) {
		// printf("Buffer[%d]: %x\n", i, buffer[i]);
		if(buffer[i] == FLAG)
			break;
		i++;
	}

	printf("Sai do read...\n");

	 if(i < 2){
	 	printf("Error: no data in package!");
	 	return NULL;
	 }
	*length = i - 1;

	//print buffer
	//for(i =0; i<length; i++)
    	//printf("buffer no DESTUFF: %x\n",buffer[i]);
			//
			// printf("byteDestuffingFunction: \n");
			// for(i =0; i<*length; i++)
			// 	printf("buffer no DESTUFF %d: %x\n",i,buffer[i]);
			// printf("buffer: %d\n",*length);


	buffer = byteDestuffing(buffer, length);

	printf("Buffer destuffed\n");

	// printf("byteDestuffingFunction after: \n");
	// for(i =0; i<*length; i++)
	// 	printf("buffer DESTUFF %d: %x\n",i,buffer[i]);
	// printf("buffer: %d\n",*length);
	//
	// printf("TAMANHO apos destuffing: %d \n",*length);

	// for(i=1970; i < *length ; i++)
	// 	printf("buff_end: %x\n", buffer[i]);
	//
	// 	printf("index bcc : %d\n",*length+1);


	unsigned char bcc_received = buffer[*length];

	//print buffer
	//for(i =0; i<length; i++)
    //	printf("buffer: %x\n",buffer[i]);

	unsigned char bcc = 0;
	 for(i = 0; i < *length; i++) {
		bcc ^= buffer[i];
	 }

	printf("bcc = %x\n", bcc_received);

	 if(bcc == bcc_received){
	 	if(duplicate_flag){
			printf("data bcc ok! duplicate tho!\n");
			if(sendHeader(C_RR(sequence_number_read))<0)
				return NULL;
		} else {
			printf("data bcc ok!\n");
			if(sendHeader(C_RR(sequence_number_read))<0)
				return NULL;
			return buffer;
		}
	 }
	 else {
		if(duplicate_flag){
			printf("data bcc not ok! duplicate tho!\n");
			if(sendHeader(C_RR(sequence_number_read))<0)
				return NULL;
		} else {
			printf("data bcc not ok!\n");
			if(sendHeader(C_REJ(sequence_number_read))<0)
				return NULL;
		}
	 }
 }
 return buffer;
}

unsigned char* byteDestuffing(unsigned char* buffer,unsigned int* length){
	printf("Length: %d", *length);
	printf("Buffer: %d", buffer == NULL);
	printf("Gonna malloc the buff\n");
	unsigned char* buff = malloc(*length);
	printf("buff malloc'd\n");
	unsigned int new_length = *length;
	unsigned int i, j=0;

	//buff[0] = buffer[0];

	for(i = 0; i<(*length); i++, j++){
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

	buff[j] = buffer[(*length)];


	/*printf("byteDestuffingFunction: \n");
	for(i =0; i<(*length); i++)
		printf("buffer %d: %x\n",i,buffer[i]);
	printf("buffer: %d\n",*length);

	for(i =0; i<new_length; i++)
		printf("buff %d: %x\n",i,buff[i]);
	printf("buffer new: %d\n",new_length);*/

	*length = new_length;

	printf("Length is: %d\n", *length);

	//buff = realloc(buff, *length);

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

	if(received_C != C_INFO(sequence_number_read)){
		printf("package duplicado com : %x\n", received_C);
		duplicate_flag = TRUE;}
	else{

		if(sequence_number_read == 0){
			sequence_number_read = 1;}
		else{
			sequence_number_read = 0;}
		duplicate_flag = FALSE;
	}

  return 0;
}

int sendHeader(unsigned char c){
	unsigned char* buff = malloc(5);
	buff = createHeader(c);
	buff[4] = FLAG;
	if(write(fd,buff,5)!= 5){
		perror("sendHeader went wrong");
		// free(buff);
		return -1;}
	printf("sent header with %x\n", c);
	// free(buff);
	return 0;
}
