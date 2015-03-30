#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>

#define DEVICE_NAME "/dev/fakedevice"

bool CheckRoot(void) {return getuid()==0;}

int main () {

	int i, fd;
	ssize_t written, readd;
	char ch, write_buf[100], read_buf[100];

	/* Check if user running this program is actually root */ 
	if(CheckRoot()) {
		
		
		fd = open(DEVICE_NAME, O_RDWR);
		
		if (fd == -1) {		// Check for Errors		
			printf("Failed to open device %s. Device is busy or there is not an appropriate kernel module loaded?\n", DEVICE_NAME);
			exit(-1);
		}

		printf("r = read from device\nw = write to device\nenter command: ");
		scanf("%c", &ch);

		/* Based on user input do appropriate routine */ 
		switch (ch)
		{
			case 'w':
				printf("enter data: ");
				scanf(" %[^\n]", write_buf);
				written = write(fd, write_buf, sizeof(write_buf));
				printf("bytes written: %zu\n", written);
				break;
			case 'r':
				readd = read(fd, read_buf, sizeof(read_buf));
				printf("device data: %s\n", read_buf);
				printf("bytes read: %lu\n", sizeof(read_buf));
				break;
			default:
				printf("command not recognised\n");
				break;

		}
		
		close(fd);		// Close file

	}
	else {
		printf("You have to be root to run this program!\n");	// Inform user to get root
	}

	return 0;

}


