#ifndef _LLOPEN_H_
#define _LLOPEN_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "utils.h"
#include "sendMessage.h"

unsigned char SET[5];


int stateMachine(int fd, unsigned char message);


int llopen(char* porta, int type);


#endif
