#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define BUFSIZE 1024
#define SHM_KEY 0x1234


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

        if(proc_num < con_proc)
        {
                printf("I noticed you wanted more concurrent processes than you indicated would be the max number of processes. That's dumb. I'll make them equivalent so we can continue.\n");
                proc_num = con_proc;
        }

	if(proc_num > 20)
	{
		printf("You entered too processes. We'll keep it to 20.\n");
		proc_num = 20;
	}


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

	fclose(fp);
	
	//String manipulation section
	int i, j = 0, k = 0, strSize = 0;
	//remove the spaces from the string and decapitalize
	for (i = 0; i < strlen(input); i++)
	{
		input[i] = input[i + k];
		if (input[i] == ' ')
		{
			k++;
			i--;
		}
		if (isupper(input[i]))
                        input[i] = tolower(input[i]);
	}
	//determine number of lines in the input string
	for (i = 0; i < strlen(input); i++)
	{
		if (input[i] == '\t' || input[i] == '\n' || input[i] == '\r')
			strSize++;
	}
	
	//create the shared memory variable based on the number of lines in the input
        int shmid;
        char *shmPtr;
        shmid = shmget(SHM_KEY, 80 * strSize, 0644|IPC_CREAT);
        if(shmid == -1)
        {
                perror("Shared memory");
                return 1;
        }

        shmPtr = (char *) shmat(shmid, NULL, 0);
        if (shmPtr == (void *) -1)
        {
                perror("Shared memoery attachment");
                return 1;
        }

	//add strings to presized segements of the array and fill the rest with \0 for easy access
	for (i = 0; i < strlen(input); i++)
	{		
		if (input[i] == '\t' || input[i] == '\n' || input[i] == '\r')
		{
			for (; (j % 80) != 0; j++);
				shmPtr[j] = '\0';	
			i++;
		}
		shmPtr[j] = input[i];
		j++;
	}
	//make sure the string copied over correctly by printing it to the console
	j = 0;
	printf("Strings present in shmPtr:\n");
	for (i = 0; i < strSize * 80; i++)
	{
		if (shmPtr[i] == '\0')
		{	
			printf(" ");
			j++;
			i = 80*j;
		}	
		printf("%c", shmPtr[i]);
	}
	printf("\n");
	//copy the new string into shared memory
//	strcpy(shmPtr, input);
//	strcpy(shmPtr, temp);
	free(input);

	//Children factory
	i = 0;
	while(proc_num > 0)
	{
		if(con_proc == counter)
		{
			wait(NULL);
			counter--;
		}

		counter++;
		char b[2];
		snprintf(b, 2, "%d", i);

		exec[1] = b;
		exec[2] = NULL;

		i++;
		if (i == strSize)
			i = 0;
		
		if ((child = fork()) == 0) 
		{
			execvp(exec[0], exec);
			perror("Exec failed");
		}
		if (child < 0)
		{
			perror("Failed to fork\n");
			exit(1);
		}	

		if (waitpid(-1, NULL, WNOHANG) > 0)
			counter--;
		proc_num--;
	}

	if (child > 0)
		while(wait(NULL) > 0);

	printf("Should be done!\n");

	shmdt(shmPtr);
	shmctl(shmid, IPC_RMID, NULL);
		
	return 0;
}

