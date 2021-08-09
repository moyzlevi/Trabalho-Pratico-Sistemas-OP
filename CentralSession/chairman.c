#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#define SIZE_USERS 20

struct message_buffer
{
    long msgtyp;
    long source;
    char arg[5];
    char txt[100];
};

struct user
{
    long pid;
    char username[20];
};
struct user users[SIZE_USERS];
int lastUserIndex;

struct message_buffer handle_msgs(struct message_buffer msg)
{
    if (strcmp(msg.arg, "login") == 0)
    {
        int flagFound = 0;
        for (int i = 0; i < SIZE_USERS; i++)
        {
            char usr[20] = users[i].username;
            if (strcmp(msg.txt, usr) == 0)
            {
                struct message_buffer resp_msg;
                resp_msg.msgtyp = msg.source;
                strcpy(resp_msg.txt, "Usuário já existente");
                return resp_msg;
            }
        }
        //copia pid recebido e nome de usuario para memoria
        strcpy(users[lastUserIndex].username, msg.arg);
        users[lastUserIndex].pid = msg.source;
        lastUserIndex++;
        //retorna mensagem de sucesso
        struct message_buffer resp_msg;
        resp_msg.msgtyp = msg.source;
        strcpy(resp_msg.txt, "Login foi efetuado com sucesso");
        return resp_msg;
    }
}

int main(int argc, char const *argv[])
{
    lastUserIndex = 0;
    struct message_buffer message;
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
