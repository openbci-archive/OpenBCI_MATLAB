/*

This program provides serial port communication with the OpenBCI Board.

Written by Gabriel Ibagon at OpenBCI, June 2016
gabriel.ibagon@gmail.com

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
int * byte_parser (unsigned char buf[], int res);
int wait_flag=TRUE;    

main()
{
	int fd;								// File descriptor for the port
	int c;
	int res;
	unsigned char buf[16];
	struct termios serialportsettings;  // Declare the serial port struct
	struct sigaction saio;				// Declare signals
	fd = open(PORT, O_RDWR | O_NOCTTY); // declare serial port file descriptor

	//*****************************************************************************************
	//	SERIAL PORT SETUP 

	// Attempt to read attributes of serial port
	if(tcgetattr(fd,&serialportsettings) != 0)
		printf("\n ERROR! In opening ttyUSB0\n");
	else
		printf("\n ttyUSB0 Opened Successfully\n");
	
	//Baud Rate information (Baud=115200)
	cfsetispeed(&serialportsettings,B115200);		// set the input baud rate
	cfsetospeed(&serialportsettings,B115200);		// set the output baud rate	

	//Hardware Information
	serialportsettings.c_cflag &= ~PARENB;			// set the parity bit (0)
	serialportsettings.c_cflag &= ~CSTOPB;			// stop bits = 1 (2 is default)
	serialportsettings.c_cflag &= ~CSIZE;			// clears the mask ()
	serialportsettings.c_cflag |= CS8;				// set the data bits = 8
	serialportsettings.c_cflag &= ~CRTSCTS;			// no hardware based flow control (RTS/CTS)
	serialportsettings.c_cflag |= CREAD;	// turn on the receiver of the serial port (CREAD)
	serialportsettings.c_cflag |= CLOCAL;			//no modem
	//Input data flags (not needed?)
	serialportsettings.c_iflag &= ~(IXOFF | IXON);
	// serialportsettings.c_iflag |= IGNBRK;
	// serialportsettings.c_iflag &= ~(BRKINT | IGNPAR | PARMRK | INPCK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);

	//Echoing and character processing flags
	serialportsettings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); //no echoing, input proc, signals, or background proc halting
	//Output data flags ("for ")
	// serialportsettings.c_oflag |= OPOST; //causes the output data to be processed in an implementation-defined manner

	serialportsettings.c_cc[VMIN]=33;
	serialportsettings.c_cc[VTIME]=0;


	/* Make the file descriptor asynchronous (the manual page says only 
	   O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
	// fcntl(fd, F_SETFL, FASYNC);
	fcntl(fd, F_SETFL, O_NDELAY|O_ASYNC );


	tcsetattr(fd,TCSANOW,&serialportsettings); 		//set the above attributes


	/* install the signal handler before making the device asynchronous
		sa_handler = pointer to the signal-catching function
		sa_mask = additional set of signals to be blocked during execution of signal-catching fxn
		sa_flags = special flags to affect behavior of signal */
	saio.sa_handler = signal_handler_IO;
	// saio.sa_mask = 0;		\\this kept giving me an error, so the next two lines replace this
	// sigemptyset(&saio.sa_mask);
	// sigaddset(&saio.sa_mask, SIGINT);
	saio.sa_flags = 0;
	saio.sa_restorer = NULL;
	sigaction(SIGIO,&saio,NULL);

	/* Special Characters
		MIN > 0, TIME == 0 (blocking read)
		  read(2) blocks until MIN bytes are available, and returns up
		  to the number of bytes requested.
		VMIN = minimal number of characters for noncanonical read
		VTIME = timeout in deciseconds for noncanonical read */

	/* Flush
		fd = object
		TCIOFLUSH = flush written and received data */
	tcflush(fd, TCIOFLUSH);
	write(fd,"v",1);

	while (STOP==FALSE) {
		if (wait_flag==FALSE) { 
			int bytes_available;
			// ioctl(fd, FIONREAD, &bytes_available);
			// printf("BYTES WAITING: %d", FIONREAD);
			res = read(fd,&buf,33);
			printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$number of bytes:%d\n", res);
			byte_parser(buf,res);
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

int packetslost;
packetslost = 0;
int samplecounter;

int * byte_parser (unsigned char buf[], int res){
	static unsigned char framenumber = 0;
	static int channel_number = 0;
	static int byte_count = 0;
	static int temp_val = 0;
	static int output[7];
	int parse_state = 0;
	for (int i=0; i<res;i++){
		printf("%d |", i);
		printf("PARSE STATE %d | ", parse_state);
		printf("BYTE %x\n",buf[i]);
		switch (parse_state) {
			case 0:
			// if parses loses its place, find the end to restart
				if (buf[i] == 0xC0){
					parse_state++;
				}
				else if (buf[i] == 0xA0){
					parse_state = 2;
				}
				break;
			case 1:
			// look for header
					// printf("START BYTE %x\n",buf[i]);

					if (buf[i] == 0xA0){
						parse_state++;
					}else{
						// printf("ok..../n");
						parse_state = 0;
					}
				break;
			case 2: 
			// Check the packet counter
					// printf("COUNT BYTE %x\n",buf[i]);
					if ((buf[i]-framenumber!= 1) && (buf[i]!=0)){
						// printf("%d",(buf[i]-framenumber!= 1));
						packetslost++;
					}
					printf("%d\n", framenumber);
					framenumber++;
					channel_number=0;
					parse_state++;
					break;
			case 3:
			// get ADS channel values **CHANNEL DATA**
				temp_val |= (((unsigned int)buf[i]) << (16 - (byte_count*8))); //convert to MSB
				byte_count++;
				// printf("BYTE COUNT = %d\n", byte_count);
				//24 bit to 32 bit conversion
				if (byte_count==3){
					if ((temp_val & 0x00800000) > 0) {
						temp_val |= 0xFF000000;
					}else{
						temp_val &= 0x00FFFFFF;
					}
					output[channel_number] = temp_val;
					// printf("CHANNEL NO. %d\n", channel_number);
					channel_number++;
					if (channel_number==8){
						// printf("ALL CHANNELS WOOP WOOP\n");
						parse_state++;
						byte_count = 0;
						temp_val = 0;
					}else{
						byte_count = 0;
						temp_val = 0;
					}
				}
				break;
			case 4:
			// get LIS3DH channel values 2 bytes times 3 axes **ACCELEROMETER**
				temp_val |= (((unsigned int)buf[i]) << (8 - (byte_count*8)));
				byte_count++;
				if (byte_count==2) {
					if ((temp_val & 0x00008000) > 0) {
						temp_val |= 0xFFFF0000;
					} else {
						temp_val &= 0x0000FFFF;
					}  
					output[channel_number]=temp_val;
					channel_number++;
					if (channel_number==(8+3)) {  // all channels arrived !
						parse_state++;
						byte_count=0;
						channel_number=0;
						temp_val=0;
					}
					else { byte_count=0; temp_val=0; }
				}
				break;

			case 5: 
			// End byte
				if (buf[i] == 0xC0){
					// call message pump???
					parse_state = 1;
				}
				else{
					// Insert something about synching here
					printf("CATCH UP!\n");
					parse_state= 0;	// resync
				}
				break;

		default: parse_state=0;  // resync		}
		}
	}
	return output;
}