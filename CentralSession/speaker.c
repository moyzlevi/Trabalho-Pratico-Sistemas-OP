#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define SIZE_USERS 20

int flagLogin;
key_t my_key;
int msg_id;
int flagLogin;
struct message_buffer
{
    long msgtyp;
    pid_t source;
    char arg[10];
    char txt[100];
} message;


void login()
{   
    while (1)
    {   
        fflush(stdin);
        struct message_buffer login_msg;
        //Message Reset
        login_msg.source = getpid();
        login_msg.msgtyp = 1;
        strcpy(login_msg.arg, "login");

        printf("Digite o usuário para se logar:\n");
        scanf("%s", login_msg.txt);
        printf("\nCaptured[arg][txt]: %s-%s\n", login_msg.arg, login_msg.txt);
        if (strlen(login_msg.txt) <= SIZE_USERS)
        {   
            printf("\nEnviado:[arg][txt][source]: %s-%s-%d\n", login_msg.arg, login_msg.txt,login_msg.source);
            msgsnd(msg_id, &login_msg, sizeof(login_msg), 0);
            msgrcv(msg_id, &login_msg, sizeof(login_msg), 1, 0);
            printf("\nEnviado:[arg][txt][source]: %s-%s-%d\n", login_msg.arg, login_msg.txt,login_msg.source);
            if (strcmp(login_msg.arg, "ok") == 0)
            {
                flagLogin = 1;
                break;
            }
            else
            {
                continue;
            }
        }
        else
        {
            printf("\nNome de usuário deve ter menos que 20 caracteres!\n");
            continue;
        }
    }
}

int main(int argc, char const *argv[])
{
    flagLogin = 0;
    my_key = ftok("progfile", 65);
    msg_id = msgget(my_key, 0666 | IPC_CREAT);
    char erros[50];
    printf("=================Programa Speaker=========PID:%d=======\n", getpid());
    login();
    // msgsnd(msg_id, &message, sizeof(message), 0);
    // msgrcv(msg_id, &message, sizeof(message), 1, 0);
    // printf("\nRecebido[arg][txt]: %s-%s\n", message.arg, message.txt);
    return 0;
}
