#ifndef _LLCLOSE_H_
#define _LLCLOSE_H_

#include "sendMessage.h"
#include "utils.h"
#include "llopen.h"

int llclose(int fd, int type);

unsigned char* supervisionPacking(unsigned char c);

#endif
