#ifndef _LLWRITE_H_
#define _LLWRITE_H_

#include "utils.h"
#include "sendMessage.h"


/**
 * @brief Link layer interface function, writes a message to an existing connection following protocol
 *
 * Follows the link layer protocol to write a message to the serial port file
 * descriptor of an existing conection. Adds link layer protocol header and tail
 * to the provided data buffer, does byte stuffing to prevent escape charaters
 * and sends the complete message.
 *
 * @param fd File descriptor of the open serial port.
 * @param buffer Application layer message to be written.
 * @param length Length of the message to be writen.
 * @return Returns the number of bytes writen on success and -1 otherwise.
 */
int llwrite(int fd, unsigned char* buffer, int length);

/**
 * @brief Link Layer Function, creates a tail for a given data pack.
 *
 * Follows the link layer protocol to calculate the bcc2 and add a flag.
 *
 * @param buffer Data used to calculate bcc2 control field.
 * @param length Length of the data to be used.
 * @return Returns a two bytes array with the tail for the data received.
 */
unsigned char* createTail(unsigned char* buffer, int length);

/**
 * @brief Link Layer Function, creates an header with a given control byte
 *
 * Follows the link layer protocol to calculate the bcc2 and add a flag.
 *
 * @param c Control byte of the header
 * @return Returns a two bytes array with the tail for the data received.
 */
unsigned char* createHeader(char c);

/**
 * @brief Link Layer Function, stuffs possible FLAG bytes to avoid message shortages.
 *
 * Follows the link layer protocol to stuff the message array.
 *
 * @param buffer Array of bytes to be stuffed
 * @param length Length of the buffer
 * @return Returns the stuffed Array of bytes.
 */
unsigned char* byteStuffing(unsigned char* buffer, int* length);

/**
 * @brief Link Layer Function, state machine to read a response header
 *
 * State machine to verify bcc1 and after reading a complete resconse packet header
 * with a valid control(RR or REJ) changes the state of the last package sent to
 * success or insuccess, also chages the sequence number of the packets accordingly.
 *
 * @param fd File descriptor of the open serial port.
 * @return Returns a two bytes array with the tail for the data received.
 */
int stateMachineReadAnswer(int fd);

/**
 * @brief Link Layer Function, takes care of the sequence number and validation of sent packets
 *
 * Takes in the control byte received and verifies it to take adequate action
 * about changing the sequence number and the last packet success flag.
 *
 * @param recieved_C Control field recieved in last response read.
 */
void verifyConditions(unsigned char received_C);

#endif
