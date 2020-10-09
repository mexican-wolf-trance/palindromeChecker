#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
	int option, proc_num = 2, con_proc = 4, proc_time = 100;
	char file[32];	

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

	

	return 0;
}

