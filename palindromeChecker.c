#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
	printf("We're in the palindrome!\n");
	printf("Palindrome shared memory:\n%s\n", shmPtr);
	sleep(5);
}
