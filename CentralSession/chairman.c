#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include<string.h>  
#define SIZE_USERS 20

struct message_buffer
{
    long msgtyp;
    char arg[5];
    char txt[100];
};

struct user
{
    long pid;
    char user[20]; 
};

void handle_msgs(struct message_buffer msg){
    if(strcmp(msg.arg,"login")==0){
        for (int i = 0; i < SIZE_USERS; i++)
        {
            
        }
        
    }
}


int main(int argc, char const *argv[])
{   
    struct message_buffer message;
    struct user users[SIZE_USERS];
    key_t main_key;
    main_key = ftok("progfile", 65);
    int id_fila = msgget(main_key, 0666 | IPC_CREAT);
    
    while (1)
    {
        msgrcv(id_fila, &message, sizeof(message), 1, 0);
        handle_msgs(message);
    }
    
    
    return 0;
}
