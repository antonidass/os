#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

void check_status(int status);

int main()
{
	printf("PARENT: PID = %d, GRP = %d\n\n", getpid(), getpgrp());
	int childpid1, childpid2;
	int status;
	pid_t childpid;


	if ((childpid1 = fork()) == 1)
	{
		perror("Not forked child 1!\n");
		return 1;
	} 
	else if (childpid1 == 0) 
	{
		printf("CHILD_1 : PID = %d, GRP = %d PPID = %d\n\n", getpid(), getpgrp(), getppid());
		
        if (execl("merge_sort.out", "5", "19", "12", "14", "11", "42", NULL) == -1)
		{
			perror("CHILD_1 cant exec");
			return 1;
		}

		return 0;
	}

	if ((childpid2 = fork()) == 1)
	{
		perror("Not forked child 2!\n");
		return 1;
	} 
	else if (childpid2 == 0) 
	{
		printf("CHILD_2 : PID = %d, GRP = %d PPID = %d\n\n", getpid(), getpgrp(), getppid());

        if (execl("newton_interpolation.out", "func.txt", "0.4", "4", NULL) == -1)
		{
			perror("CHILD_2 cant exec");
			return 1;
		}

		return 0;
	}

	childpid = wait(&status);
	check_status(status);

	childpid = wait(&status);
	check_status(status);

	printf("Parent: id: %d pgrp: %d child1: %d child2: %d\n", getpid(), getpgrp(), childpid1, childpid2);
	printf("Parent died!\n\n");

	return 0;
}


void check_status(int status)
{
	if (WIFEXITED(status))
	{
		printf("The child process is completed normally.\n");
		printf("Child process termination code %d.\n\n", WEXITSTATUS(status));
		return;
	}

	if (WIFSIGNALED(status))
	{
		printf("The child process terminates with an un-intercepted signal.\n");
		printf("Signal number %d.\n", WTERMSIG(status));
		return;
	}

	if (WIFSTOPPED(status))
	{
		printf("The child process has stopped.\n");
		printf("Signal number %d.", WSTOPSIG(status));
	}
}