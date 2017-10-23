#include "appLayerUtils.h"

unsigned int packageNumber = 0;

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

unsigned char* dataPackaging(unsigned char* buffer, int length){
  unsigned char* header = malloc(PACKING_HEADER_SIZE+length);
  header[0] = C_DATA;
  header[1] = packageNumber;
  header[2] = (length / 256);
  header[3] = (length % 256);

  unsigned int i;
  for(i=0;i<length;i++){
    header[PACKING_HEADER_SIZE+i] = buffer[i];
  }

  return header;
}

unsigned char* controlPacking(unsigned char c, unsigned int fileSize,
  char* name, unsigned char nameSize,unsigned int* length){

    (*length) = 5*sizeof(char)+sizeof(int)+nameSize;
    unsigned char* buff = malloc(*length);

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

    return buff;
  }

int fsize(FILE* file) {
   long int currPos = ftell(file);

   if (fseek(file, 0, SEEK_END) == -1){
      perror("file size: ");
			return -1;
	}

	// saving file size
	long int size = ftell(file);

	// seeking to the previously saved position
	fseek(file, 0, currPos);

	// returning size
	return size;
}
