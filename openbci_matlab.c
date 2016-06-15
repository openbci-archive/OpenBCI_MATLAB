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
    char buf[1024];
    // Declare the serial port struct
    struct termios serialportsettings;

	/* definition of signal action 
	allows the calling process to examine and/or specify the action to be associated 
	with a specific signal */
	struct sigaction saio;

	fd = open(PORT, O_RDWR | O_NOCTTY);


	if(fd < 0)
		printf("\n ERROR! In opening ttyUSB0\n");
	else
		printf("\n ttyUSB0 Opened Successfully\n");


	/**********************************************************
		SERIAL PORT SETUP 
	*/
    
    tcgetattr(fd,&serialportsettings);				// Gets the parameters associated with the object (fd) and stores them
    cfsetispeed(&serialportsettings,B115200);		// set the input baud rate
	cfsetospeed(&serialportsettings,B115200);		// set the output baud rate	
	
	//Hardware Information
	serialportsettings.c_cflag &= ~PARENB;			// set the parity bit (0)
	serialportsettings.c_cflag &= ~CSTOPB;			// stop bits = 1
	serialportsettings.c_cflag &= ~CSIZE;			// clears the mask
	serialportsettings.c_cflag |= CS8;				// set the data bits = 8
	serialportsettings.c_cflag &= ~CRTSCTS;			// turn off hardware based flow control (RTS/CTS)
	serialportsettings.c_cflag |= CREAD | CLOCAL;	// turn on the receiver of the serial port (CREAD)
	
	//Input data flags
	serialportsettings.c_iflag &= ~(IXON | IXOFF | IXANY);
	//Echoing and character processing flags
	serialportsettings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	//Output data flags
	serialportsettings.c_oflag |= OPOST;

	/* set parameters on the OS
		fd = file descriptor for terminal
		TCSANOW = the changes occur immediately
		&serialportsettings = pointer towards the serial port */
	tcsetattr(fd,TCSANOW,&serialportsettings);

	/* open serial port, establish settings. 
		O_RDWR = read/write mode. 
		O_NOCTTY = This program should not be the controlling terminal for that port. 
		O_NDELAY = UNIX doesn't care what state the DCD signal line is in */




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
    // fcntl(fd, F_SETFL, FASYNC);
    fcntl(fd, F_SETFL, FNDELAY|O_ASYNC );

	/* Input Modes
    	  IGNPAR = ignore framing errors and parity errors
    	  ICRNL = translate carriage return to newline on input (unless IGNCR is set*/
    // serialportsettings.c_iflag = IGNPAR | ICRNL;

    /* Output Modes
    	not sure why this is 0. 0 is set for canonical input processing */

	/* Local Modes
		ICANON = Enable canonical mode (is our board canonical?) */

	// serialportsettings.c_lflag = ICANON;
	
	/* Special Characters
		MIN > 0, TIME == 0 (blocking read)
	      read(2) blocks until MIN bytes are available, and returns up
	      to the number of bytes requested.

		VMIN = minimal number of characters for noncanonical read
		VTIME = timeout in deciseconds for noncanonical read */
	serialportsettings.c_cc[VMIN]=33;
	serialportsettings.c_cc[VTIME]=0;

	/* Flush
		fd = object
		TCIOFLUSH = flush written and received data */
	tcflush(fd, TCIOFLUSH);

	/* Sets parameters associated with the board
		fd = object
		TCSANOW = the change occurs immediately
		&serialportsettings = pointer to serialportsettings */
	tcsetattr(fd,TCSANOW,&serialportsettings);

	// char write_buffer[] = "v";
	// write(fd,write_buffer,sizeof(write_buffer));
	
	/* loop while waiting for input. normally we would do something
	useful here */ 
	while (STOP==FALSE) {
		// printf(".\n");usleep(1000);
		/* after receiving SIGIO, wait_flag = FALSE, input is available
		 and can be read */
		if (wait_flag==FALSE) { 
			int bytes_available;
			// ioctl(fd, FIONREAD, &bytes_available);
			// printf("BYTES WAITING: %d", FIONREAD);
			res = read(fd,buf,31);
			printf("%s\n", buf);
			// printf(":number of bytes:%d\n", res);
			if (res==0){
				STOP=TRUE;		// stop loop if only a CR was input 
				printf("STOPPED STREAM\n");
			}
			wait_flag = TRUE;			/* wait for new input */
		}
	}

	close(fd);
}

void signal_handler_IO (int status){
	// printf("-------------------received SIGIO signal---------------------------\n");
	wait_flag = FALSE;
}
