#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#define BUFSIZE 1024
#define SHM_KEY 0x12345
#define MSG_KEY 0x98765

struct msgbuf
{
	long mtype;
	int msg;
} message;

int shmid, msgqid;
char *shmPtr;

void sigint(int sig)
{
        shmdt(shmPtr);
        shmctl(shmid, IPC_RMID, NULL);
	if (msgctl(msgqid, IPC_RMID, NULL) == -1)
                fprintf(stderr, "Message queue could not be deleted\n");

        if(sig == SIGALRM)
                write(1, "\nAlarm! Alarm!\n", 15);
        else if(sig == SIGSEGV)
                write(1, "\nWoops! You got a segmentation fault!\n", 38);
        else if(sig == SIGINT)
                write(1, "\nCTRL C was hit!\n", 17);
        else
                write(1, "Oh no! An error!\n", 17);

        write(1, "Now killing the kiddos\n", 23);
        kill(0, SIGQUIT);
}

int main(int argc, char **argv)
{
	signal(SIGINT, sigint);
        signal(SIGSEGV, sigint);

	int option, proc_num = 4, con_proc = 2, proc_time = 100, counter = 0;
	char b[2], file[32], *exec[] = {"./palin", NULL, NULL}, *input;	
	pid_t child = 0;
	FILE *fp;

	//Check there is the correct number of arguments
	if (argc < 2)
	{
		printf("Invalid usage! Check the readme\nUsage: master [-n x] [-s x] [-t time] file\n");
		return 0;
	}

	//Create and validate the options
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
			printf("%c is not an option. Use -h for usage details\n", optopt);
	}
	strcpy(file, argv[optind]);

	//Display the options chosen
	printf("You entered these options: -n %d -s %d -t %d %s\n", proc_num, con_proc, proc_time, file);

	//Want to make sure the options make sense
        if(proc_num < con_proc)
        {
                printf("I noticed you wanted more concurrent processes than you indicated would be the max number of processes. That's dumb. I'll make them equivalent so we can continue.\n");
                proc_num = con_proc;
        }

	if(proc_num > 20 && con_proc > 20)
	{
		printf("That's a lot of processes. We'll keep the number of concurrent process to 20.\n");
		con_proc = 20;
	}

	//set up message queue
        msgqid = msgget(MSG_KEY, 0644 | IPC_CREAT);
        if (msgqid == -1)
        {
                perror("msgqid get failed");
                return 1;
        }

	//Read the chosen file
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
	int i, j = 0, strSize = 0;
	//remove the spaces and special characters from the string and decapitalize
	for (i = 0; input[i] != '\0'; i++)
	{
		if ((input[i] >= 'A' && input[i] <= 'Z') || (input[i] >= 'a' && input[i] <= 'z') || input[i] == '\t' || input[i] == '\n' || input[i] == '\r')
		{
			input[j] = input[i];
			j++;
		}
	}
	for (i = 0; input[i] != '\0'; i++)
	{
		if (isupper(input[i]))
                        input[i] = tolower(input[i]);
	}
	//determine number of lines in the input string
	for (i = 0; i < strlen(input); i++)
	{
		if (input[i] == '\t' || input[i] == '\n' || input[i] == '\r')
			strSize++;
	}
	//set up shared memory
        if ((shmid = shmget(SHM_KEY, sizeof(shmPtr), 0644|IPC_CREAT)) < 0)
        {
                perror("Shmget failed");
                exit(1);
        }

        if ((shmPtr = (char *) shmat(shmid, NULL, 0)) < 0)
        {
                perror("Shmat failed");
                exit(1);
        }

	//add strings to presized segements of the array and fill the rest with \0 for easy access
	j = 0;
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
	//free malloc
	free(input);
	
	message.mtype = 1;
	message.msg = 1;
	msgsnd(msgqid, &message, sizeof(message), 0);

	alarm(proc_time);
	signal(SIGALRM, sigint);
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
		//This determines the starting index in the palin exe
		snprintf(b, 2, "%d", i);
		//We add it to the arguments sent to palin
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
	//Deallocate shared memory
	shmdt(shmPtr);
	shmctl(shmid, IPC_RMID, NULL);
        if (msgctl(msgqid, IPC_RMID, NULL) == -1)
                fprintf(stderr, "Message queue could not be deleted\n");
			
	return 0;
}
