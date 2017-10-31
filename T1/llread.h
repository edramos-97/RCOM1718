#ifndef _LLREAD_H_
#define _LLREAD_H_

#include "utils.h"
#include "llwrite.h"

extern double totalTime;


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
 * @brief Link Layer Function, reads a data packet header and validates it.
 *
 * State machine to read a data packet, checks for duplicates and changes the
 * sequence number and duplicate flag accordingly.
 *
 * @param fd File descriptor of the open serial port.
 * @return Returns 0 on success, -1 otherwise.
 */
int stateMachineRead(int fd);


/**
*	Send header with answer.
**/
/**
 * @brief Link Layer Function, sends a supervision packet.
 *
 * Receives a control byte, builds the supervision packet with it and sends it
 * to the open serial port.
 *
 * @param c Control byte.
 * @return Returns 0 on success, -1 otherwise.
 */
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
unsigned char* llread(int fd, unsigned char* buffer);


/**
 * @brief Link Layer Function, alternates between 0 and 1.
 *
 * Based on the previous number, changes to the other.
 *
 * @param previous_num Number previously received
 * @return Returns the switched sequence number, -1 when the precious number is not a 0 or 1.
 */
char switchSequenceNumber(char previous_num);


#endif
