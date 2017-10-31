#include "utils.h"
#include "llwrite.h"
#include "llopen.h"
#include "llread.h"
#include "llclose.h"
#include "appLayerUtils.h"


int main(int argc, char** argv) {

	if(atoi(argv[2]) && argc != 3){
		printf("Expected 2 arguments, received %d\n", argc - 1);
		return -1;
	}
	else if (!atoi(argv[2]) && argc != 4){
		printf("Expected 3 arguments, received %d\n", argc - 1);
		return -1;
	}

	(void) signal(SIGALRM, writeTimeOut);
	(void) signal(SIGUSR1, disconnectHandler);

	if(atoi(argv[2]) == TRANSMITTER) { // TRANSMITTER

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

		printf("\nSEND START PACK\n\n");
		unsigned char * controlBuff = controlPacking(C_START,fileSize,argv[3],strlen(argv[3]),&length);
		llwrite(fd,controlBuff,length);
		free(controlBuff);
		printf("\nSENT START PACKET SUCCESSFULLY\n\n");

		unsigned int index = 0;
		unsigned int blocks = fileSize / MAX_SIZE;
		unsigned int rest = fileSize % MAX_SIZE;
		unsigned int total = blocks*MAX_SIZE+rest;
		printf("\nSending %d bytes over %d packages of %d bytes and 1 package of %d bytes\n",total,blocks,MAX_SIZE,rest);

		unsigned char * dataBuff;

		clock_t begin = clock();

		//SENDING FILE ON BLOCKS
		for (index = 0; index < blocks; index++) {
		  packageNumber = (packageNumber+1) % 256;
			dataBuff = dataPackaging(buffer+(MAX_SIZE*index), MAX_SIZE);
			printf("\n\nPackage number:  %d\n",packageNumber);
			llwrite(fd,dataBuff,MAX_SIZE+PACKING_HEADER_SIZE);
			free(dataBuff);
		}

		//  // END PACKET BEFORE IT SHOULD - FOR TESTING PURPOSES
		//  printf("\nSEND END PACKET\n\n");
		//  controlBuff = controlPacking(C_END,fileSize,argv[3],strlen(argv[3]),&length);
		//  llwrite(fd,controlBuff,length);
		//  free(controlBuff);
		//  printf("\nSENT END PACKET SUCCESSFULLY\n\n");

		//  // LLCLOSE BEFORE IT SHOULD - FOR TESTING PURPOSES
		//  if(llclose(fd, TRANSMITTER)<0){
		//  	printf("Error to diconnected!\n");
		//  	return -1;
		//  }
		//  return 0;


		//SENDING REST OF FILE
		if(rest > 0) {
			packageNumber = (packageNumber+1) % 256;
			dataBuff = dataPackaging(buffer+MAX_SIZE*index, rest);
			printf("\n\nPackage number:  %d\n",packageNumber);
			llwrite(fd,dataBuff,rest+PACKING_HEADER_SIZE);
		}

		printf("\nSEND END PACKET\n\n");
		controlBuff = controlPacking(C_END,fileSize,argv[3],strlen(argv[3]),&length);
		llwrite(fd,controlBuff,length);
		free(controlBuff);
		printf("\nSENT END PACKET SUCCESSFULLY\n\n");

		clock_t end = clock();

		double time_spent = (double)(end-begin)/CLOCKS_PER_SEC;

		printf("Time spend sending data: %f\n",time_spent );
		printf("Number of packets sent: %d\n",sentPackets);
		printf("Number of packets failed: %d\n",failPackets);

		if(llclose(fd, TRANSMITTER)<0){
			printf("Error to diconnected!\n");
			return -1;
		}
	}

	if(atoi(argv[2]) == RECEIVER){ // RECEIVER
		unsigned int blockCounter = 0;
		//OPENING SERIAL PORT
		llopen(argv[1],RECEIVER);

		unsigned int length = 1;

		printf("\nWAITING FOR START PACKET\n");
		unsigned char* controlBuff = malloc(length);
		controlBuff = llread(fd,controlBuff);
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

		//READING FILE BLOCKS
		while (TRUE) {
			dataBuff = malloc(dataBuffLength);
			clock_gettime(CLOCK_MONOTONIC_RAW, &time_init);
			dataBuff = llread(fd,dataBuff);
			clock_gettime(CLOCK_MONOTONIC_RAW, &time_curr);
			totalTime += convertToMiliseconds(time_curr)-convertToMiliseconds(time_init);

			//vERIFING FAILED READ
			if (dataBuff == NULL){
				free(dataBuff);
				continue;
			}

			//CHECKING FOR END OF DATA MESSAGES
			if (dataBuff[0] == C_END) {
				unsigned int fileSizeEnd;
				readControllPacket(dataBuff, C_END, &fileSizeEnd);
				if (fileSizeEnd != fileSize)
					// fileSize differs
					printf("Warning: START and END file size information differ.\n");
				else if(fileSizeEnd > index){
					// didn't read whole file
					free(buff);
					printf("Received END before the whole file size was read.\n");
					if(llclose(fd, RECEIVER)<0)
						printf("Error to diconnect!\n");
					printf("File was not created\n");
					return -1;
				}
				printf("Received END\n\n");
				break;
			}

			//UNEXPECTED IDENTIFIER IN DATA PACKET
			if (dataBuff[0] != 0x00){
				printf("Packet read after START did not contain data END nor DATA identifier\n");
			}

			printf("Trying to reaaplication.c:49:1: error: expd the block no: %d\n", dataBuff[1] + blockCounter * 256);

			if(dataBuff[1] == 255)
				blockCounter++;

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
			printf("Error to diconnect!\n");
			return -1;
		}

		fclose(output);
	}

	printf("\nGonna exit\n\n");

	return 0;
}
