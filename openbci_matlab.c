/*

This program provides serial port communication with the OpenBCI Board.

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
#define _POSIX_SOURCE 1				// POSIX compliant source
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE; 


void signal_handler_IO (int status);  							 // definition of signal handler */
void init_byte_parser(unsigned char buf[], int res); //method to parse the bytes during initialization
float * byte_parser (unsigned char buf[], int res); // methoD to parse the bytes while streaming
int wait_flag=FALSE;    														// signalling
int streaming = 1;																	// used to switch parsing between initialization mode and streaming mode

main()
{
	int c;								
	int res;														// return of function read(): number of bytes read
	unsigned char buf[33];							// byte buffer
	int fd;															// the file descriptor for the serial port
	struct termios serialportsettings;	// the serial port struct						
	struct sigaction saio;							// Declare signals
	fd = open(PORT, O_RDWR | O_NOCTTY); // declare serial port file descriptor

	/******************************************************************************************
	//	SERIAL PORT SETUP 
	//
	*/

	// Attempt to read attributes of serial port 
	if(tcgetattr(fd,&serialportsettings) != 0)
		printf("\n ERROR! In opening ttyUSB0\n");

	else
		printf("\n ttyUSB0 Opened Successfully\n");

	/* Baud Rate Information (Baud=115200) */
	cfsetispeed(&serialportsettings,B115200);		// set the input baud rate
	cfsetospeed(&serialportsettings,B115200);		// set the output baud rate	


	/* Flags */
	//Hardware Information Flags
	serialportsettings.c_cflag &= ~PARENB;				// set the parity bit (none)
	serialportsettings.c_cflag &= ~CSTOPB;				// # of stop bits = 1 (2 is default)
	serialportsettings.c_cflag &= ~CSIZE;					// clears the mask
	serialportsettings.c_cflag |= CS8;						// set the # of data bits = 8
	serialportsettings.c_cflag &= ~CRTSCTS;				// no hardware based flow control (RTS/CTS)
	serialportsettings.c_cflag |= CREAD;					// turn on the receiver of the serial port (CREAD)
	serialportsettings.c_cflag |= CLOCAL;					// no modem
	//Input Data Flags
	serialportsettings.c_iflag &= ~(IXOFF | IXON);	//ignore 'XOFF' and 'XON' command bits (fixes a bug where the parser skips '0x11' and '0x13')
	// serialportsettings.c_iflag |= IGNBRK;
	// serialportsettings.c_iflag &= ~(BRKINT | IGNPAR | PARMRK | INPCK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	//Echoing and character processing flags
	// serialportsettings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); //no echoing, input proc, signals, or background proc halting
	//Output data flags ("for ")
	// serialportsettings.c_oflag |= OPOST; //causes the output data to be processed in an implementation-defined manner


	// Set the minimum size of packet for read (33 bytes, 0 seconds)
	serialportsettings.c_cc[VMIN]=33; 						// should initially 1 (during board initialization) but changed to 33 once entering streaming mode
	serialportsettings.c_cc[VTIME]=0;							// 0 seconds
	

	fcntl(fd, F_SETFL, O_NDELAY|O_ASYNC );				// asynchronous settings
	tcsetattr(fd,TCSANOW,&serialportsettings); 		// set the above attributes
	saio.sa_handler = signal_handler_IO;					// signal handling
	saio.sa_flags = 0;														// signal handling
	saio.sa_restorer = NULL;											// signal handling
	sigaction(SIGIO,&saio,NULL);									// signal handling
	tcflush(fd, TCIOFLUSH);												// flush the serail port
	write(fd,"v",1); 															//reset the board and receive+print board information 


	//*******************************************************************************************
	// STREAMING LOOP
	//

	while (STOP==FALSE) {

		// signal
		if (wait_flag==FALSE) { 
			int bytes_available;
			res = read(fd,&buf,33);										// read 33 bytes from serial and place at buf
			// printf("AMT %d",res);
			// if (!streaming){
			// 	printf("entering...");
			// 	init_byte_parser(buf,res);
			// }else{
			// 	// printf("STREAMING");
			// 	byte_parser(buf,res);
			// 	printf("sup dog");
			// }

			byte_parser(buf,res);										// send the read to byte_parser()
			wait_flag = TRUE;												/* wait for new input */
		}
	}


	/* CLOSE SERIAL PORT */
	close(fd);
}

/*************************************************************************************
//	SIGNAL HANDLER
//
*/
void signal_handler_IO (int status){
	wait_flag = FALSE;
}



/***********************************************************************************
//	PARSER
//
*/


