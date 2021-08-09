#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#define SIZE_USERS 20
int flagLogin;
struct message_buffer
{
    long msgtyp;
    long source;
    char arg[5];
    char txt[100];
};

int main(int argc, char const *argv[])
{   
    flagLogin = 0;
    struct message_buffer message;
    key_t main_key;
    main_key = ftok("progfile", 65);
    int id_fila = msgget(main_key, 0666 | IPC_CREAT);

    printf("Digite o usuário para se logar:");
    char username[20];
    scanf("%s",username);
    
    message.msgtyp = 1;
    strcpy(message.txt,username);
    strcpy(message.arg,"login");
    msgsnd(id_fila, &message, sizeof(message), 0);
    msgrcv(id_fila, &message, sizeof(message), 1, 0);
    printf("%s",message.txt);
    return 0;
}
