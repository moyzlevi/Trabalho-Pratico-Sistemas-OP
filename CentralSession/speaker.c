#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#define SIZE_USERS 20
#define MAX_ARGS 5

int flagLogin;
key_t my_key;
int msg_id;
int flagLogin;
char *global_args[10];
int args_number;
struct message_buffer
{
    long msgtyp;
    pid_t source;
    char arg[10];
    char txt[100];
} message;
void clear(void)
{
    while (getchar() != '\n')
        ;
}
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
            printf("\nEnviado:[arg][txt][source]: %s-%s-%d\n", login_msg.arg, login_msg.txt, login_msg.source);
            msgsnd(msg_id, &login_msg, sizeof(login_msg), 0);
            msgrcv(msg_id, &login_msg, sizeof(login_msg), 1, 0);
            printf("\nEnviado:[arg][txt][source]: %s-%s-%d\n", login_msg.arg, login_msg.txt, login_msg.source);
            if (strcmp(login_msg.arg, "ok") == 0)
            {
                flagLogin = 1;
                fflush(stdin);
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

void logout()
{
    struct message_buffer logout_msg;
    //Message Reset
    logout_msg.source = getpid();
    logout_msg.msgtyp = 1;
    strcpy(logout_msg.arg, "logout");
    printf("Deslongando...\n");
    msgsnd(msg_id, &logout_msg, sizeof(logout_msg), 0);
    msgrcv(msg_id, &logout_msg, sizeof(logout_msg), 1, 0);
    printf("\nRecebido:[arg][txt][source]: %s-%s-%d\n", logout_msg.arg, logout_msg.txt, logout_msg.source);
    exit(0);
}

int main(int argc, char const *argv[])
{
    flagLogin = 0;
    my_key = ftok("progfile", 65);
    msg_id = msgget(my_key, 0666 | IPC_CREAT);
    char erros[50];
    
    char *current_args[50];
    char str[256];
    
    printf("=================Programa Speaker=========PID:%d=======\n", getpid());
    login();
    while (1)
    {
        // handle_input();
        // printf("%s\n", global_args[0]);
        clear();
        printf("Esperando comandos:\n");
        fgets(str, 256, stdin);
        str[strcspn(str, "\n")] = 0;
        char *rest = str;
        int i = 0;
        printf("Splitting string \"%s\" into tokens:\n", str);

        while ((current_args[i] = strtok_r(rest, " ", &rest)))
        {
            if (strcmp(current_args[i], "exit") == 0)
            {
                logout();
            }
            // printf("TEMP: %s\n",temp);--debug
            // printf("GLOBAL: %s\n",global_args[i]);
            // printf("#%d: %s\n", i, current_args[i++]);
            i++;
        }

        printf("Args global:\n");
        printf("GLOBAL IN FUNC: %s|%s|%s\n", current_args[0], current_args[1], current_args[2]);
    }

    // msgsnd(msg_id, &message, sizeof(message), 0);
    // msgrcv(msg_id, &message, sizeof(message), 1, 0);
    // printf("\nRecebido[arg][txt]: %s-%s\n", message.arg, message.txt);
    return 0;
}
