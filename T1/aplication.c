#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "sendMessage.h"
#include "utils.h"
#include "llwrite.h"
#include "llopen.h"
#include "llread.h"


unsigned int j = 0,i=0;

int main(int argc, char** argv) {
	if (argc != 4){
		perror("Wrong number of arguments");
		return -1;
	}
  (void) signal(SIGALRM, writeTimeOut);

  if(atoi(argv[2]) == TRANSMITTER){

		//OPEN AND READ FILE TO BUFFER
    FILE *input;
    input = fopen(argv[3],"rb");
		if(input == NULL) perror("Input error");
    int fileSize = fsize(input);
    unsigned char buffer[fileSize];
    fread(buffer,sizeof(char), fileSize, input);
    fclose(input);
		//FILE READ TO BUFFER

		llopen(argv[1],TRANSMITTER);

		unsigned int length = 0;

		//SEND START PACK
		unsigned char * controlBuff = controlPacking(C_START,fileSize,argv[3],strlen(argv[3]),&length);
		//llwrite(fd,controlBuff,length);

		// for(j=0; j < length; j++)
    //   printf("Start pack hex: %x\n", controlBuff[j]);
		// for(j=0; j < length; j++)
    //   printf("Start pack: %c\n", controlBuff[j]);
		free(controlBuff);

		unsigned int index = 0;
		unsigned int blocks = fileSize / MAX_SIZE;
		unsigned int rest = fileSize % MAX_SIZE;
		unsigned int total = blocks*MAX_SIZE+rest;
		printf("blocks : %d; rest: %d; toltal: %d\n",blocks,rest,total);

		unsigned char * dataBuff;
		for (index = 0; index < blocks; index++) {
			dataBuff = dataPackaging(buffer+(MAX_SIZE*index), MAX_SIZE);

			for(j=0; j < MAX_SIZE+PACKING_HEADER_SIZE; j++)
	      printf("Data pack: %x\n", dataBuff[j]);
				printf("END FOR\n");
			//llwrite(fd,dataBuff,MAX_SIZE+PACKING_HEADER_SIZE);
			free(dataBuff);
		}

		if(rest > 0) {
		dataBuff = dataPackaging(buffer+MAX_SIZE*index, rest);
		for(j=0; j < rest+PACKING_HEADER_SIZE; j++)
			printf("Data pack: %x\n", dataBuff[j]);
	}

		controlBuff = controlPacking(C_END,fileSize,argv[3],strlen(argv[3]),&length);
		//llwrite(fd,controlBuff,length);
		// for(j=0; j < length; j++)
		// 	printf("End pack hex: %x\n", controlBuff[j]);
		// for(j=0; j < length; j++)
		// 	printf("End pack: %c\n", controlBuff[j]);
		free(controlBuff);

  	}

    if(atoi(argv[2]) == RECEIVER){
      llopen(argv[1],RECEIVER);
      //llwrite(fd,(unsigned char*)argv[2], atoi(argv[3]));
      unsigned char* buff = malloc(12000);
			//unsigned char* buffer[MAX_SIZE+PACKING_HEADER_SIZE];

      FILE * output = fopen("test_ouput.gif","wb");
      if(output == NULL) perror("output error");

      unsigned int length = 0;

			buff = llread(fd,buff,&length);

			/*printf("byteDestuffingFunction after: \n");
			for(i =0; i<length; i++)
				printf("buffer FINAL %d: %x\n",i,buff[i]);
			printf("buffer: %d\n",length);*/




      //while((length=llread(fd,buff))>=0){
      printf("entar while\n");
      //  if(length<0)continue;
      printf("tamanho a ser escrito: %d\n",length-4);
      fwrite(buff+4,sizeof(char),length-4,output);
      printf("acaba while\n");
      //}

      fclose(output);
    }

  /*********
  Testing!!!
  **********/
 /* int numberArgs = atoi(argv[2]);
  unsigned char* trama = dataPackaging(buffer, fileSize);

  unsigned char* pack =  createHeader(0xFF);
  //char* tail = malloc()
  unsigned char* tail = createTail(trama, fileSize+4);
  free(trama);
    free(tail);*/

  //unsigned int i;

/*
  for(i=0; i<numberArgs; i++)
    pack[4+i] = trama[i];
  numberArgs += 4;

  for(i=0; i<2; i++)
    pack[numberArgs+i] = tail[i];
  numberArgs += 2;
  free(trama);

  pack = bitStuffing(pack,&numberArgs);
  byteDestuffing(pack, numberArgs);

    for(i =0; i<numberArgs; i++)
    printf("pack: %x\n",pack[i]);

unsigned int* var = 12158786;
unsigned char* name = "ababababababc";
  controlPacking(0x02, 12158786, name, 13);
*/

    return 0;
  }
