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


/**
 * @brief Link layer interface function, waits for a control package with the given control byte
 *
 * Reads from the file descriptor one byte at a time until a complete and valid
 * link layer header is read.
 *
 * @param fd File descriptor of the open serial port.
 * @param message Expected control byte.
 * @return Returns 0 on success and -1 otherwise.
 */
int stateMachine(int fd, unsigned char message);

/**
 * @brief Link layer interface function, creates a connection following protocol
 *
 * Follows the link layer protocol to conect using a given serial port using a
 * file descriptor. Needs a second parameter as protocol is diferent in case of
 * reciever or transmitter.
 *
 * @param porta Name of the serial port to be opened.
 * @param type identiier of the role of the calling program (0 or 1 for transmiter or reciever).
 * @return Returns 0 on success and -1 otherwise.
 */
int llopen(char* porta, int type);


#endif
