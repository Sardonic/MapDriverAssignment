#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFSIZE 512

extern int errno;

int main(int argc, char* argv[])
{
	int fid;
	char buffer[BUFFSIZE];

	fid = open("/dev/asciimap", O_RDONLY);

	if (fid == -1)
	{
		fprintf(stderr, "Failed to open /dev/asciimap\n");
		perror(NULL);
	}

	read(fid, buffer, BUFFSIZE);

	fprintf(stdout, "%s\n", buffer, BUFFSIZE);

	return 0;
}
