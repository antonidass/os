#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

void check_status(int status);

int main()
{
    printf("PARENT: PID = %d, GRP = %d\n\n", getpid(), getpgrp());
    int childpid1, childpid2;

    if ((childpid1 = fork()) == 1)
    {
        perror("Not forked child 1!\n");
        return 1;
    }
    else if (childpid1 == 0) 
    {
        printf("CHILD_1 : PID = %d, GRP = %d PPID = %d\n", getpid(), getpgrp(), getppid());
        sleep(5);
        return 0;
    }

    if ((childpid2 = fork()) == 1)
    {
        perror("Not forked child 2!\n");
        return 1;
    } 
    else if (childpid2 == 0) 
    {
        printf("CHILD_2 : PID = %d, GRP = %d PPID = %d\n", getpid(), getpgrp(), getppid());
        sleep(7);
        return 0;
    }

    int status;
    pid_t childpid;

    childpid = wait(&status);
    printf("STATUS = %d,  CHILD_PID = %d\n", status, childpid);
    check_status(status);

    childpid = wait(&status);
    printf("STATUS = %d,  CHILD_PID = %d\n", status, childpid);
    check_status(status);

    printf("Parent: id: %d pgrp: %d\n", getpid(), getpgrp());
    printf("Parent died!\n\n");

    return 0;
}


void check_status(int status)
{
	if (WIFEXITED(status))
	{
		printf("Процесс - потомок завершен нормально.\n");
		printf("Код завершения процесса - потомка %d.\n\n", WEXITSTATUS(status));
		return;
	}

	if (WIFSIGNALED(status))
	{
		printf("Дочерний процесс завершается неперехватываемым сигналом.\n");
		printf("Номер сигнала %d.\n", WTERMSIG(status));
		return;
	}

	if (WIFSTOPPED(status))
	{
		printf("Дочерний процесс остановился.\n");
		printf("Номер сигнала %d.", WSTOPSIG(status));
	}
}