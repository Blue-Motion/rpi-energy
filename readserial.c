#include <stdio.h>
#include <unistd.h>			/*Used for UART*/
#include <fcntl.h>			/*Used for UART*/
#include <termios.h>			/*Used for UART*/

#include <time.h>
#include <string.h>
#include <stdlib.h>

#define NUM_LINES 28

int main(){

	/*-------------------------
	//----- SETUP USART 0 -----
	//-------------------------
	At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively*/
	int uart0_filestream = -1, i;
        FILE *fp;
	unsigned char datalines[NUM_LINES][100];
	struct termios options;

	/*OPEN THE UART
	//The flags (defined in fcntl.h):
	//	Access modes (use 1 of these):
	//		O_RDONLY - Open for reading only.
	//		O_RDWR - Open for reading and writing.
	//		O_WRONLY - Open for writing only.
	//
	//	O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
	//											if there is no input immediately available (instead of blocking). Likewise, write requests can also return
	//											immediately with a failure status if the output can't be written immediately.
	//
	//	O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.*/
	uart0_filestream = open("/dev/ttyAMA0", O_RDONLY | O_NOCTTY);
	if (uart0_filestream == -1)
	{
		printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
	}

	/*CONFIGURE THE UART
	//The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
	//	CSIZE:- CS5, CS6, CS7, CS8
	//	CLOCAL - Ignore modem status lines
	//	CREAD - Enable receiver
	//	IGNPAR = Ignore characters with parity errors
	//	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
	//	PARENB - Parity enable
	//	PARODD - Odd parity (else even)*/
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B115200 | CS7 | CLOCAL | CREAD | IGNCR | PARENB | PARODD;
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = ICANON;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);


	while (1)
	{
		for(i=0;i<NUM_LINES;i++){

			int rx_length = read(uart0_filestream, (void*)datalines[i], 99);
			datalines[i][rx_length] = '\0';
		}

               fp = fopen ("energemeter-dump.txt", "a");


		for(i=0;i<NUM_LINES;i++){
			/*strip useful data and store it*/
			printf("%s", datalines[i] );
			fprintf(fp, "%s", datalines[i]);
		}

		fclose(fp);

	}


	/*If the while loop ever closes
	//----- CLOSE THE UART -----*/
	close(uart0_filestream);


}
