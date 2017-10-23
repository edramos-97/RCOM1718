#ifndef _LLREAD_H_
#define _LLREAD_H_

#include "utils.h"
#include "llwrite.h"


/**
 * @brief Link Layer Function, destuffs possible FLAG bytes.
 *
 * Follows the link layer protocol to destuff the message array.
 *
 * @param buffer Array of bytes to be destuffed
 * @param length Length of the buffer
 * @return Returns the destuffed Array of bytes.
 */
unsigned char* byteDestuffing(unsigned char* buffer, unsigned int* length);


/**
*	State Machine to validate fields
**/
int stateMachineRead(int fd);


/**
*	Send header with answer.
**/
int sendHeader(unsigned char c);


/**
 * @brief Link layer interface function, reads a message from an existing connection following protocol
 *
 * Follows the link layer protocol to read a message from the serial port file
 * descriptor of an existing conection. Reads the message byte by byte, checks if bcc1 is correct,
 * if it is, keeps reading until it finds a FLAG, if not, it waits until a message with a valid bcc1 is received.
 * After a successful message reception, the message is byte destuffed.
 * Bcc2 is checked, if it's ok the message is saved, if not the writer is informed to resend the message.
 *
 * @param fd File descriptor of the open serial port.
 * @param buffer Application layer message to be read.
 * @return Returns the array of bytes read on success and NULL otherwise.
 */
unsigned char* llread(int fd, unsigned char* buffer,unsigned int* length);


/**
*	Read data of control buffer.
**/
char* readControllPacket(unsigned char* controlBuff,char controll, unsigned int * filesize);


/**
*	Switch sequence number for next package.
**/
char switchSequenceNumber(char previous_num);


#endif
