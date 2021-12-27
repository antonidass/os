#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>


_Bool flag = false;

void check_status(int status);


void catch_sig(int sig_numb)
{
	flag = true;
	printf("catch_sig: %d\n", sig_numb);
}


int main()
{
	signal(SIGINT, catch_sig);

	int childpid_1, childpid_2;
	int fd[2];
	char text1[57] = "\0", text2[44] = "\0";
	char first_text[57] = "Tyger Tyger, burning bright, In the forests of the night";
    char second_text[44] = "The forest paths are muddy, after the rain.";
	pid_t childpid;
	int status;

	printf("Parent: press CTRL+C if you want to see messages from childs\n\n");
	sleep(5);

	if (pipe(fd) == -1)
	{
		perror("Cant pipe.\n");
		return 1;
	}

	if ((childpid_1 = fork()) == -1)
	{
		perror("Not forked.\n");
		return 1;
	}
	else if (!childpid_1)
	{
		if (flag) {
			close(fd[0]);
			write(fd[1], first_text, strlen(first_text) + 1);

			printf("First child message send to parent, message: %s\n", first_text);
		}

		return 0;
	}

	if ((childpid_2 = fork()) == -1)
	{
		perror("Cant fork.\n");
		return 1;
	}
	else if (!childpid_2) 
	{
		if (flag) {
			close(fd[0]);
			write(fd[1], second_text, strlen(second_text) + 1);

			printf("Second child message send to parent, message: %s\n", second_text);
		}

		return 0;
	}

	if (childpid_1 && childpid_2)
	{
		close(fd[1]);

		read(fd[0], text1, strlen(first_text) + 1);
		read(fd[0], text2, strlen(second_text) + 1);

		printf("\nText: %s\n", text1);
		printf("Text: %s\n\n", text2);
	}
 
	childpid = wait(&status);
	check_status(status);

	childpid = wait(&status);
	check_status(status);

	printf("Parent: id: %d pgrp: %d child1: %d child2: %d\n", getpid(), getpgrp(), childpid_1, childpid_2);
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