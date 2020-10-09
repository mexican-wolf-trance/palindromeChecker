#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFSIZE 1024

int main(int argc, char **argv)
{
	int option, proc_num = 4, con_proc = 2, proc_time = 100, counter = 0;
	char file[32], *exec[] = {"./palin", NULL}, *input;	
	pid_t child = 0;
	FILE *fp;

	if (argc < 2)
	{
		printf("Invalid usage! Check the readme\nUsage: master [-n x] [-s x] [-t time] file\n");
		return 0;
	}

	while ((option = getopt(argc, argv, "hn:s:t:")) != -1)
	switch (option)
	{
		case 'h':
			printf("This program checks palindromes! Use it correctly!\n");
			printf("Usage: master[-n x] [-s x] [-t time] file\n");
			printf("-n is the number total processes\n-s is the number of concurrent processes\n-t is the total time the program will run before quitting (unless the file is processed completely)\nfile is the file we will be checking for palindromes\n");
			printf("The default vaules are n = 2, s = 4, t = 100 (but we still need you to enter the file)\n");
			return 0;
		case 'n':
			proc_num = atoi(optarg);
			break;
		case 's':
			con_proc = atoi(optarg);
			break;
		case 't':
			proc_time = atoi(optarg);
			break;
		case '?':
			printf("%c is not an option\n", optopt);
	}
	strcpy(file, argv[optind]);
	printf("You entered these options: -n %d -s %d -t %d %s\n", proc_num, con_proc, proc_time, file);

	if((fp = fopen(file, "r")) == NULL)
	{
		perror("Failed to open file");
		return 1;
	}
	else
	{
		fseek (fp, 0, SEEK_END);
                long length = ftell(fp);
                fseek (fp, 0, SEEK_SET);
               	input = malloc(length);
                if(input != NULL)
                	fread(input, 1, length, fp);
	}
	
	printf("Input %s\n", input);
	fclose(fp);

	if(proc_num < con_proc)
	{
		printf("I noticed you wanted more concurrent processes than you indicated would be the max number of processes. That's dumb. I'll make them equivalent so we can continue.\n");
		proc_num = con_proc;
	}

	while(proc_num > 0)
	{
		if(con_proc == counter)
		{
			wait(NULL);
			counter--;
		}

		counter++;

		if ((child = fork()) == 0) 
		{
			execvp(exec[0], exec);
			return 1;
		}
		if (child < 0)
		{
			perror("Failed to fork\n");
			return 1;
		}

		if (waitpid(-1, NULL, WNOHANG) > 0)
			counter--;
		proc_num--;
	}

	if (child > 0)
		while(wait(NULL) > 0);

	printf("Should be done!\n");
		
	return 0;
}

