#include "llclose.h"

int llclose(int fd, int type) {

  unsigned char * discBuff = supervisionPacking(C_DISC);

  if(type == TRANSMITTER){

  if(sendMessage(fd, discBuff, SUPERVISION_SIZE) < 0){
      printf("Send disconnect: failed\n");
      return -1;
  } printf("Sent DISC\n");

  printf("Transmitter waiting for DISC\n");
  stateMachine(fd, C_DISC);

  discBuff = supervisionPacking(C_UA);

  if(sendMessage(fd, discBuff, SUPERVISION_SIZE) < 0){
      printf("Send UA: failed\n");
      return -1;
  } printf("Sent  UA\n");
}
if(type == RECEIVER){
      printf("RECEIVER waiting for DISC\n");
    stateMachine(fd, C_DISC);

    if(sendMessage(fd, discBuff, SUPERVISION_SIZE) < 0){
        printf("Send disconnect: failed\n");
        return -1;
    } printf("Sent DISC\n");

    stateMachine(fd, C_UA);
    printf("Received UA\n");
}
  return 0;
}

unsigned char* supervisionPacking(unsigned char c) {
  unsigned char* buff = malloc(SUPERVISION_SIZE);

  buff[0] = FLAG;
  buff[1] = A;
  buff[2] = c;
  buff[3] = buff[1] ^ buff[2];
  buff[4] = FLAG;

  return buff;
}

void disconnectHandler(){
  printf("\nUnexpected writer DISCONNECTION! Starting close protocol...\n\n");
  alarm(3);
  unsigned char * discBuff = supervisionPacking(C_DISC);
  if(sendMessage(fd, discBuff, SUPERVISION_SIZE) < 0){
      printf("Send disconnect: failed\n");
  }

  stateMachine(fd, C_UA);
  printf("Received UA\n");
  exit(0);
}
