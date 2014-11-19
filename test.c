#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFSIZE 4096

extern int errno;

int main(int argc, char* argv[])
{
	int fid;
	char buffer[BUFFSIZE];
	size_t bytes_read;

	fid = open("/dev/asciimap", O_RDONLY);

	if (fid == -1)
	{
		fprintf(stderr, "Failed to open /dev/asciimap\n");
		perror(NULL);
		exit(1);
	}

	/* Doing 2 reads, now */
	{
		bytes_read = read(fid, buffer, 20); /* Save space for NULL */

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

	printf("\n\nReading again...\n");

	{
		bytes_read = read(fid, buffer, 20);
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

	return 0;
}
