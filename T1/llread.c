#include "llread.h"

char duplicate_flag = FALSE;
char sequence_number_read = 0;

unsigned char *  llread(int fd, unsigned char* buffer, unsigned int* length) {

	while(1) {

		stateMachineRead(fd);
		printf("Recebeu link layer Header\n");
		unsigned int new_length = 1;
	unsigned int i = 0;
	while(read(fd, &buffer[i], 1)) {
		if(buffer[i] == FLAG)
			break;
		new_length++;
		buffer = realloc(buffer,new_length);
		i++;
	}

	 if(i < 2){
	 	printf("Error: no data in package!");
	 	return NULL;
	 }

	buffer = byteDestuffing(buffer, &new_length);

	unsigned char bcc_received = buffer[new_length-2];

	unsigned char bcc = 0;
	 for(i = 0; i < new_length-2; i++) {
		bcc ^= buffer[i];
	 }

	printf("bcc expected= %x; bcc received= %x\n",bcc , bcc_received);

	if(duplicate_flag) {
		printf("Duplicate Pack!\n");
		if(sendHeader(C_RR(sequence_number_read))<0)
			return NULL;
	}
	else	if(bcc == bcc_received) {
		printf("Data bcc ok!\n");
		sendHeader(C_RR(sequence_number_read));
		return buffer;
	}
	else {
		printf("Data not bcc ok!\n");
		sequence_number_read = switchSequenceNumber(sequence_number_read);
		sendHeader(C_REJ(sequence_number_read));
		return NULL;
	}
 }

 return buffer;
}

unsigned char* byteDestuffing(unsigned char* buffer,unsigned int* length){
	unsigned char* buff = malloc(*length);
	unsigned int new_length = *length;
	unsigned int i, j=0;

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

	*length = new_length;

	return buff;
}

int stateMachineRead(int fd) {

	unsigned char received, received_A, received_C;
	int state = 0;

	while(state != 4) {
		read(fd, &received, 1);

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

	if(received_C == C_INFO(1)){
		if(sequence_number_read==0){duplicate_flag = TRUE;}
		sequence_number_read = 0;
	}
	else if(received_C == C_INFO(0)){
		if(sequence_number_read == 1){duplicate_flag = TRUE;}
		sequence_number_read = 1;
	}

  return 0;
}

int sendHeader(unsigned char c){
	unsigned char* buff = malloc(SUPERVISION_SIZE);
	buff = createHeader(c);
	buff[4] = FLAG;

	if(write(fd,buff,5)!= 5){
		perror("sendHeader went wrong");
		return -1;
	}
	printf("Sent response with %x\n", c);

	alarm(0);
	return 0;
}

char* readControllPacket(unsigned char* controlBuff, char controll, unsigned int * fileSize){
	char * fileName;
	unsigned int * size;

	if(controlBuff[0]!=controll)
	{
		printf("Packet read  did not contain the expected identifier\n");
		return NULL;
	}
	else if(controlBuff[1]==0x00)
	{
		size = (unsigned int*)(controlBuff+3);
		fileName = malloc((unsigned int)controlBuff[8]+1);
		memcpy(fileName,controlBuff+9,(unsigned int)controlBuff[8]);
	}
	else if(controlBuff[1]==0x01)
	{
		fileName = malloc((unsigned int)controlBuff[2]+1);
		memcpy(fileName,controlBuff+3,(unsigned int)controlBuff[2]);
		fileSize = (unsigned int*)(controlBuff+(unsigned int)controlBuff[2]+2);
	}
	else
	{
		printf("Controll packet contained an unkown Type\n");
		return NULL;
	}

	*fileSize = *size;
	return fileName;
}


char switchSequenceNumber(char previous_num){
	if(previous_num == 0)
		return 1;

	if (previous_num == 1)
		return 0;

	printf("Sequence number is out of range\n");
	return -1;
}
