#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define SIZE_USERS 20

struct message_buffer
{
    long msgtyp;
    pid_t source;
    char arg[10];
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
            char usr[20];
            strcpy(usr,users[i].username);
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
        
        strcpy(resp_msg.txt, "Login foi efetuado com sucesso");
        strcpy(resp_msg.arg, "ok");
        return resp_msg;
    }
}

int main(int argc, char const *argv[])
{
    lastUserIndex = 0;
    struct message_buffer message;
    key_t my_key;
    int msg_id;
    my_key = ftok("progfile", 65);
    msg_id = msgget(my_key, 0666 | IPC_CREAT);
    printf("===========Programa ChairMan=========\nAguardando Mensagens...\n");
    while (1)
    {
        msgrcv(msg_id, &message, sizeof(message), 1, 0);
        printf("\nMensagem recebida[arg][txt][source]: %s-%s-%d\n",message.arg,message.txt,message.source);
        struct message_buffer resp_message = handle_msgs(message);
        printf("\nResp_Message after func[arg][txt][source]: \n%s-%s-%d\n",resp_message.arg,resp_message.txt,message.source);
        printf("----------------------------------------------------");
        resp_message.msgtyp = 1;
        msgsnd(msg_id, &resp_message, sizeof(resp_message), 0);
       
    }

    return 0;
}
