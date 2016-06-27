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
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "openbci_matlab.c"

void board_information();
int mexEvalString(const char *command);


/* The gateway function*/
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]){	
	// char prhs_arg = *prhs;
	char *port, *command, *arg;
	size_t buflen;
	int channel_number = 16;
  char output_buf[3];
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
    char c;
  	/* Evaluate Coammnds */
  	if (strcmp(command,"open")==0)
  		initialize_port(arg);
  	else if (strcmp(command,"close")==0)
  		close_port();
  	else if (strcmp(command,"reset")==0){
  		char c = 'v'; 
      send_to_board(&c);
    }
    else if (strcmp(command,"stream")==0){
      char c = 'b';
      send_to_board(&c);
    }
    else if (strcmp(command,"stop")==0){
      char c = 's';
      send_to_board(&c);
    }
    else if (strcmp(command,"channel off")==0){
      mexPrintf("Setting channel settings...\n"); 	//print
      mexEvalString("drawnow;");									//spits out status string immediately
      // int number_of_arguments = strlen(arg);
      // printf("%d\n",number_of_arguments);
      // printf("%s\n", arg);
      // char str[] = &arg;
      // char *wow;
      // wow = strtok(arg," ");
      // printf("%s\n", wow);
      // arg = &argptr;
   		if (channel_number == 8){
      // printf("%d\n",strlen(wow));
        for (int i=0;i<strlen(arg);i++){
          char c = arg[i];
          printf("%c\n",c);
          send_to_board(&c);
          sleep(1);
        }
      }
	    else if (channel_number == 16){
        //for channels 1 through 9
        // printf("%d", strlen(output_buf));
        int output_count = 0;
        for (int i=0;i<(strlen(arg)+1);i++){
          char c = arg[i];
          if ((c!=' ') && (i !=(strlen(arg)))){
            output_buf[output_count] = c;
            output_count++;
          }else{
            if (output_count == 1 && (output_buf[0] >'0' && output_buf[0] < '9')){ 
              char c = output_buf[0];
              send_to_board(&c);
              usleep(250000);
              printf("SENT %c TO BOARD\n", c);
              memset(output_buf,0,3); 
              output_count = 0;
            }else{
    	        if (strcmp(output_buf, "9")==0)
    	        		c = 'q';
    	        else if (strcmp(output_buf,"10")==0)
    	        		c = 'w';
              else if (strcmp(output_buf,"11")==0)
    	        		c = 'e';
              else if (strcmp(output_buf,"12")==0)
    	        		c = 'r';
              else if (strcmp(output_buf,"13")==0)
    	        		c = 't';
    	        else if (strcmp(output_buf,"14")==0)
    	        		c = 'y';
    	        else if (strcmp(output_buf, "15")==0)
    	        		c = 'u';
              else if (strcmp(output_buf, "16")==0)
    	        		c = 'i';
              else {
                printf("Please enter valid channel values (1-16)");
              }
              send_to_board(&c);
              usleep(250000);
              printf("SENT %c TO BOARD\n", c);
              memset(output_buf,0,3); 
              output_count = 0;
            }
          }
        }
	    }
    }
  }
    // else if (strcmp(command,"channel on")==0){
    //   mexPrintf("Setting channel settings...\n"); 	//print
    //   mexEvalString("drawnow;"); 										//spits out status string immediately
    //   for (int i=0;i<strlen(arg);i++){
    //     char c = arg[i];
    //     /* translate channel input to board messaage protocol */
    //     switch(c){
    //     	case '1':
    //     		c = '!';
    //     		break;
    //     	case '2':
    //     		c = '@';
    //     		break;
    //     	case '3':
    //     		c = '#';
    //     		break;
    //     	case '4':
    //     		c = '$';
    //     		break;
    //     	case '5':
    //     		c = '%';
    //     		break;
    //     	case '6':
    //     		c = '^';
    //     		break;
    //     	case '7':
    //     		c = '&';
    //     		break;
    //     	case '8':
    //     		c = '*';
    //     		break;
    //     } 
				// send_to_board(&c);
    //     sleep(1);
    //   }
    // }
    // else if (strcmp(command, "channel settings")==0){
    // 	/* format of 'channel settings command is as follows:
    // 				openbci('channel settings', 'CHANNEL, POWER_DOWN, GAIN_SET, INPUT_TYPE_SET, BIAS_SET, SRB2_SET, SRB1_SET') */
    	
    // 	// To access channel settings, first send an 'x'
    // 	char c = 'x';
    // 	send_to_board(&c);
    // 	sleep(1);

    // 	// select which channel
    // 	if (channel_number==8){
	   //  	char c = arg[0];
	   //  }
    //   else if (channel_number==16){
    //   	if (atoi(arg[0]) < 9){ // if channel selected is 1-8, simple case
    //   		char c = arg[0];
    //   	}else{								// if channel selected is 9-16, do conversion
	   //    	switch(c){
	   //      	case '9':
	   //      		c = 'Q';
	   //      		break;
	   //      	case "10":
	   //      		c = 'W';
	   //      		break;
	   //      	case "11":
	   //      		c = 'E';
	   //      		break;
	   //      	case "12":
	   //      		c = 'R';
	   //      		break;
	   //      	case "13":
	   //      		c = 'T';
	   //      		break;
	   //      	case "14":
	   //      		c = 'Y';
	   //      		break;
	   //      	case "15":
	   //      		c = 'U';
	   //      		break;
	   //      	case "16":
	   //      		c = 'I';
	   //      		break;
	   //      } 
	   //    }
	   //  }
    //   send_to_board(&c);
    //   sleep(1)

    //   // Power Down, 0 (on) or 1 (off)
    //   char c = arg[1];
    //   send_to_board(&c);
    //   sleep(1)

    //   // Gain Set (1,2,4,6,8,12,24)
    //   char c = arg[2];
    //   switch (c):
    //   	case '1':
    //   		c = '0';
    //   		break;
    //   	case '2':
    //   		c = '1';
    //   		break;
    //   	case '4':
    //   		c = '2';
    //   		break;
    //   	case '6':
    //   		c = '3';
    //   		break;
    //   	case '8':
    //   		c = '4';
    //   	case '12':
    //   		c = '5';
    //   		break;
    //   	case '24':
    //   		c = '6';
    //   		break;
    //   send_to_board(&c);
    //   sleep(1);

    //   //Input Type Set
    //   char string = arg[3]; //this needs to accept a string, not just a char
    //   if (strcom(string,"NORMAL")==0){
    //   	char c = '0';
    //   }else if (strcom(string,"INPUT_SHORTED")==0){
    //   	char c = '1';
    //   }else if (strcom(string,"BIAS_MEAS")==0){
    //   	char c = '2';
    //   }else if (strcom(string,"MVDD")==0){
    //   	char c = '3';
    //   }else if (strcom(string,"TEMP")==0){
    //   	char c = '4';
    //   }else if (strcom(string,"TESTSIG")==0){
    //   	char c = '5';
    //   }else if (strcom(string, "BIAS_DRP")==0){
    //   	char c = '6';
    //   }else if (strcom(string, "DRN")==0){
    //   	char c = '7'
    //   }
    //   send_to_board(&c);
    //   sleep(1);

    //   //Bias Set
    //   char c = arg[4];
    //   send_to_board(&c);
    //   sleep(1)

    //   //SRB2 Set
    //   char c = arg[5];
    //   send_to_board(&c)
    //   sleep(1)

    //   //SRB 1 Set
    //   char c = arg[6];
    //   send_to_board(&c);
    //   sleep(1);

    //   //Send a final "X"
    //   char c = 'X';
    //   send_to_board(&c);
    //   sleep(1);
    // }else if (strcmp(command,"set default")==0){
    //   char c = 'd';
    //   send_to_board(&c);
    // }else if (strcmp(command,"get default")==0){
    //   char c = 'D';
    //   send_to_board(&c);
    // }else if (strcmp(command, "impedence")==0){
    // 	// to initiate impedence testing, send a 'z' to the board
    // 	char c = 'z';
    // 	send_to_board(&c);
    // 	sleep(1);

    // 	// Channel number
    // 	if (channel_number==8){
	   //  	char c = arg[0];
	   //  }else if (channel_number==16){
    //   	if (atoi(arg[0]) < 9){ // if channel selected is 1-8, simple case
    //   		char c = arg[0];
    //   	}else{								// if channel selected is 9-16, do conversion
	   //    	switch(c){
	   //      	case '9':
	   //      		c = 'Q';
	   //      		break;
	   //      	case '10':
	   //      		c = 'W';
	   //      		break;
	   //      	case '11':
	   //      		c = 'E';
	   //      		break;
	   //      	case '12':
	   //      		c = 'R';
	   //      		break;
	   //      	case '13':
	   //      		c = 'T';
	   //      		break;
	   //      	case '14':
	   //      		c = 'Y';
	   //      		break;
	   //      	case '15':
	   //      		c = 'U';
	   //      		break;
	   //      	case '16':
	   //      		c = 'I';
	   //      		break;
	   //      } 
	   //    }
	   //  }
	   //  send_to_board(&c);
	   //  sleep(1);

	   //  //PCHAN (0 or 1)
	   //  char c = arg[1];
    // 	send_to_board(&c);
    // 	sleep(1);

    // 	//NCHAN (0 or 1)
    // 	char c = arg[2];
    // 	send_to_board(&c);
    // 	sleep(1);

    // 	//send a 'Z' to the board to apply impedence
    // 	char c = 'Z';
    // 	send_to_board(&c);
    // 	sleep(2);
    // }else if (strcmp(command,"SD record")==0){
    // 	char string[] = arg[0];
    // 	if (strcmp(string,"5MIN")==0)
    // 		char c = 'A';
    // 	else if (strcmp(string,"15MIN")==0)
    // 		char c = 'S';
    // 	else if (strcmp(string,"30MIN")==0)
    // 		char c = 'F';
    // 	else if (strcmp(string,"1HR")==0)
    // 		char c = 'G';
    // 	else if (strcmp(string,'2HR')==0)
    // 		char c = 'H';
    // 	else if (strcmp(string, '4HR')==0)
    // 		char c = 'J';
    // 	else if (strcmp(string,'12HR')==0)
    // 		char c = 'K';
    // 	else if (strcmp(string,'24HR')==0)
    // 		char c = 'L';
    // 	else if (strcmp(string,'test')==0)
    // 		char c = 'a';
    // 	send_to_board(&c);
    // }else if (strcmp(command,"SD save")==0){
    // 	char c = 'j';
    // 	send_to_board(&c);
    // }else if (strcmp(command,"registry")==0){
    //   char c = '?';
    //   send_to_board(&c);
    // }


void board_information(){
	printf("sup\n");
}