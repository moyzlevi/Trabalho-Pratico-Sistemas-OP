#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define SIZE_USERS 20
int flagLogin;

struct message_buffer
{
    long msgtyp;
    pid_t source;
    char arg[10];
    char txt[100];
};

int main(int argc, char const *argv[])
{
    flagLogin = 0;
    struct message_buffer message;
    key_t my_key;
    int msg_id;
    my_key = ftok("progfile", 65);
    msg_id = msgget(my_key, 0666 | IPC_CREAT);

    printf("=================Programa Speaker=========PID:%d=======\n",getpid());
    printf("Digite o usuário para se logar:\n");
    strcpy(message.arg,"login");
    scanf("%s", message.txt);  
    printf("\nCaptured[arg][txt]: %s-%s\n", message.arg,message.txt);
    message.source = getpid();
    message.msgtyp = 1;

    msgsnd(msg_id, &message, sizeof(message), 0);
    
    msgrcv(msg_id, &message, sizeof(message), 1, 0);
    printf("\nRecebido[arg][txt]: %s-%s\n", message.arg,message.txt);
    return 0;
}
