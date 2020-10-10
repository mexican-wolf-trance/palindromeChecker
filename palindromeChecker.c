#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define BUFFSIZE 1024
#define SHM_KEY 0x1234

void isPali(char str[])
{
	int l = 0;
	int h = strlen(str) - 1;

	while (h > l)
	{
		if (str[l++] != str[h--])
		{
			printf("%s is not a palindrome\n", str);
			exit(1);
		}
	}
	printf("%s is a palindrome\n", str);
}

int main(int argc, char *argv[])
{
	int shmid, i, j = 0, startingIndex, pid;
	char *shmPtr;
//	FILE *pali, *nonp, *log;

	if (argc > 3)
	{
		fprintf(stderr, "%s: Too few arguments!", argv[0]);
		exit(1);
	}

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

	startingIndex = atoi(argv[1]) * 80;
	pid = atoi(argv[2]);
	printf("We're in the palindrome! Child process no %d\n", pid);
/*	
	if ((pali == fopen("palin.out", "w")) == NULL)
	{
		perror("Palin.out");
		exit(1);
	}
        if ((nonp == fopen("nopalin.out", "w")) == NULL)
        {
                perror("Nopalin.out");
                exit(1);
        }
*/
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

	isPali(temp);

	sleep(2);

//	fclose(pali);
//	fclose(nonp);
}
