#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <unistd.h>

#define MQ_REQRESP_KEY	10020

#define LOGIN_REQ		0
#define LOGOUT_REQ		1
#define SEND			2
#define MSGS			3
#define POST			3
#define USERS			4
#define MYID			5
#define STOP_SERVER_REQ	6

#define OK_RESP		0
#define ERROR_RESP		1
#define ALREADY_LOG_RESP	2
#define CANNOT_LOG_RESP	3
#define INVALID_REQ_RESP	4

#define SERVER_ID 		1

#define MAXSIZE_USERNAME 	20
#define MAX_USERS 		100
#define MAXSIZE_MSGDATA 	200

// Estrutura de requisição das mensagens
struct request_msg {
 	long	server_id;
 	long	client_id;
   	int	request;
   	long 	index;
	char	requestdata[MAXSIZE_MSGDATA+1];
	char    destinatario[MAXSIZE_USERNAME+1];
};

// Estrutura de respostas das mensagens
struct response_msg {
	long	client_id;
	int	response;
	char	responsedata[MAXSIZE_MSGDATA+1];
};

// Estrutura de registro dos usuários
struct user_record 
{
	long id;
	char name[MAXSIZE_USERNAME+1];
};

char *safegets(char *buffer, int buffersize)
{
	char *str;
	int slen;
	
	str = fgets(buffer,buffersize,stdin);
	
	if (str!=NULL) {
		slen = strlen(str);
		if (slen>0)
			str[slen-1] = '\0';
	}
	return str;
}

void main()
{
	int mqid;
	struct request_msg req_msg;
	struct response_msg resp_msg;
	struct user_record user_info;
	int opt;
	int stop_client;
	char tmp;
	
	// Reconhece a fila, que existe apenas se o servidor está em execução
	mqid = msgget(MQ_REQRESP_KEY, 0777);
	if (mqid == -1) {
   		printf("msgget() falhou, fila nao existe, servidor nao inicializado\n"); 
   		exit(1); 
	}

	// Faz o login no servidor
	printf("\n===============Sistema CentralSession===============\n");
	printf("Entre com o nome de usuario: \n");
	
	//Leitura do nome do usuário para estrutura usuario
	safegets(user_info.name,MAXSIZE_USERNAME);
	//Passa o id para estrutura usuario
	user_info.id = getpid();	
	
	// Prepara requisicao de login
	req_msg.request = LOGIN_REQ; 
	req_msg.server_id = SERVER_ID; 
	req_msg.client_id = user_info.id;
	strncpy(req_msg.requestdata,user_info.name,MAXSIZE_USERNAME);	
	
	// Envia requisicao ao servidor
	msgsnd(mqid,&req_msg,sizeof(struct request_msg)-sizeof(long),0);
	printf("\nServer_id : %ld\nCliente_id : %ld\nSolicitação : %ld\nDado da solicitação : %s\n", req_msg.server_id, req_msg.client_id, req_msg.request, req_msg.requestdata);
	
	// Espera pela mensagem de resposta, usando o PID do processo cliente como tipo de mensagem
	if (msgrcv(mqid,&resp_msg,sizeof(struct response_msg)-sizeof(long),user_info.id,0) < 0) {
		printf("msgrcv() falhou\n"); 
		exit(1);
	}
	
	// Apresenta o texto da resposta
	printf("%s\n",resp_msg.responsedata);
	
	// Se nao aceitou login, sai do cliente
	if (resp_msg.response!=OK_RESP){
		exit(1);
	}
	
	// Laco de interface com o usuario
	stop_client=0;
	while(!stop_client) {
		printf("Qual opcao?\n");
		printf("   1: logout\n");
		printf("   2: send\n");
		printf("   3: msgs\n");
		printf("   4: users\n");
		printf("   5: myid\n");
		printf("   6: exit\n");
		
		scanf("%d",&opt);
		scanf("%c",&tmp);

		// Prepara campos da estrutura de requisição
		req_msg.server_id = SERVER_ID;
		req_msg.client_id = user_info.id;
		req_msg.index = -1;

		switch(opt){
			case LOGOUT_REQ:				
				// Prepara requisicao de logout
				stop_client = 1;
				req_msg.request=LOGOUT_REQ;
				strncpy(req_msg.requestdata,user_info.name,MAXSIZE_USERNAME);
				// Envia requisicao ao servidor
				msgsnd(mqid,&req_msg,sizeof(struct request_msg)-sizeof(long),0);
				// Espera pela mensagem de resposta
				if (msgrcv(mqid,&resp_msg,sizeof(struct response_msg)-sizeof(long),user_info.id,0) < 0) {
					printf("msgrcv() falhou\n"); 
					exit(1);
				}
				// Apresenta o texto da resposta
				printf("%s\n",resp_msg.responsedata);
				break;
			
			case SEND:
				printf("Entre com o usuário destino: ");
				// Leitura do destinatario
				safegets(req_msg.destinatario,MAXSIZE_USERNAME);
				printf("Entre com a mensagem a ser enviada:\n");
				// Leitura da mensagem
				safegets(req_msg.requestdata,MAXSIZE_MSGDATA);
				req_msg.request = SEND;
				msgsnd(mqid,&req_msg,sizeof(struct request_msg)-sizeof(long),0);
				if (msgrcv(mqid,&resp_msg,sizeof(struct response_msg)-sizeof(long),user_info.id,0) < 0) {
					printf("msgrcv() falhou\n"); 
					exit(1);
				}
				break;

			case MSGS:
				printf("\n\nMensagens recebidas listadas no servidor:\n\n");
				req_msg.request = MSGS;
				msgsnd(mqid,&req_msg,sizeof(struct request_msg)-sizeof(long),0);
				if (msgrcv(mqid,&resp_msg,sizeof(struct response_msg)-sizeof(long),user_info.id,0) < 0) {
					printf("msgrcv() falhou\n"); 
					exit(1);
				}
				printf("%s\n",resp_msg.responsedata);
				break;

			case USERS:
				printf("\n\nUsuários listados no servidor!\n\n");
				req_msg.request = USERS;
				msgsnd(mqid,&req_msg,sizeof(struct request_msg)-sizeof(long),0);
				if (msgrcv(mqid,&resp_msg,sizeof(struct response_msg)-sizeof(long),user_info.id,0) < 0) {
					printf("msgrcv() falhou\n"); 
					exit(1);
				}
				break;

			// MYID não necessita enviar requisição ao servidor
			case MYID:
				printf("\n\nDados do usuário:\n");
				printf("ID: %ld\n", req_msg.client_id);
				printf("Usuário: %s\n\n\n", user_info.name);
				break;

			case STOP_SERVER_REQ:
				stop_client = 1;
				// Prepara requisicao de parada de servidor
				req_msg.request = STOP_SERVER_REQ;
				// Envia requisicao ao servidor
				msgsnd(mqid,&req_msg,sizeof(struct request_msg)-sizeof(long),0);
				// Espera pela mensagem de resposta
				if (msgrcv(mqid,&resp_msg,sizeof(struct response_msg)-sizeof(long),user_info.id,0) < 0) {
					printf("msgrcv() falhou\n"); exit(1);
				}
				// Apresenta o texto da resposta
				printf("%s\n",resp_msg.responsedata);
				break;

			default:
				printf("Opcao invalida\n");
				break;
		}
	}
			
	exit(0);	
}
