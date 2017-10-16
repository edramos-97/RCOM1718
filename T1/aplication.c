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



int main(int argc, char** argv) {
	if (argc != 4){
		perror("Wrong number of arguments");
		return -1;
	}
  (void) signal(SIGALRM, writeTimeOut);

  if(atoi(argv[2]) == TRANSMITTER){
    FILE *ptr;
    ptr = fopen(argv[3],"rb");
    int fileSize = fsize(ptr);
    unsigned char buffer[fileSize];
    fread(buffer,sizeof(char), fileSize, ptr);
    fclose(ptr); 

    unsigned int j;
    for(j=0; j < fileSize; j++)
      printf("info_txt: %c\n", buffer[j]);

    printf("filesize %d\n", fileSize);

    llopen(argv[1],TRANSMITTER);

    unsigned int i;
    for(i = 0; i < 1; i++) {
      llwrite(fd, buffer, fileSize);
    }
  }

    if(atoi(argv[2]) == RECEIVER){
      llopen(argv[1],RECEIVER);
      //llwrite(fd,(unsigned char*)argv[2], atoi(argv[3]));
      unsigned char* buff = malloc(2000);

      FILE * output = fopen("test_ouput.jpeg","a");
      if(output == NULL) perror("não abriu");

      unsigned int length = 0;

      length=llread(fd,buff);


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
