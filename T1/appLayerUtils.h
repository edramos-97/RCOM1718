#ifndef _APP_LAYER_UTILS_H_
#define _APP_LAYER_UTILS_H_

#include "utils.h"

extern unsigned int packageNumber;

// Funções utils da aplicação

/**
*	Read data of control buffer.
**/
char* readControllPacket(unsigned char* controlBuff,char controll, unsigned int * filesize);

/**
 * @brief Application Layer Function, adds aplication layer header to the data.
 *
 * Takes the raw data and creates and adds an header to follow application protocol.
 *
 * @param buffer raw data to be packed.
 * @param length Length of the raw data.
 * @return Returns the comlete data packet with the header
 */
unsigned char* dataPackaging(unsigned char* buffer, int length);

/**
 * @brief Application Layer Function, creates the start and end packets of the aplication layer
 *
 * Takes in the file name and size to build the control packet for the purpose
 * specified by the control byte
 *
 * @param c Control field recieved in last response read.
 * @param fileSize Unsigned int type to pass the size of the file.
 * @param name Char* type to pass the file name.
 * @param nameSize Number of bytes of name.
 * @param length Pointer which is returned with the final length of the packet
 * @return Returns the complete control packet
 */
unsigned char* controlPacking(unsigned char c, unsigned int fileSize,char* name, unsigned char nameSize,unsigned int* length);

/**
 * @brief Returns the size of a file in bytes.
 *
 * Analyses a file and returns its size in number of bytes.
 *
 * @param file File pointer type of the file to be analysed.
 * @return Returns the number of bytes of the given file.
 */
int fsize(FILE* file);

#endif
