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
			printf("%s is not a palindrome", str);
			return 1;
		}
	}
	printf("%s is a palindrome", str);
}

int main()
{
	int shmid, i;
	char *shmPtr;
	FILE *pali, *nonp, *log;

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

	for (i = 0;, i < strlen(shmPtr)
		isPali(shmPtr[i]);

	printf("We're in the palindrome!\n");
	printf("Palindrome shared memory:\n%s\n", shmPtr);
	sleep(2);

	fclose(pali);
	fclose(nonp);
}
