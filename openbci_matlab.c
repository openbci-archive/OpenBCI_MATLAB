/*

This program provides serial port communication with the OpenBCI Board.

NOTES:
-OpenBCI board uses asynchronous, 

*/

#include	<stdio.h>			 	// standard input/output definitions
#include	<termios.h>				// POSIX terminal control definitions

#define BAUDRATE B115200			// define baudrate (115200bps)
#define PORT "/dev/ttyUSB0"			// define port
#define _POSIX_SOURCE 1				// POSIX compliant source ((((how necessary is this...))))



main()
{
	int fd;							// File descriptor for the port
	/* open serial port, establish settings. 
		O_RDWR = read/write mode. 
		O_NOCTTY = This program should not be the controlling terminal for that port. 
		O_NDELAY = UNIX doesn't care what state the DCD signal line is in */
	fd = open(PORT, O_RDWR | O_NOCTTY | O_NDELAY)
	if(fd < 0)
		printf("\n ERROR! In opening ttyUSB0\n");
	else
		printf("\n ttyUSB0 Opened Successfulllyyyyyy!!!!!!!!!!!!!!!!\n");
	close(fd);
}