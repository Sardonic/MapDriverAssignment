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

	bytes_read = read(fid, buffer, BUFFSIZE - 1); /* Save space for NULL */

	if (bytes_read == -1)
	{
		fprintf(stderr, "Failed to read from file\n");
		perror(NULL);
		exit(1);
	}
	else
	{
		buffer[bytes_read] = '\0';
		printf("Read %d bytes\n", bytes_read);
	}

	printf("%s\n", buffer);

	return 0;
}