//INITIALIZATION BYTE PARSER
int samplecounter;
int EOT = 0;
void init_byte_parser(unsigned char buf[], int res){
	for (int i=0;i<res;i++){

		// printf("TRUE??? %d\n",(buf[i] == 36));
		if (buf[i] == 36){
			EOT++;
			if ((1==1)){
				printf("ok");
				// set_stream_mode();
			}
		}
	}
}

// After initialization is finished, change the parse mode to stream mode
// This mostly means that c_cc[VMIN] must be set to 33 bytes, rather than 1
/*
void set_stream_mode(){
 	printf("slksksksksskks");
 	serialportsettings.c_cc[VMIN]=1; //initially 1, but changed to 33 once entering streaming mode
 	tcsetattr(fd,TCSANOW,&serialportsettings);
 }
*/


// STREAMING BYTE PARSER
float * byte_parser (unsigned char buf[], int res){
	static unsigned char framenumber = 0;							// framenumber = sample number from board (0-255)
	static int channel_number = 0;										// channel number (0-7)
	static int acc_channel = 0;												// accelerometer channel (0-2)
	static int byte_count = 0;												// keeps track of channel bytes as we parse
	static int temp_val = 0;													// holds the value while converting channel values from 24 to 32 bit integers
	static float output[11];													// buffer to hold the output of the parse (all -data- bytes of one sample)
	int parse_state = 0;															// state of the parse machine (0-5)
	printf("######### NEW PACKET ##############\n");
	for (int i=0; i<res;i++){													// iterate over the contents of a packet
		printf("%d |", i);															// print byte number (0-33)
		printf("PARSE STATE %d | ", parse_state);				// print current parse state
		printf("BYTE %x\n",buf[i]);											// print value of byte

		/**************************************************************
		// STATE MACHINE
		//
		*/
		switch (parse_state) {
			case 0:																				// STATE 0: find end+beginning byte
				if (buf[i] == 0xC0){												// if finds end byte first, look for beginning byte next
					parse_state++;								
				}
				else if (buf[i] == 0xA0){										// if find beginning byte first, proceed to parsing sample number (state 2)
					parse_state = 2;													
				}
				break;
			case 1:																				// STATE 1: Look for header (in case C0 found first)
					if (buf[i] == 0xA0){
						parse_state++;
					}else{
						parse_state = 0;
					}
				break;
			case 2: 																				// Check framenumber
					if ((buf[i]-framenumber!= 1) && (buf[i]!=0)){	
						/* Do something like this to check for missing
								packets. The above "if" doesn't work, but it
								should do something similar (computer if the 
								last packet is one less than the current one). */
					}
					printf("%d\n", framenumber);
					framenumber++;
					channel_number=0;
					parse_state++;
					break;
			case 3:																					// get ADS channel values **CHANNEL DATA**
				temp_val |= (((unsigned int)buf[i]) << (16 - (byte_count*8))); //convert to MSB
				byte_count++;	
				if (byte_count==3){														// if 3 bytes passed, 24 bit to 32 bit conversion
					if ((temp_val & 0x00800000) > 0) {
						temp_val |= 0xFF000000;
					}else{
						temp_val &= 0x00FFFFFF;
					}
					/* simple count to uV conversion needed here! 
					Check docs.openbci.com */
					output[channel_number] = temp_val;					// place value into data output buffer
					printf("CHANNEL NO. %d\n", channel_number);
					channel_number++;
					if (channel_number==8){											// check to see if 8 channels have already been parsed
						parse_state++;
						byte_count = 0;
						temp_val = 0;
					}else{
						byte_count = 0;
						temp_val = 0;
					}
				}
				break;
			case 4:																					// get LIS3DH channel values 2 bytes times 3 axes **ACCELEROMETER**
				temp_val |= (((unsigned int)buf[i]) << (8 - (byte_count*8)));
				byte_count++;
				if (byte_count==2) {
					if ((temp_val & 0x00008000) > 0) {
						temp_val |= 0xFFFF0000;
					} else {
						temp_val &= 0x0000FFFF;
					}  
					// output[acc_channel+8]=temp_val;				// output onto buffer
					acc_channel++;
					channel_number++;
					if (channel_number==(8+3)) {  						// all channels arrived !
						parse_state++;
						byte_count=0;
						channel_number=0;
						temp_val=0;
					}
					else { byte_count=0; temp_val=0; }
				}
				break;

			case 5: 																			// look for end byte
				if (buf[i] == 0xC0){
					// call message pump???
					parse_state = 0;
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
