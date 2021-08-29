#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 1024 /* make it a 1K shared memory segment */

int main(int argc, char const *argv[])
{
    key_t key;
    int shmid;
    char *data;
    int mode;

    if ((key = ftok("data.txt", 'R')) == -1) /*Here the file must exist */
    {
        perror("ftok");
        exit(1);
    }

    if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1)
    {
        perror("shmget");
        exit(1);
    }

    data = shmat(shmid, NULL, 0);
    if (data == (char *)(-1))
    {
        perror("shmat");
        exit(1);
    }

        // printf("writing to segment: \"%s\"\n", "jose");
        // strncpy(data, "jose", SHM_SIZE);

    
        printf("segment contains: \"%s\"\n", data);
    
    // while (1)
    // {
        
    // }
    

    // if (shmdt(data) == -1)
    // {
    //     perror("shmdt");
    //     exit(1);
    // }

    return 0;
}
