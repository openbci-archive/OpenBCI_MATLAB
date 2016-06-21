/*
	This class simulates data already streaming from the serial port, and
	ready to be processed and imported into MATLAB.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>


FILE *fp;
size_t nread;
void sleep_wrapper(int i);
struct timeval stop, start;

void main(){
	fp = fopen("sample_data.txt", "r");
	int i = 0;
  char buffer[4096];
  gettimeofday(&start,NULL);
  while (fgets(buffer, sizeof(buffer), fp) != 0){
    fputs(buffer, stdout);
		sleep_wrapper(i);
		i++;
	}

	if (ferror(fp)) {
			/* deal with error */
		printf("Error reading file");
	}
	fclose(fp);
}

void sleep_wrapper(int i){
	gettimeofday(&stop,NULL);
	float program_time = stop.tv_usec - start.tv_usec;
	float recording_time = (i/.0250000000000000000)+5;
	printf("\n");
	printf("%d\n",i);
	printf("PGRM TIME %f\n ", program_time);
	printf("REC TIME %f\n",recording_time);
	sleep((program_time-recording_time)/1000000);

}