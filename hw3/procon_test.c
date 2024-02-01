#include "types.h"
#include "stat.h"
#include "user.h"

void *producer(void*);
void *consumer(void*);
int mutex, empty, full;
char buffer[1];
int pro_left = 10000; 
int con_left = 10000;
int p1_result = 0;
int p2_result = 0;
int c1_result = 0;
int c2_result = 0;


int main(void)
{   
    mutex = sem_create(1);
    empty = sem_create(1);
    full = sem_create(0);


    int p1 = hufs_thread_create(&producer, 0);
    int p2 = hufs_thread_create(&producer, 0);
    int c1 = hufs_thread_create(&consumer, 0);
    int c2 = hufs_thread_create(&consumer, 0);

    hufs_thread_join(p1);
    hufs_thread_join(p2);
    hufs_thread_join(c1);
    hufs_thread_join(c2);

    printf(1,"producer(1): %d produced\n", p1_result);
    printf(1,"producer(2): %d produced\n", p2_result);
    printf(1,"consumer(1): %d consumed\n", c1_result);
    printf(1,"consumer(2): %d consumed\n", c2_result);

    exit();
}

void *producer(void *v)
{
    int pro_counter=0;

    while(1){
        if(pro_left <= 1) break;

        sem_wait(empty);
        sem_wait(mutex);
        buffer[0] = 1;
        pro_counter++;
        pro_left --;
        sem_signal(mutex);
        sem_signal(full);
    }

    // 먼저 끝나면 p1 이후는 p2
    sem_wait(mutex);
    if(p1_result == 0)   p1_result = pro_counter;
    else    p2_result = pro_counter;
    sem_signal(mutex);

    exit();
}

void *consumer(void *v)
{
    int con_counter=0;

    while(1){
        if(con_left <= 1) break;

        sem_wait(full);
        sem_wait(mutex);
        buffer[0] = 0;
        con_counter++;
        con_left--;
        sem_signal(mutex);
        sem_signal(empty);
    }

    // 먼저 끝나면 c1 이후는 c2
    sem_wait(mutex);
    if(c1_result == 0)   c1_result = con_counter;
    else    c2_result = con_counter;
    sem_signal(mutex);

    exit();
}
