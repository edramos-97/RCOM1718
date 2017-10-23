#include "llwrite.h"

unsigned char sucessLastPackage = FALSE;

int llwrite(int fd, unsigned char* buffer, int length){

  int n_chars = 0;

  do{

    int numberArgs = length;

    printf("sequence %d\n",sequenceNumber);
    printf("Sending");
    unsigned char* pack =  createHeader(C_INFO(sequenceNumber));

    unsigned char* tail = createTail(buffer, numberArgs);

    unsigned int i;
    pack = realloc(pack,numberArgs + INFO_HEADER_SIZE);
    for(i=0; i < numberArgs; i++)
    pack[INFO_HEADER_SIZE+i] = buffer[i];
    numberArgs += INFO_HEADER_SIZE;

    pack = realloc(pack,numberArgs + INFO_TAIL_SIZE);
    for(i=0; i < INFO_TAIL_SIZE; i++)
    pack[numberArgs+i] = tail[i];
    numberArgs += INFO_TAIL_SIZE;
    free(tail);

    pack = byteStuffing(pack, &numberArgs);

    n_chars = sendMessage(fd, pack, numberArgs);
    if(n_chars < 0){
      return -1;
    }

    stateMachineReadAnswer(fd);

  } while(!sucessLastPackage);

  return n_chars;

}

unsigned char* createHeader(char c){
  unsigned char* header = malloc(INFO_HEADER_SIZE);
  header[0] = FLAG;
  header[1] = A;
  header[2] = c;
  header[3] = header[1] ^ header[2];

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
    } else
    buff[j] = buffer[i];
  }

  buff[j] = buffer[*length-1];

  *length = new_length;
  return buff;
}

int stateMachineReadAnswer(int fd) {

  unsigned char received, received_A, received_C;
  int state = 0;

  while(state != 5) {
    read(fd, &received, 1);
    printf(".");

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
      received == C_REJ(1) || received == C_REJ(0)) {
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
    if(sequenceNumber == 1){
      printf(" Sent\n\n");
      sequenceNumber = 0;
      sucessLastPackage = TRUE;
    } else {
      printf(" Duplicate\n");
      sucessLastPackage = FALSE;
    }
  }
  else if(received_C == C_RR(1)){
    if(sequenceNumber == 0){
      printf(" Sent\n");
      sequenceNumber = 1;
      sucessLastPackage = TRUE;
    } else {
      printf(" Duplicate\n");
      sucessLastPackage = FALSE;
    }
  }
  else if(received_C == C_REJ(0)){
    printf(" Rejected\nResending");
    sequenceNumber = 0;
    sucessLastPackage = FALSE;
  }
  else if(received_C == C_REJ(1)){
    printf(" Rejected\nResending");
    sequenceNumber = 1;
    sucessLastPackage = FALSE;
  }
}
