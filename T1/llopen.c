#include "llopen.h"

int llopen(char* porta, int type)
{
  (void) signal(SIGALRM, writeTimeOut);
  struct termios oldtio,newtio;
  //char buf[255];

  if (
   ((strcmp("/dev/ttyS0", porta)!=0) &&
     (strcmp("/dev/ttyS1", porta)!=0))) {
   printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
 exit(1);
}


  /*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
  */

fd = open(porta, O_RDWR | O_NOCTTY );
if (fd <0) {perror(porta); exit(-1); }

  if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
perror("tcgetattr");
exit(-1);
}

bzero(&newtio, sizeof(newtio));
newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
newtio.c_iflag = IGNPAR;
newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
newtio.c_lflag = 0;

  newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */

  /*
  VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
  leitura do(s) pr�ximo(s) caracter(es)
  */

tcflush(fd, TCIOFLUSH);

if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
  perror("tcsetattr");
  exit(-1);
}

printf("New termios structure set\n");

  // Ainda não sabemos nada desta vida
  // Eu nasci aqui
  // Eu sofro da cabeça quase de nascença

  if (type){ // Receiver

  	stateMachine(fd, C_SET);

  	SET[0] = FLAG;
   SET[1] = A;
   SET[2] = C_UA;
   SET[3] = SET[1] ^ SET[2];
   SET[4] = FLAG;

   if (sendMessage(fd, SET, SUPERVISION_SIZE) < 0)
    return -1;

  } else {   // Transmiter


  	SET[0] = FLAG;
   SET[1] = A;
   SET[2] = C_SET;
   SET[3] = SET[1] ^ SET[2];
   SET[4] = FLAG;

   if (sendMessage(fd, SET, SUPERVISION_SIZE) < 0)
    return -1;

  alarm(3);


  stateMachine(fd, C_UA);
}
numberTries = 0;
return 0;
}

int stateMachine(int fd, unsigned char message) {

	unsigned char received, received_A, received_C;
	int state = 0;

	while(state != 5) {
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
      if(received == message) {
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

	alarm(0);

  return 0;
}
