#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#define SIZE_USERS 20
#define SIZE_MSGS 100

struct message_buffer
{
    long msgtyp;
    pid_t source;
    char arg[10];
    char txt[500];
};
struct req_message_buffer
{
    long msgtyp;
    pid_t source;
    char arg[10];
    char txt[100];
    char dest[100];
};

struct user
{
    long pid;
    char username[20];
};
struct user users[SIZE_USERS];
int lastUserIndex;
int lastMsgIndex;
int lastPostIndex;

struct msg_record
{
    char source[20];
    char content[100];
    char dest[20];
};
struct msg_record msgs[SIZE_MSGS];
struct msg_record forum[SIZE_MSGS];

struct message_buffer handle_msgs(struct req_message_buffer msg)
{
    if (strcmp(msg.arg, "login") == 0)
    {
        int flagFound = 0;
        for (int i = 0; i < SIZE_USERS; i++)
        {
            char usr[20];
            strcpy(usr, users[i].username);
            // printf("\n#%dBuscado: %s\n",i,usr); --debug
            if (strcmp(msg.txt, usr) == 0)
            {
                struct message_buffer resp_msg;
                resp_msg.msgtyp = msg.source;
                strcpy(resp_msg.txt, "Usuário já existente");
                strcpy(resp_msg.arg, "fail");
                return resp_msg;
            }
        }
        //copia pid recebido e nome de usuario para memoria
        strcpy(users[lastUserIndex].username, msg.txt);
        users[lastUserIndex].pid = msg.source;
        lastUserIndex++;
        //retorna mensagem de sucesso
        struct message_buffer resp_msg;
        resp_msg.msgtyp = msg.source;
        resp_msg.source = getpid();
        strcpy(resp_msg.txt, "Login foi efetuado com sucesso");
        strcpy(resp_msg.arg, "ok");
        return resp_msg;
    }
    else if (strcmp(msg.arg, "logout") == 0)
    {
        //Algoritmo para deletar um array de structs

        for (int i = 0; i < SIZE_USERS; i++)
        {
            if (msg.source == users[i].pid)
            {
                printf("\n%dBuscado: %ld\n", i, users[i].pid);
                int actual = i;
                for (i = actual; actual < SIZE_USERS - 1; actual++)
                {
                    users[actual] = users[actual + 1];
                }
                struct message_buffer resp_msg;
                resp_msg.msgtyp = msg.source;
                strcpy(resp_msg.txt, "Logout Efetuado com Sucesso");
                strcpy(resp_msg.arg, "ok");
                return resp_msg;
            }
        }
    }
    else if (strcmp(msg.arg, "send") == 0)
    {
        int flagFound = 0;
        for (int i = 0; i < SIZE_USERS; i++)
        {
            if (strcmp(msg.dest, users[i].username) == 0)
            {
                flagFound = 1;
                strcpy(msgs[lastMsgIndex].content, msg.txt);
                strcpy(msgs[lastMsgIndex].dest, msg.dest);
                for (int j = 0; j < SIZE_USERS; j++)
                {
                    if (msg.source == users[j].pid)
                    {
                        strcpy(msgs[lastMsgIndex].source, users[j].username);
                    }
                }
            }
        }

        struct message_buffer resp_msg;
        resp_msg.msgtyp = msg.source;
        resp_msg.source = getpid();
        if (flagFound)
        {
            printf("Mensagem guardada como[dest][source][txt]: %s-%s-%s", msgs[lastMsgIndex].dest,
                   msgs[lastMsgIndex].source, msgs[lastMsgIndex].content);
            lastMsgIndex++;
            strcpy(resp_msg.txt, "mensagem recebida com sucesso");
            strcpy(resp_msg.arg, "ok");
        }
        else
        {
            strcpy(resp_msg.txt, "Dest Not Found");
            strcpy(resp_msg.arg, "fail");
        }

        return resp_msg;
    }
    else if (strcmp(msg.arg, "msgs") == 0)
    {
        char currentUsername[20];
        for (int i = 0; i < SIZE_USERS; i++)
        {
            if (users[i].pid == msg.source)
            {
                strcpy(currentUsername, users[i].username);
            }
        }

        for (int i = 0; i < SIZE_MSGS; i++)
        {
            if (strcmp(msgs[i].dest, currentUsername) == 0)
            {
                printf("\n%s said: %s\n", msgs[i].source, msgs[i].content);
            }
        }

        struct message_buffer resp_msg;
        resp_msg.msgtyp = msg.source;
        resp_msg.source = getpid();
        strcpy(resp_msg.txt, "mensagens mostradas");
        strcpy(resp_msg.arg, "ok");
        return resp_msg;
    }
    else if (strcmp(msg.arg, "users") == 0)
    {
        struct message_buffer resp_msg;
        resp_msg.msgtyp = msg.source;
        resp_msg.source = getpid();
        strcpy(resp_msg.arg, "ok");
        strcpy(resp_msg.txt, "");
        for (int i = 0; i < lastUserIndex; i++)
        {

            strcat(resp_msg.txt, users[i].username);
            if (i < lastUserIndex - 1)
            {
                strcat(resp_msg.txt, ",");
            }
        }
        return resp_msg;
    }
    else if (strcmp(msg.arg, "post") == 0)
    {
        char currentUsername[20];
        for (int i = 0; i < SIZE_USERS; i++)
        {
            if (users[i].pid == msg.source)
            {
                strcpy(currentUsername, users[i].username);
            }
        }
        strcpy(forum[lastPostIndex].content, msg.txt);
        strcpy(forum[lastPostIndex].source, currentUsername);
        printf("Mensagem guardada como [source][txt]: %s-%s", forum[lastPostIndex].source,
               forum[lastPostIndex].content);
        lastPostIndex++;
        struct message_buffer resp_msg;
        resp_msg.msgtyp = msg.source;
        resp_msg.source = getpid();
        strcpy(resp_msg.arg, "ok");
        strcpy(resp_msg.txt, "adicionado no forum");
        return resp_msg;
    }
    else if (strcmp(msg.arg, "show") == 0)
    {
        struct message_buffer resp_msg;
        resp_msg.msgtyp = msg.source;
        resp_msg.source = getpid();
        strcpy(resp_msg.arg, "ok");
        strcpy(resp_msg.txt, "");

        for (int i = 0; i < lastPostIndex; i++)
        {
            char str[5];
            sprintf(str, "#%d ", i);
            strcat(resp_msg.txt, str);
            strcat(resp_msg.txt, forum[i].source);
            strcat(resp_msg.txt, ": ");
            strcat(resp_msg.txt, forum[i].content);

            if (i < lastUserIndex - 1)
            {
                strcat(resp_msg.txt, "\n");
            }
        }
        return resp_msg;
    }
    else if (strcmp(msg.arg, "dmsgs") == 0)
    {
        char currentUsername[20];
        for (int i = 0; i < SIZE_USERS; i++)
        {
            if (users[i].pid == msg.source)
            {
                strcpy(currentUsername, users[i].username);
            }
        }

        for (int i = 0; i < SIZE_MSGS; i++)
        {
            if (strcmp(currentUsername, msgs[i].dest) == 0)
            {
                printf("\n%dBuscado: %ld\n", i, users[i].pid);
                int actual = i;
                for (i = actual; actual < SIZE_USERS - 1; actual++)
                {
                    users[actual] = users[actual + 1];
                }
                struct message_buffer resp_msg;
                resp_msg.msgtyp = msg.source;
                strcpy(resp_msg.txt, "Mensagens Deletadas");
                strcpy(resp_msg.arg, "ok");
                return resp_msg;
            }
        }
    }
    else if (strcmp(msg.arg, "dpost") == 0)
    {
        int post_i = atoi(msg.txt);
        for (int i = post_i; i <SIZE_MSGS  - 1; i++)
        {
            forum[i] = forum[i + 1];
        }
        struct message_buffer resp_msg;
        resp_msg.msgtyp = msg.source;
        strcpy(resp_msg.txt, "Post Deletado com Sucesso");
        strcpy(resp_msg.arg, "ok");
        lastPostIndex--;
        return resp_msg;
    }
}

int main(int argc, char const *argv[])
{
    lastUserIndex = 0;
    lastMsgIndex = 0;
    struct message_buffer message;
    struct req_message_buffer rq_message;
    key_t my_key;
    int msg_id;
    my_key = ftok("progfile", 65);
    msg_id = msgget(my_key, 0666 | IPC_CREAT);
    printf("===========Programa ChairMan===PID:%d======\nAguardando Mensagens...\n", getpid());
    while (1)
    {
        msgrcv(msg_id, &rq_message, sizeof(rq_message), 1, 0);
        printf("\nMensagem recebida[arg][txt][source][dest]: %ld-%s-%s-%d-%s\n", rq_message.msgtyp, rq_message.arg, rq_message.txt, rq_message.source, rq_message.dest);
        message = handle_msgs(rq_message);
        printf("\nResp_Message after func[type][arg][txt][source]: \n%ld-%s-%s-%d\n", message.msgtyp, message.arg, message.txt, message.source);
        printf("----------------------------------------------------");
        msgsnd(msg_id, &message, sizeof(message), 0);
    }

    return 0;
}
