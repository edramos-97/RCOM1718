#ifndef _SEND_MESSAGE_H_
#define _SEND_MESSAGE_H_

#include "utils.h"


/**
 * @brief Writes a message to an existing file descriptor
 *
 * Writes the length bytes from message to the fd file descriptor.
 * Changes the global variables that contain the last message sent and its length.
 *
 * @param fd File descriptor of the open serial port.
 * @param message Application layer message to be written.
 * @param length Length of the message to be writen.
 * @return Returns the 0 on success and -1 otherwise.
 */
int sendMessage(int fd, unsigned char* message, int length);

#endif
