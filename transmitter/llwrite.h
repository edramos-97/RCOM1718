#ifndef _LLWRITE_H_
#define _LLWRITE_H_

#include "utils.h"

unsigned char* createTail(unsigned char* buffer, int length);
unsigned char* dataPackaging(unsigned char* buffer, int length);
unsigned char* createHeader(char c);
unsigned char* byteStuffing(unsigned char* buffer, int* length);
int stateMachineReadAnswer(int fd);
void verifyConditions(unsigned char received_C);

int llwrite(int fd, unsigned char* buffer, int length){

int n_chars = 0;

while(!sucessLastPackage){

  int numberArgs = length;
  unsigned char* trama = dataPackaging(buffer, numberArgs);

  numberArgs += PACKING_HEADER_SIZE;
  unsigned char* pack =  createHeader(C_INFO(sequenceNumber));

  unsigned char* tail = createTail(trama, numberArgs);

unsigned int i;
pack = realloc(pack,numberArgs + INFO_HEADER_SIZE);
for(i=0; i < numberArgs; i++)
    pack[INFO_HEADER_SIZE+i] = trama[i];
numberArgs += INFO_HEADER_SIZE;
free(trama);


pack = realloc(pack,numberArgs + INFO_TAIL_SIZE);
for(i=0; i < INFO_TAIL_SIZE; i++)
    pack[numberArgs+i] = tail[i];
numberArgs += INFO_TAIL_SIZE;
free(tail);

pack = byteStuffing(pack, &numberArgs);


//Pack to send:
for(i =0; i<numberArgs; i++)
    printf("pack: %x\n",pack[i]);

	n_chars = sendMessage(fd, pack, numberArgs);
	if(n_chars < 0){
		return -1;
	}
	
	stateMachineReadAnswer(fd);
	 printf("passou state machine\n");
}
sucessLastPackage = FALSE;
	return n_chars;

}

unsigned char* createHeader(char c){
	unsigned char* header = malloc(INFO_HEADER_SIZE);
	header[0] = FLAG;
	header[1] = A;
	header[2] = c;
	header[3] = header[1] ^ header[2];

	/*
	printf("createHeaderFunction: \n");
	unsigned int i;
	for(i =0; i < 4; i++)
		printf("header: %x\n", header[i]);
	printf("\n \n");*/

	return header;
}

unsigned char* createTail(unsigned char* buffer, int length){
	unsigned char* tail = malloc(INFO_TAIL_SIZE);
	tail[0] = 0;
	unsigned int i;

	for(i = 0; i < length; i++)
		tail[0] ^= buffer[i];

	tail[1] = FLAG;

	return tail;
}

unsigned char* dataPackaging(unsigned char* buffer, int length){
	unsigned char* header = malloc(PACKING_HEADER_SIZE+length);
	header[0] = 0x01; //  Verificar estes
	header[1] = 0x01; //  dois valores
	header[2] = (length / 256); //  Verificar estes
	header[3] = (length % 256); //  dois valores

	unsigned int i;
	for(i=0;i<length;i++){
		header[PACKING_HEADER_SIZE+i] = buffer[i];
	}

	/*
	printf("dataPackagingFunction: \n");
	for(i =0; i < 4+length; i++)
		printf("header: %x\n", header[i]);*/
	printf("\n \n");

	return header;
}

unsigned char* byteStuffing(unsigned char* buffer, int* length) {
	unsigned char* buff = malloc(*length);

	int new_length = *length;
	unsigned int i, j = 1;

	buff[0] = buffer[0];

	for(i = 1; i<*length-1; i++, j++){
		if(buffer[i] == FLAG){
			buff[j] = 0x7D;
			new_length++;
			j++;
			buff = realloc(buff,new_length);
			buff[j]= 0x5E;
		}else if(buffer[i] == 0x7D){
			buff[j] = buffer[i];
			new_length++;
			j++;
			buff = realloc(buff,new_length);
			buff[j] = 0x5D;
		} else{
			buff[j] = buffer[i];
		}
	}

	buff[j] = buffer[*length-1];

	/*printf("bitStuffingFunction: \n");
	printf("length nova : %d\n",new_length);
	for(i =0; i<*length; i++)
		printf("buffer: %x\n",buffer[i]);

	for(i =0; i<new_length; i++)
		printf("buff: %x\n",buff[i]);
	printf("\n \n");*/

	*length = new_length;
	return buff;
}



unsigned char* controlPacking(unsigned char c, unsigned int fileSize, 
	char* name, unsigned char nameSize){

	unsigned int length = 5*sizeof(char)+sizeof(int)+nameSize;
	unsigned char* buff = malloc(length);

	buff[0] = c;
	buff[1] = 0x00;
	buff[2] = sizeof(int);

	unsigned int* int_location = (unsigned int *)(&buff[3]);
	*int_location = fileSize;
	
	buff[7]=0x01;
	buff[8]=nameSize;

	unsigned int i;
	for(i = 0; i < nameSize; i++)
    	buff[9+i] = name[i];
    
	for(i =0; i<9+nameSize; i++)
    	printf("buff: %x\n",buff[i]);

    return buff;
}

int stateMachineReadAnswer(int fd) {
	
	unsigned char received, received_A, received_C;
	int state = 0;

	printf("state machine\n");
	
	while(state != 5) {
		printf("STATE MACHINE!!!!!!!");
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
      if(received == C_RR(1) || received == C_RR(0) || 
      	received == C_RJ(1) || received == C_RJ(0)) {
        state = 3;
        received_C = received;
      }
      else if (received == FLAG) state = 1;
      else state = 0;
      break;
      case 3:
      if(received == (received_A ^ received_C)) state = 4;
      else state = 0;
      break;
      case 4:
      if(received == FLAG) state = 5;
      else state = 0;
      break;
    }
	}
  
  verifyConditions(received_C);

  alarm(0);
  numberTries = 0;
  return 0;
}

void verifyConditions(unsigned char received_C) {
  	if(received_C == C_RR(0)){
		printf("RR = 0\n");
		sequenceNumber = 0;
		sucessLastPackage = TRUE;
  	}
	else if(received_C == C_RR(1)){
		printf("RR = 1\n");
		sequenceNumber = 1;
		sucessLastPackage = TRUE;
	}
	 else if(received_C == C_RJ(0)){
		printf("RJ = 0\n");
		sequenceNumber = 0;
		sucessLastPackage = FALSE;
	}
	else if(received_C == C_RJ(1)){
		printf("RJ = 1\n");
		sequenceNumber = 1;
		sucessLastPackage = FALSE;
	}
}

#endif
