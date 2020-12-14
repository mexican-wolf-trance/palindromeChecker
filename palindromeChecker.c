#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define BUFFSIZE 1024
#define SHM_KEY 0x12345
#define MSG_KEY 0x98765

struct msgbuf
{
        long mtype;
        int msg;
} message;


int isPali(char str[])
{
	int l = 0;
	int h = strlen(str) - 1;

	while (h > l)
	{
		if (str[l++] != str[h--])
		{
			printf("%s is not a palindrome\n", str);
			return 0;
		}
	}
	printf("%s is a palindrome\n", str);
	return 1;
}

int main(int argc, char *argv[])
{
	int shmid, msgqid, i, j = 0, startingIndex, pid;
	char *shmPtr;
	FILE *pali, *nonp;

	if ((shmid = shmget(SHM_KEY, sizeof(shmPtr), 0644)) < 0)
	{
		perror("Shmget failed");
		exit(1);
	}

	if ((shmPtr = shmat(shmid, NULL, 0)) == (char *) -1)
	{
		perror("Shmat failed");
		exit(1);
	}

        msgqid = msgget(MSG_KEY, 0644 | IPC_CREAT);
        if (msgqid == -1)
        {
                perror("msgqid get failed");
                return 1;
        }


	startingIndex = atoi(argv[1]) * 80;
	pid = getpid();
	printf("We're in the palindrome! Child process no %d\n", pid);
	
	if ((pali = fopen("palin.out", "a")) == NULL)
	{
		perror("Palin.out");
		exit(1);
	}
        if ((nonp = fopen("nopalin.out", "a")) == NULL)
        {
                perror("Nopalin.out");
                exit(1);
        }

	char temp[80];
	for (i = startingIndex; i < (startingIndex + 80); i++)
	{
		if (shmPtr[i] == '\0')
		{
			temp[j] = shmPtr[i];
			break;
		}
		temp[j] = shmPtr[i];
		j++;
	}

	if(msgrcv(msgqid, &message, sizeof(message), 1, 0) > 0)
	{
		sleep(2);
		if(isPali(temp))
			fprintf(pali,"%li %d %s %s", (long) pid, startingIndex, temp, "is a palindrome!\n");
		else
			fprintf(nonp,"%li %d %s %s", (long) pid, startingIndex, temp, "is not a palindrome...\n");
		message.mtype = 1;
		message.msg = 1;
		msgsnd(msgqid, &message, sizeof(message), 0);
	}

	fclose(pali);
	fclose(nonp);
}
