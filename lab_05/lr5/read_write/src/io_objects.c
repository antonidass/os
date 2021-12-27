#include "io_objects.h"


struct sembuf START_READ[] = {
    {WAITING_READERS, 1, 0},
    {ACTIVE_WRITERS, 0, 0},
    {WAITING_WRITERS, 0, 0},
    {ACTIVE_READERS, 1, 0},
    {WAITING_READERS, -1, 0},
};

struct sembuf STOP_READ[] = {
    {ACTIVE_READERS, -1, 0},
};

struct sembuf START_WRITE[] = {
    {WAITING_WRITERS, 1, 0},
    {ACTIVE_READERS, 0, 0},
    {ACTIVE_WRITERS, 0, 0},
    {ACTIVE_WRITERS, 1, 0},
    {WAITING_WRITERS, -1, 0},
};

struct sembuf STOP_WRITE[] = {
    {ACTIVE_WRITERS, -1, 0},
};


int start_read(int s_id)
{
    return semop(s_id, START_READ, 5) != -1;
}

int stop_read(int s_id)
{
    return semop(s_id, STOP_READ, 1) != -1;
}

int start_write(int s_id)
{
    return semop(s_id, START_WRITE, 5) != -1;
}

int stop_write(int s_id)
{
    return semop(s_id, STOP_WRITE, 1) != -1;
}


int run_reader(int* const shared_counter, const int sid, const int reader_id)
{
    if (!shared_counter)
        return -1;

    srand(time(NULL) + reader_id);

    int sleep_time;
    for (int i = 0; i < ITERATIONS_AMOUNT; i++) {
        sleep_time = rand() % TIME_RANGE + TIME_FROM;
        sleep(sleep_time);

        if (!start_read(sid)) {
            perror("Something went wrong with start_read!");
            exit(READER_LOCK_FAILED);
        }

        int val = *shared_counter;
        printf(" \e[1;31mReader #%d read:  %5d\e[0m (slept %ds)\n", reader_id, val, sleep_time);
        
        if (!stop_read(sid)) {
            perror("Something went wrong with stop_read!");
            exit(READER_RELEASE_FAILED);
        }
    }
    return 0;
}

int run_writer(int* const shared_counter, const int sid, const int writer_id)
{
    if (!shared_counter)
        return -1;

    srand(time(NULL) + writer_id + READERS_AMOUNT);

    int sleep_time;
    for (int i = 0; i < ITERATIONS_AMOUNT; i++) {
        sleep_time = rand() % TIME_RANGE + TIME_FROM;
        sleep(sleep_time);

        if (!start_write(sid)) {
            perror("Something went wrong with start_write!");
            exit(WRITER_LOCK_FAILED);
        }

        int val = ++(*shared_counter);
        printf(" \e[1;32mWriter #%d write: %5d\e[0m (slept %ds)\n", writer_id, val, sleep_time);

        if (!stop_write(sid)) {
            perror("Something went wrong with stop_write!");
            exit(WRITER_RELEASE_FAILED);
        }
    }
    return 0;
}
