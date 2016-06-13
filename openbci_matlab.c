/*

This program provides serial port communication with the OpenBCI Board.

NOTES:
-OpenBCI board uses asynchronous, 

*/

#include <stdio.h>			 		// standard input/output definitions
#include <termios.h>				// POSIX terminal control definitions
#include <fcntl.h>					// File Control definitions
#include <unistd.h>					// UNIX standard function definitions
#include <errno.h>					//
#include <sys/signal.h>
#include <sys/types.h>

#define BAUDRATE B115200			// define baudrate (115200bps)
#define PORT "/dev/ttyUSB0"			// define port
#define _POSIX_SOURCE 1				// POSIX compliant source ((((how necessary is this...))))
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE; 

void signal_handler_IO (int status);   /* definition of signal handler */
int wait_flag=TRUE;    

main()
{
	int fd;								// File descriptor for the port
    int c;
    int res;
    char buf[33];

    struct termios oldtio,newtio;

	/* definition of signal action 
		allows the calling process to examine and/or specify the action to be associated 
		with a specific signal */
	struct sigaction saio;

	/* open serial port, establish settings. 
		O_RDWR = read/write mode. 
		O_NOCTTY = This program should not be the controlling terminal for that port. 
		O_NDELAY = UNIX doesn't care what state sthe DCD signal line is in */
	fd = open(PORT, O_RDWR | O_NOCTTY | O_NDELAY);

	if(fd < 0)
		printf("\n ERROR! In opening ttyUSB0\n");
	else
		printf("\n ttyUSB0 Opened Successfully\n");



    /* install the signal handler before making the device asynchronous

    	sa_handler = pointer to the signal-catching function
    	sa_mask = additional set of signals to be blocked during execution of signal-catching fxn
    	sa_flags = special flags to affect behavior of signal */

    saio.sa_handler = signal_handler_IO;
    // saio.sa_mask = 0;		\\this kept giving me an error, so the next two lines replace this
    sigemptyset(&saio.sa_mask);
	sigaddset(&saio.sa_mask, SIGINT);
    saio.sa_flags = 0;
    saio.sa_restorer = NULL;
    sigaction(SIGIO,&saio,NULL);



    /* Make the file descriptor asynchronous (the manual page says only 
       O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
    fcntl(fd, F_SETFL, FASYNC);
	// Gets the paramets associated with the object (fd) and stores them
    tcgetattr(fd,&oldtio);



    /* Control Modes
    	  BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
          CRTSCTS : output hardware flow control (only used if the cable has
                    all necessary lines. See sect. 7 of Serial-HOWTO)
          CS8     : 8n1 (8bit,no parity,1 stopbit)
          CLOCAL  : local connection, no modem contol
          CREAD   : enable receiving characters*/
    newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;

	/* Input Modes
    	  IGNPAR = ignore framing errors and parity errors
    	  ICRNL = translate carriage return to newline on input (unless IGNCR is set*/
    newtio.c_iflag = IGNPAR | ICRNL;

    /* Output Modes
    	not sure why this is 0. 0 is set for canonical input processing */
	newtio.c_oflag = 0;

	/* Local Modes
		ICANON = Enable canonical mode (is our board canonical?) */

	// newtio.c_lflag = ICANON;
	
	/* Special Characters
		MIN > 0, TIME == 0 (blocking read)
	      read(2) blocks until MIN bytes are available, and returns up
	      to the number of bytes requested.

		VMIN = minimal number of characters for noncanonical read
		VTIME = timeout in deciseconds for noncanonical read */
	newtio.c_cc[VMIN]=1;
	newtio.c_cc[VTIME]=0;

	/* Flush
		discards data written to the object, data received but not read 
		fd = object
		TCIFLUSH = data received but not read */
	tcflush(fd, TCIFLUSH);

	/* Sets parameters associated with the board
		fd = object
		TCSANOW = the change occurs immediately
		&newtio = pointer to newtio */
	tcsetattr(fd,TCSANOW,&newtio);

	
	
	/* loop while waiting for input. normally we would do something
	useful here */ 
	while (STOP==FALSE) {
		// printf(".\n");usleep(1000);
		/* after receiving SIGIO, wait_flag = FALSE, input is available
		 and can be read */
		// printf("%d",wait_flag);
		if (wait_flag==FALSE) { 	
			res = read(fd,buf,24);
			buf[33]=0;
			// for (int i=0;i<res;i++){
			// 	printf("%d\n",buf[res]);
			// }
			printf(":buff-%s\n", buf);
			printf(":number of bytes:%d\n", res);
			if (res==0){
				STOP=TRUE;		// stop loop if only a CR was input 
				printf("STOPPED STREAM\n");
			}
			wait_flag = TRUE;			/* wait for new input */
		}
	}



	close(fd);
	// return(fd);
}

void signal_handler_IO (int status){
	printf("-------------------received SIGIO signal---------------------------\n");
	wait_flag = FALSE;
	// printf("wait flag: %d\n",wait_flag);
}
