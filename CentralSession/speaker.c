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
char myuser[21];
struct message_buffer
{
    long msgtyp;
    pid_t source;
    char arg[10];
    char txt[500];
} message;

struct req_message_buffer
{
    long msgtyp;
    pid_t source;
    char arg[10];
    char txt[100];
    char dest[100];
};

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
        struct req_message_buffer login_msg;
        struct message_buffer resp_msg;
        //Message Reset
        login_msg.source = getpid();
        login_msg.msgtyp = 1;
        strcpy(login_msg.arg, "login");

        printf("Digite o usuário para se logar:\n");
        scanf("%s", login_msg.txt);
        strcpy(myuser, login_msg.txt);
        printf("\nCaptured[arg][txt]: %s-%s\n", login_msg.arg, login_msg.txt);
        if (strlen(login_msg.txt) <= SIZE_USERS)
        {
            printf("\nEnviado:[arg][txt][source]: %s-%s-%d\n", login_msg.arg, login_msg.txt, login_msg.source);
            msgsnd(msg_id, &login_msg, sizeof(login_msg), 0);
            msgrcv(msg_id, &resp_msg, sizeof(resp_msg), login_msg.source, 0);
            printf("\nRecebido:[type][arg][txt][source]: %ld-%s-%s-%d\n", resp_msg.msgtyp, resp_msg.arg, resp_msg.txt, resp_msg.source);
            if (strcmp(resp_msg.arg, "ok") == 0)
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
    msgrcv(msg_id, &logout_msg, sizeof(logout_msg), logout_msg.source, 0);
    printf("\nRecebido:[type][arg][txt][source]: %ld-%s-%s-%d\n", logout_msg.msgtyp, logout_msg.arg, logout_msg.txt, logout_msg.source);
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
        // printf("Splitting string \"%s\" into tokens:\n", str);

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

        // printf("Args global:\n");
        // printf("ARGS CAPTURED: %s|%s|%s\n", current_args[0], current_args[1], current_args[2]);

        if (strcmp(current_args[0], "send") == 0)
        {
            struct req_message_buffer send_msg;
            send_msg.source = getpid();
            send_msg.msgtyp = 1;
            strcpy(send_msg.arg, current_args[0]);
            strcpy(send_msg.dest, current_args[1]);
            strcpy(send_msg.txt, "");
            for (int j = 2; j < i; j++)
            {
                strcat(send_msg.txt, current_args[j]);
                if (j < i - 1)
                {
                    strcat(send_msg.txt, " ");
                }
            }
            printf("SEND MSG ARG: %s\n", send_msg.arg);
            printf("SEND MSG TXT: %s\n", send_msg.txt);
            printf("SEND MSG DEST: %s\n", send_msg.dest);
            msgsnd(msg_id, &send_msg, sizeof(send_msg), 0);
        }
        if (strcmp(current_args[0], "msgs") == 0)
        {
            struct req_message_buffer send_msg;
            struct message_buffer resp_msg;
            send_msg.source = getpid();
            send_msg.msgtyp = 1;
            strcpy(send_msg.arg, current_args[0]);
            strcpy(send_msg.dest, "chairman");
            strcpy(send_msg.txt, "");
            msgsnd(msg_id, &send_msg, sizeof(send_msg), 0);
            msgrcv(msg_id, &resp_msg, sizeof(resp_msg), getpid(), 0);
            printf("\nRecebido:[type][arg][txt][source]: %ld-%s-%s-%d\n", resp_msg.msgtyp, resp_msg.arg, resp_msg.txt, resp_msg.source);
        }
        if (strcmp(current_args[0], "users") == 0)
        {
            struct req_message_buffer send_msg;
            struct message_buffer resp_msg;
            send_msg.source = getpid();
            send_msg.msgtyp = 1;
            strcpy(send_msg.arg, current_args[0]);
            strcpy(send_msg.dest, "chairman");
            strcpy(send_msg.txt, "");
            msgsnd(msg_id, &send_msg, sizeof(send_msg), 0);
            msgrcv(msg_id, &resp_msg, sizeof(resp_msg), getpid(), 0);
            printf("\nUsuários: %s", resp_msg.txt);
        }
        if (strcmp(current_args[0], "myid") == 0)
        {
            printf("My PID: %d\nMy Username: %s", getpid(), myuser);
        }
        if (strcmp(current_args[0], "post") == 0)
        {
            struct req_message_buffer send_msg;
            struct message_buffer resp_msg;
            send_msg.source = getpid();
            send_msg.msgtyp = 1;
            strcpy(send_msg.arg, current_args[0]);
            // strcpy(send_msg.dest, current_args[1]);
            strcpy(send_msg.txt, "");
            for (int j = 1; j < i; j++)
            {
                strcat(send_msg.txt, current_args[j]);
                if (j < i - 1)
                {
                    strcat(send_msg.txt, " ");
                }
            }
            printf("SEND MSG ARG: %s\n", send_msg.arg);
            printf("SEND MSG TXT: %s\n", send_msg.txt);
            // printf("SEND MSG DEST: %s\n",send_msg.dest);
            msgsnd(msg_id, &send_msg, sizeof(send_msg), 0);
            msgrcv(msg_id, &resp_msg, sizeof(resp_msg), getpid(), 0);
            printf("\nRecebido:[type][arg][txt][source]: %ld-%s-%s-%d\n", resp_msg.msgtyp, resp_msg.arg, resp_msg.txt, resp_msg.source);
        }
        if (strcmp(current_args[0], "show") == 0)
        {
            struct req_message_buffer send_msg;
            struct message_buffer resp_msg;
            send_msg.source = getpid();
            send_msg.msgtyp = 1;
            strcpy(send_msg.arg, current_args[0]);
            msgsnd(msg_id, &send_msg, sizeof(send_msg), 0);
            msgrcv(msg_id, &resp_msg, sizeof(resp_msg), getpid(), 0);
            printf("\nPosts:\n%s", resp_msg.txt);
        }
    }

    // msgsnd(msg_id, &message, sizeof(message), 0);
    // msgrcv(msg_id, &message, sizeof(message), 1, 0);
    // printf("\nRecebido[arg][txt]: %s-%s\n", message.arg, message.txt);
    return 0;
}
