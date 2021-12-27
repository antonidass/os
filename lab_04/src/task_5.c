#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

_Bool flag = false;

void check_process_status(int status)
{
    if (WIFEXITED(status))
    {
        printf("Child process completed successfully\n");
        printf("Child process return code: %d\n\n", WEXITSTATUS(status));
        return;
    }

    if (WIFSIGNALED(status))
    {
        printf("Child process terminates with an un-intercepted signal\n");
        printf("Signal number: %d\n\n", WTERMSIG(status));
        return;
    }

    if (WIFSTOPPED(status))
    {
        printf("Child process has stopped\n");
        printf("Signal number: %d\n\n", WSTOPSIG(status));
        return;
    }
}

void catch_signal_ctrl_c(int signal_number)
{
    flag = true;
	printf("\nСaught signal: %d\n", signal_number);
}

int main()
{
    int childpid_first, childpid_second, status;
    pid_t childpid;
    int fd[2];
    char first_text[137] = "LaTeX is a high-quality typesetting system. It includes\nfeatures designed for the production of technical and scientific documentation.";
    char second_text[21] = "Pestilence shall come";
    char text_1[137] = "\0", text_2[21] = "\0";

    signal(SIGINT, catch_signal_ctrl_c);

    printf("Parent: to write child messages in the channel press: CTRL+C\n\n");
    sleep(10);

    if (pipe(fd) == -1)
    {
        perror("Can't pipe\n");
        return 1;
    }

    if ((childpid_first = fork()) == -1)
    {
        perror("Can't fork\n");
        return 1;
    }
    else if (childpid_first == 0)
    {
        if (flag)
        {
            close(fd[0]);

            if ((write(fd[1], first_text, strlen(first_text) + 1)) != strlen(first_text) + 1)
            {
                printf("First child can't write\n");
                exit(1);
            }

            printf("First child message send to parent, message: %s\n", first_text);
        }

        exit(0);
    }

    if ((childpid_second = fork()) == -1)
    {
        perror("Can't fork\n");
        return 1;
    }
    else if (childpid_second == 0)
    {
        if (flag)
        {
            close(fd[0]);

            if ((write(fd[1], second_text, strlen(second_text) + 1)) != strlen(second_text) + 1)
            {
                printf("Second child can't write\n");
                exit(1);
            }

            printf("Second child message send to parent, message: %s\n", second_text);
        }

        exit(0);
    }

    if (childpid_first && childpid_second)
    {
        close(fd[1]);

        read(fd[0], text_1, strlen(first_text) + 1);
		read(fd[0], text_2, strlen(second_text) + 1);

        printf("\nText: %s", text_1);
        printf("\nText: %s\n\n", text_2);

        childpid = wait(&status);
        printf("Process status: %d, Child pid: %d\n", status, childpid);
        check_process_status(status);

        childpid = wait(&status);
        printf("Process status: %d, Child pid: %d\n", status, childpid);
        check_process_status(status);

        printf("Parent: id: %d pgroup: %d first child: %d second child: %d\n", getpid(), getpgrp(), childpid_first, childpid_second);
    }
}