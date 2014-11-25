#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

char* carveFile(char* fileName, int width, int height, int lineNum);
void printArgUsage();

int main(int argc, char* argv[])
{
	int option = 0;
	char* width = "10";
	char* height = "10";
	int line = 1;

	while((option = getopt(argc, argv, "w:h:l:")) != -1)
	{
		switch(option)
		{
			case 'w':
				width = optarg;
				break;
			case 'h':
				height = optarg;
				break;
			case 'l':
				line = atoi(optarg);
				break;
			default:
				printArgUsage();
				break;
		}
	}
	int nonOptArgc = argc - optind + 1;

	if(nonOptArgc == 1) /* just exec genmap.sh */
	{
		pid_t pid = fork();

		if(pid == 0) /* child */
		{
			char* genmapArgv[] = {"./genmap.sh", width, height, NULL};
			char* genmapEnviron[] = {NULL};
			execve("./genmap.sh", genmapArgv, genmapEnviron);

			/* if we reach here execve has failed */
			perror("execve() has failed");
			exit(1);
		}
		else /* parent */
		{
			printf("This kid's PID: %d\n", pid);
			printf("I will bravely wait for my children!\n");
			wait(NULL);
			printf("Finally rid of those brats\n");
		}
	}
	else
	{
		int i = optind;
		for(; i < argc; ++i)
		{
			pid_t pid = fork();

			if(pid == 0) /* carve a file with my new process */
			{
				// carve file
				char* carvedMap = carveFile(argv[i], atoi(width), atoi(height), line);

				printf("%s", carvedMap);
				exit(0);
			}
			else
			{
				printf("This kid's PID: %d\n", pid);
			}
		}

		/* only parent gets here, child has already exited */
		printf("I will bravely wait for my children!\n");

		while(wait(NULL) > 0){}

		printf("Finally rid of those brats\n");
	}

	exit(0);
}

/* TODO: implement starting at a line and figure out tabs */
char* carveFile(char* fileName, int width, int height, int lineNum)
{
	FILE* fileDesc;
	char* mapLine;
	if(fileDesc = fopen(fileName, "r"))
	{
		int mapSize = width * height + height + 1;
		mapLine = (char*)malloc(mapSize); /* allocate buffer */
		memset(mapLine, 0, mapSize); /* zero out buffer */

		char* line = NULL;
		size_t len = 0;
		ssize_t read;

		int writeStart;
		int writeEnd;

		int itr = 1; /* counts current row */
		while(read = getline(&line, &len, fileDesc) != -1 && itr <= height)
		{
			strncat(mapLine, line, width);

			writeStart = width * (itr - 1) + (itr - 1);
			writeEnd = width * itr + (itr - 1);

			int lineEnd = writeStart + strlen(line) - 1;
			if(lineEnd < writeEnd) /* if line was too short, fill out with spaces */
			{
				int i = lineEnd;
				for(; i < writeEnd; ++i)
				{
					mapLine[i] = ' ';
				}
			}

			mapLine[writeEnd] = '\n'; /* end each line with a newline char */
			itr++;
		}

		if(writeEnd < mapSize - 2) /* if file was too short, fill out with spaces */
		{
			while(itr <= height)
			{
				writeStart = width * (itr - 1) + (itr - 1);
				writeEnd = width * itr + (itr - 1);

				int j = writeStart;
				for(; j < writeEnd; ++j)
				{
					mapLine[j] = ' ';
				}

				mapLine[writeEnd] = '\n';

				itr++;
			}
		}

		/* Reached EOF, print values */
		fclose(fileDesc);
	}
	else
	{
		printf("The file %s doesn't exist!\n", fileName);
	}

	return mapLine;
}

void printArgUsage()
{
	printf("Invalid arg.\nValid args are -w (width), -h (height), -l (line)\n");
}
