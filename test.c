#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

#define BUFFSIZE (4096 * 2)

extern int errno;

static void readAndPrintBuffer(int fid, char* buffer, int numChar)
{
	size_t bytes_read;

	bytes_read = read(fid, buffer, numChar);
	if (bytes_read == -1)
	{
		fprintf(stderr, "Failed to read from file\n");
		perror(NULL);
		exit(1);
	}

	buffer[bytes_read] = '\0';
	printf("Read %d bytes\n", bytes_read);
	printf("12345678901234567890123456789012345678901234567890\n");
	printf("         1         2         3         4         5\n");

	printf("%s\n", buffer);
}

#define ERROR_CHECK(x) \
	if (x == -1) \
	{ \
		fprintf(stderr, "Failed to open /dev/asciimap\n"); \
		perror(NULL); \
		exit(1); \
	}

int main(int argc, char* argv[])
{
	int fid;
	char buffer[BUFFSIZE];
	int bytes_written;

	fid = open("/dev/asciimap", O_RDONLY);

	ERROR_CHECK(fid);

	/* Doing 2 reads, now */
	readAndPrintBuffer(fid, buffer, 20);

	printf("\n\nReading again...\n");

	readAndPrintBuffer(fid, buffer, 20);

	/* Now close it, reopen it, and try reading again */

	printf("Closing...\n");
	close(fid);

	printf("Re-opening...\n");
	fid = open("/dev/asciimap", O_RDONLY);

	ERROR_CHECK(fid);

	readAndPrintBuffer(fid, buffer, 20);

	printf("\n\nReading again...\n");

	readAndPrintBuffer(fid, buffer, 20);

	close(fid);

	/* Open again and do some writing */

	fid = open("/dev/asciimap", O_WRONLY);

	ERROR_CHECK(fid);

	{
		//assert(20 < BUFFSIZE);

		int i;
		for (i = 0; i < BUFFSIZE; i++)
		{
			buffer[i] = '0';
		}
	}

	bytes_written = write(fid, buffer, BUFFSIZE);

	printf("Wrote %d bytes\n", bytes_written);

	close(fid);

	fid = open("/dev/asciimap", O_RDONLY);
	lseek(fid, 0, SEEK_SET);

	readAndPrintBuffer(fid, buffer, BUFFSIZE);

	return 0;
}
