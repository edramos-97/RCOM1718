#include "sendMessage.h"
#include "utils.h"
#include "llwrite.h"
#include "llopen.h"
#include "llread.h"
#include "llclose.h"


int main(int argc, char** argv) {

	if (argc != 4){
		perror("Wrong number of arguments");
		return -1;
	}

	(void) signal(SIGALRM, writeTimeOut);

	if(atoi(argv[2]) == TRANSMITTER) {

		//OPEN AND READ FILE TO BUFFER
		FILE *input;
		input = fopen(argv[3],"rb");

		if(input == NULL){
			perror("Input error");
			exit(-1);
		}
		int fileSize = fsize(input);
		unsigned char buffer[fileSize];
		fread(buffer,sizeof(char), fileSize, input);
		fclose(input);
		//FILE READ TO BUFFER

		//OPENING SERIAL PORT
		llopen(argv[1],TRANSMITTER);

		unsigned int length = 0;

		printf("\nSEND START PACK\n");
		unsigned char * controlBuff = controlPacking(C_START,fileSize,argv[3],strlen(argv[3]),&length);
		llwrite(fd,controlBuff,length);
		free(controlBuff);
		printf("\nSENT START PACKET SUCCESSFULLY\n\n");

		unsigned int index = 0;
		unsigned int blocks = fileSize / MAX_SIZE;
		unsigned int rest = fileSize % MAX_SIZE;
		unsigned int total = blocks*MAX_SIZE+rest;
		printf("Sending %d bytes over %d packages of %d bytes and 1 package of %d bytes\n",total,blocks,MAX_SIZE,rest);

		unsigned char * dataBuff;

		//SENDING FILE ON BLOCKS
		for (index = 0; index < blocks; index++) {
			dataBuff = dataPackaging(buffer+(MAX_SIZE*index), MAX_SIZE);
			llwrite(fd,dataBuff,MAX_SIZE+PACKING_HEADER_SIZE);
			free(dataBuff);
		}

		//SENDING REST OF FILE
		if(rest > 0) {
			dataBuff = dataPackaging(buffer+MAX_SIZE*index, rest);
			printf("Sending package no %d", index + 1);
			llwrite(fd,dataBuff,rest+PACKING_HEADER_SIZE);
		}

		printf("\nSEND END PACKET\n");
		controlBuff = controlPacking(C_END,fileSize,argv[3],strlen(argv[3]),&length);
		llwrite(fd,controlBuff,length);
		free(controlBuff);
		printf("\nSENT END PACKET SUCCESSFULLY\n\n");

		if(llclose(fd, TRANSMITTER)<0){
			printf("Error to diconnected!\n");
			return -1;
		}
	}

	if(atoi(argv[2]) == RECEIVER){

		//OPENING SERIAL PORT
		llopen(argv[1],RECEIVER);

		unsigned int length = 1;

		printf("\nWAITING FOR START PACKET\n");
		unsigned char* controlBuff = malloc(length);
		controlBuff = llread(fd,controlBuff,&length);
		printf("\nRECIEVED START PACKET\n");

		unsigned int fileSize=0;
		char * fileName;

		//ANALYSING START PACKET INFO
		fileName = readControllPacket(controlBuff, C_START, &fileSize);
		if(fileName == NULL) {
			perror("FileName error");
			exit(-1);
		}

		printf("Size: %d\n",fileSize);
		printf("Name: %s\n",fileName);

		unsigned char* buff = malloc(fileSize);
		unsigned int index = 0;
		unsigned int dataBuffLength = 1;
		unsigned char* dataBuff;

		//
		while (TRUE) {
			dataBuff = malloc(dataBuffLength);
			printf("Gonna try to read the block no: %d\n", index);
			dataBuff = llread(fd,dataBuff,&dataBuffLength);

			if (dataBuff == NULL){
				free(dataBuff);
				continue;
			}

			if (dataBuff[0] == C_END) {
				printf("Received END\n\n");
				break;
			}

			if (dataBuff[0] != 0x00){
				printf("Packet read after START did not contain data END nor DATA identifier\n");
			}

			length = dataBuff[3] + dataBuff[2]*256;
			printf("Block length: %d\n", length);

			unsigned int i;
			for (i = 0; i < length; i++) {
				buff[index+i] = dataBuff[i+4];
			}
			index += length;
			free(dataBuff);
		}

		FILE * output = fopen(fileName,"wb");

		if(output == NULL)
			perror("output error");

		fwrite(buff,sizeof(unsigned char),fileSize,output);

		if(llclose(fd, RECEIVER)<0){
			printf("Error to diconnected!\n");
			return -1;
		}

		fclose(output);
	}

	printf("\nGonna exit\n\n");

	return 0;
}
