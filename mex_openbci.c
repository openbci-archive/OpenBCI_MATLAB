/*
*   OpenBCI serial port driver
 *
*   Establishes a connection with the serial port
*   in order to allow read/write to the OpenBCI board.
*/

#include "mex.h"
#include <stdio.h>			 		// standard input/output definitions
#include <termios.h>				// POSIX terminal control definitions
#include <fcntl.h>					// File Control definitions
#include <unistd.h>					// UNIX standard function definitions
#include <errno.h>					//
// #include <sys/signal.h>
#include <sys/types.h>
#include <cstring>
#include "openbci_matlab.c"

void board_information();
int mexEvalString(const char *command);


/* The gateway function*/
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]){	
	// char prhs_arg = *prhs;
	char *port, *output_buf, *command, *arg;
	size_t buflen;

	/* ARGUMENT CHECK */
	if (nrhs<1)
		mexErrMsgIdAndTxt("MATLAB:openbci_matlab:invalidNumInputs",
			"Enter correct inputs: openbci_matlab(command,arg)");
	else if (nrhs==1)
		command = mxArrayToString(prhs[0]);						//holds the port string						
	else if (nrhs>1){
		arg = mxArrayToString(prhs[1]);	
	}
	else if(nlhs > 2) 
	  mexErrMsgIdAndTxt( "MATLAB:revord:maxlhs",
			  "Too many output arguments.");

    /* input must be a string */
    // if ( mxIsChar(prhs[0]) != 1)
    //   mexErrMsgIdAndTxt( "MATLAB:revord:inputNotString",
    //           "Port must be a string (e.g. \'COM1\' or \'\\dev\\ttyUSB0\\\'')");

  	buflen = (mxGetM(prhs[0]) * mxGetN(prhs[0])) + 1;			//length of input string
  	command = mxArrayToString(prhs[0]);						//holds the port string						

  	/* Evaluate Coammnds */
  	if (strcmp(command,"open")==0)
  		initialize_port(arg);
  	else if (strcmp(command,"close")==0)
  		close_port();
  	else if (strcmp(command,"stream")==0)
  		send_to_board("b");
  	else if (strcmp(command,"stop")==0)
  		send_to_board("s");
  	else if (strcmp(command,"reset")==0)
  		send_to_board("v");
  	else if (strcmp(command,"registry settings")==0)
  		send_to_board("?");
  	else if (strcmp(command,"off")==0)
  		send_to_board(args)
  	else if (strcmp(command,"on")==0)
  		send_to_board(args)
  	else if (strcmp(command, "channel settings")==0)
  		send_to_board(args)
  	else if (strcmp(command, "set default")==0)
  		send_to_board("d")
  	else if (strcmp(command, "get default")==0)
  		send_to_board("D")
  	// else if (strcmp(command, ""))


}


void board_information(){
	printf("sup\n");
}