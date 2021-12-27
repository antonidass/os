#include <stdio.h>
#include <unistd.h>



int main()
{
    printf("PARENT: PID = %d, GRP = %d\n", getpid(), getpgrp());
    int childpid1, childpid2;

    if ((childpid1 = fork()) == 1)
    {
        perror("Not forked child 1!\n");
        return 1;
    } 
    else if (childpid1 == 0) 
    {
        printf("lock: PID = %d, GRP = %d PPID = %d\n", getpid(), getpgrp(), getppid());
        sleep(5);
        printf("unlock: PID = %d, GRP = %d PPID = %d\n", getpid(), getpgrp(), getppid());
        return 0;
    }

    if ((childpid2 = fork()) == 1)
    {
        perror("Not forked child 2!\n");
        return 1;
    } 
    else if (childpid2 == 0) 
    {
        printf("lock: PID = %d, GRP = %d PPID = %d\n", getpid(), getpgrp(), getppid());
        sleep(10);
        printf("unlock: PID = %d, GRP = %d PPID = %d\n", getpid(), getpgrp(), getppid());
        return 0;
    }

    printf("Parent: id: %d pgrp: %d child1: %d child2: %d\n", getpid(), getpgrp(), childpid1, childpid2);
    printf("Parent died!\n\n");
    return 0;
}