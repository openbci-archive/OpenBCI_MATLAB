/*
*   OpenBCI MATLAB interface
 *
*   Establishes a user interface in order to allow communication
*   with the OpenBCI board via MATLAB
*
*/

#include "mex.h"
#include <stdio.h>			 		// standard input/output definitions
#include <termios.h>				// POSIX terminal control definitions
#include <fcntl.h>					// File Control definitions
#include <unistd.h>					// UNIX standard function definitions
#include <errno.h>					//
#include <sys/types.h>
#include <cstring>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "openbci_matlab.c"

/* Function declarations */
int mexEvalString(const char *command);
void open_helper(char* arg);
void close_helper();
void reset();
void stream();
void stop();
void channel_off(char* arg);
void channel_on(char* arg);
void channel_settings(char* arg);
void set_default();
void get_default();
void impedence(char* arg);
void sd_record(char* arg);
void sd_save();
void register_settings();
void board_information();

int channel_number = 16;

/* The gateway function*/
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]){	
	// char prhs_arg = *prhs;
	char *port, *command, *arg;
	size_t buflen;
  char output_buf[3];
  char c;
  buflen = (mxGetM(prhs[0]) * mxGetN(prhs[0])) + 1;     //length of input string
  command = mxArrayToString(prhs[0]);           //holds the port string   

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

				
	/* Evaluate Coammnds */
	if (strcmp(command,"open")==0)
		open_helper(arg); //in openbci_matlab.c
	else if (strcmp(command,"close")==0)
		close_helper(); //in openbci_matlab.c
	else if (strcmp(command,"reset")==0)
    reset();
  else if (strcmp(command,"stream")==0)
    stream();
  else if (strcmp(command,"stop")==0)
    stop();
  else if (strcmp(command,"channel off")==0)
    channel_off(arg);
  else if (strcmp(command,"channel on")==0)
    channel_on(arg);
  else if (strcmp(command, "channel settings")==0)
    channel_settings(arg);
  else if (strcmp(command, "impedence")==0)
    impedence(arg);
  else if (strcmp(command, "sd record")==0)
    sd_record(arg);
  else if (strcmp(command, "sd save")==0)
    sd_save();
  else if (strcmp(command, "register settings")==0)
    register_settings();
}
void open_helper(char* arg){
  initialize_port(arg);
}
void close_helper(){
  close_port();
}
void reset(){
  char c = 'v'; 
  send_to_board(&c);
}
void stream(){
  char c = 'b';
  send_to_board(&c);
}
void stop(){
  char c = 's';
  send_to_board(&c);
}
void channel_off(char* arg){
  char output_buf[3];

  mexPrintf("Setting channel settings...\n");    //print
  mexEvalString("drawnow;");                     //pritnts out status string immediately
  
  // if 8 channel board is set
  if (channel_number == 8){
    for (int i=0;i<strlen(arg);i++){
      char c = arg[i];
      send_to_board(&c);
      usleep(250000);
    }
  }
  // else if 16 channel board is set
  else if (channel_number == 16){
    int output_count = 0;
    // iterate through each argument (channel value)
    for (int i=0;i<(strlen(arg)+1);i++){
      char c = arg[i];
      // add the characters of each argument until you find a space
      if ((c!=' ') && (i !=(strlen(arg)))){
        output_buf[output_count] = c;
        output_count++;
      }
      // when space or end is found, send appropriate command to board
      else{
        //if channels 1-8 are chosen, send characters '1' through '8'
        if (output_count == 1 && (output_buf[0] >'0' && output_buf[0] < '9')){ 
          c = output_buf[0];
        }
        // send commands if channels 9-16 are chosen
        else{
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
void channel_on(char* arg){
  char output_buf[3];

  mexPrintf("Setting channel settings...\n");   //print
  mexEvalString("drawnow;");                  //spits out status string immediately

  // if 8 channel
  if (channel_number == 8){
    for (int i=0;i<strlen(arg);i++){
      char c = arg[i];
        switch(c){
          case '1':
            c = '!';
            break;
          case '2':
            c = '@';
            break;
          case '3':
            c = '#';
           break;
         case '4':
           c = '$';
           break;
         case '5':
           c = '%';
           break;
         case '6':
           c = '^';
           break;
         case '7':
           c = '&';
           break;
         case '8':
           c = '*';
           break;
        } 
          send_to_board(&c);
          usleep(250000);
          printf("SENT %c TO BOARD\n", c);
    }
  }
  //if 16 channel
  else if (channel_number == 16){
    int output_count = 0;
    for (int i=0;i<(strlen(arg)+1);i++){
      char c = arg[i];
      if ((c!=' ') && (i !=(strlen(arg)))){
        output_buf[output_count] = c;
        output_count++;
      }else{
        if (output_count == 1 && (output_buf[0] >'0' && output_buf[0] < '9')){ 
          char c = output_buf[0];
          switch(c){
           case '1':
             c = '!';
             break;
           case '2':
             c = '@';
             break;
           case '3':
             c = '#';
             break;
           case '4':
             c = '$';
             break;
           case '5':
             c = '%';
             break;
           case '6':
             c = '^';
             break;
           case '7':
             c = '&';
             break;
           case '8':
             c = '*';
             break;
          } 
          send_to_board(&c);
          usleep(250000);
          printf("SENT %c TO BOARD\n", c);
          memset(output_buf,0,3); 
          output_count = 0;
        }else{
          if (strcmp(output_buf, "9")==0)
              c = 'Q';
          else if (strcmp(output_buf,"10")==0)
              c = 'W';
          else if (strcmp(output_buf,"11")==0)
              c = 'E';
          else if (strcmp(output_buf,"12")==0)
              c = 'R';
          else if (strcmp(output_buf,"13")==0)
              c = 'T';
          else if (strcmp(output_buf,"14")==0)
              c = 'Y';
          else if (strcmp(output_buf, "15")==0)
              c = 'U';
          else if (strcmp(output_buf, "16")==0)
              c = 'I';
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
void channel_settings(char* arg){
  int output_count = 0;
  char output_buf[15];
  int index = 0;

  /* format of 'channel settings command is as follows:
      openbci('channel settings', 'CHANNEL, POWER_DOWN, GAIN_SET, INPUT_TYPE_SET, BIAS_SET, SRB2_SET, SRB1_SET') */

  // To access channel settings, first send an 'x'
  char c = 'x';
  send_to_board(&c);
  usleep(250000);

  // select which channel
  if (channel_number==8){
    c = arg[index];
    index++;
  }else if (channel_number == 16){
    while (arg[index]!=' '){
      output_buf[output_count] = arg[index];
      index++;
      output_count++;
    }
    //if channels 1-8 are chosen, send characters '1' through '8'
    if (index == 1 && (output_buf[0] >'0' && output_buf[0] < '9')){ 
      c = output_buf[0];
    }
    // send commands if channels 9-16 are chosen
    else{
      if (strcmp(output_buf, "9")==0)
          c = 'Q';
      else if (strcmp(output_buf,"10")==0)
          c = 'W';
      else if (strcmp(output_buf,"11")==0)
          c = 'E';
      else if (strcmp(output_buf,"12")==0)
          c = 'R';
      else if (strcmp(output_buf,"13")==0)
          c = 'T';
      else if (strcmp(output_buf,"14")==0)
          c = 'Y';
      else if (strcmp(output_buf, "15")==0)
          c = 'U';
      else if (strcmp(output_buf, "16")==0)
          c = 'I';
      else
        printf("Please enter valid channel values (1-16)");
    }
    send_to_board(&c);
    usleep(250000);
    printf("SENT %c TO BOARD\n", c);
    memset(output_buf,0,3); 
  }
  index++;

  // Power Down, 0 (on) or 1 (off)
  c = arg[index];
  send_to_board(&c);
  printf("SENT %c TO BOARD\n", c);
  usleep(250000);
  index+=2;

  // Gain Set (1,2,4,6,8,12,24)
  output_count = 0;
  while(arg[index]!=' '){
    output_buf[output_count] = arg[index];
    index++;
    output_count++;
  }
  if (strcmp(output_buf, "1")==0)
      c = '0';
  else if (strcmp(output_buf,"2")==0)
      c = '1';
  else if (strcmp(output_buf,"4")==0)
      c = '2';
  else if (strcmp(output_buf,"6")==0)
      c = '3';
  else if (strcmp(output_buf,"8")==0)
      c = '4';
  else if (strcmp(output_buf,"12")==0)
      c = '5';
  else if (strcmp(output_buf, "24")==0)
      c = '6';
  else{
    c = ' ';
    printf("Enter correct gain set: 1, 2, 4, 6, 8, 12, or 24"); //maybe this should be a real error
  }
  send_to_board(&c);
  printf("SENT %c TO BOARD\n", c);
  memset(output_buf,0,3); 
  usleep(250000);
  index++;

  //Input Type Set
  output_count = 0;
  while(arg[index]!=' '){
    output_buf[output_count] = arg[index];
    index++;
    output_count++;
  }
  if (strcmp(output_buf,"NORMAL")==0){
   c = '0';
  }else if (strcmp(output_buf,"INPUT_SHORTED")==0){
   c = '1';
  }else if (strcmp(output_buf,"BIAS_MEAS")==0){
   c = '2';
  }else if (strcmp(output_buf,"MVDD")==0){
   c = '3';
  }else if (strcmp(output_buf,"TEMP")==0){
   c = '4';
  }else if (strcmp(output_buf,"TESTSIG")==0){
   c = '5';
  }else if (strcmp(output_buf, "BIAS_DRP")==0){
   c = '6';
  }else if (strcmp(output_buf, "DRN")==0){
   c = '7';
  }
  else{
    c = ' ';
    printf("Enter correct BIAS settings: 1-7 (see docs.openbci.com)");
  }
  send_to_board(&c);
  printf("SENT %c TO BOARD\n", c);
  memset(output_buf,0,15); 
  usleep(250000);
  index++;

  //Bias Set
  c = arg[index];
  send_to_board(&c);
  printf("SENT %c TO BOARD\n", c);
  usleep(250000);
  index+=2;

  //SRB2 Set
  c = arg[index];
  send_to_board(&c);
  printf("SENT %c TO BOARD\n", c);
  usleep(250000);
  index+=2;

  //SRB 1 Set
  c = arg[index];
  send_to_board(&c);
  printf("SENT %c TO BOARD\n", c);
  usleep(250000);
  index+=2;

  //Send a final "X"
  c = 'X';
  send_to_board(&c);
  printf("SENT %c TO BOARD\n", c);
  usleep(250000);
  index+=2;
}
void set_default(){
  char c = 'd';
  send_to_board(&c);
}
void get_default(){
  char c = 'D';
  send_to_board(&c);
}
void impedence(char* arg){
  int index = 0;
  char output_buf[3];
  int output_count = 0;
  char c;

  // to initiate impedence testing, send a 'z' to the board
  c = 'z';
  send_to_board(&c);
  usleep(250000);

   // Channel number
  if (channel_number==8){
    c = arg[index];
    index++;
  }else if (channel_number == 16){
    while (arg[index]!=' '){
      output_buf[output_count] = arg[index];
      index++;
      output_count++;
    }
    //if channels 1-8 are chosen, send characters '1' through '8'
    if (index == 1 && (output_buf[0] >'0' && output_buf[0] < '9')){ 
      c = output_buf[0];
    }
    // send commands if channels 9-16 are chosen
    else{
      if (strcmp(output_buf, "9")==0)
          c = 'Q';
      else if (strcmp(output_buf,"10")==0)
          c = 'W';
      else if (strcmp(output_buf,"11")==0)
          c = 'E';
      else if (strcmp(output_buf,"12")==0)
          c = 'R';
      else if (strcmp(output_buf,"13")==0)
          c = 'T';
      else if (strcmp(output_buf,"14")==0)
          c = 'Y';
      else if (strcmp(output_buf, "15")==0)
          c = 'U';
      else if (strcmp(output_buf, "16")==0)
          c = 'I';
      else
        printf("Please enter valid channel values (1-16)");
    }
    send_to_board(&c);
    usleep(250000);
    printf("SENT %c TO BOARD\n", c);
    memset(output_buf,0,3); 
  }
  index++;
  //PCHAN (0 or 1)
  c = arg[index];
  send_to_board(&c);
  printf("SENT %c TO BOARD\n", c);
  usleep(250000);
  index+=2;
  //NCHAN (0 or 1)
  c = arg[index];
  send_to_board(&c);
  printf("SENT %c TO BOARD\n", c);
  usleep(250000);
  index+=2;
  //send a 'Z' to the board to apply impedence
  c = 'Z';
  send_to_board(&c);
  usleep(250000);
}
void sd_record(char* arg){
  int index = 0;
  char output_buf[5];
  int output_count = 0;
  char c;

  // Amount of Time
  for (int index=0;index<(strlen(arg)+1);index++){
    output_buf[index] = arg[index];
  }
  if (strcmp(output_buf, "5MIN")==0)
    c = 'A';
  else if (strcmp(output_buf,"15MIN")==0)
    c = 'S';
  else if (strcmp(output_buf,"30MIN")==0)
    c = 'F';
  else if (strcmp(output_buf,"1HR")==0)
    c = 'G';
  else if (strcmp(output_buf,"2HR")==0)
    c = 'H';
  else if (strcmp(output_buf,"4HR")==0)
    c = 'J';
  else if (strcmp(output_buf,"12HR")==0)
    c = 'K';
  else if (strcmp(output_buf,"24HR")==0)
    c = 'L';
  else if (strcmp(output_buf,"test")==0)
    c = 'a';
  else
    printf("Enter acceptable time: 5MIN, 15MIN, 30MIN, 1HR, 2HR, 4HR, 12HR, 24HR, or test\n");
  send_to_board(&c);
  printf("SENT %c TO BOARD\n", c);
  usleep(250000);
}
void sd_save(){
  char c = 'j';
  send_to_board(&c);
}
void register_settings(){
  char c = '?';
  send_to_board(&c);
}
