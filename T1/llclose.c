#include "llclose.h"

int disconnect() {
  unsigned char * discBuff = supervisionPacking(C_DISC);
  printf("criou C_DISC\n");

  if(sendMessage(fd, discBuff, SUPERVISION_SIZE) < 0){
      printf("Send disconnect: failed\n");
  } printf("Enviou  C_disc\n");

  printf("Vai entrar em stateMachine e esperar DISC\n");
  stateMachine(fd, C_DISC);

  discBuff = supervisionPacking(C_UA);
  printf("criou C_UA\n");

  if(sendMessage(fd, discBuff, SUPERVISION_SIZE) < 0){
      printf("Send UA: failed\n");
  } printf("Enviou  UA\n");

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
