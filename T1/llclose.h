#ifndef _LLCLOSE_H_
#define _LLCLOSE_H_

#include "sendMessage.h"
#include "utils.h"
#include "llopen.h"


/**
 * @brief Link layer interface function, disconnects an existing connection following protocol
 *
 * Follows the link layer protocol to disconect an existing conection on the fd
 * file descriptor. Needs a second parameter as protocol is diferent in case of
 * reciever or transmitter.
 *
 * @param fd file descriptor of the open serial port.
 * @param type identiier of the role of the calling program (0 or 1 for transmiter or reciever).
 * @return Returns 0 on success and -1 otherwise.
 */
int llclose(int fd, int type);

/**
 * @brief Builds a supervision packet with the given Control byte.
 *
 * Allocates a space in memory to save the buffer, builds the buffer using the control byte given
 * and returns the pointer to the alocated memory.
 *
 * @param c the byte to be placed in the Control component of the packet header
 * @return Returns the complete supervision packet
 */
unsigned char* supervisionPacking(unsigned char c);

/**
 * @brief Handler for SIGUSR1
 *
 * Handler for the SIGUSR1 signal. Handles an unexpected disconnection.
 */
void disconnectHandler();

#endif
