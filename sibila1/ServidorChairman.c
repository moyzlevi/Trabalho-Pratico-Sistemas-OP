#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <ctype.h>

#define MQ_REQRESP_KEY	10020

#define LOGIN_REQ		0 
#define LOGOUT_REQ		1
#define SEND			2
#define MSGS			3
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
#define MAX_MSGS 		100
#define MAXSIZE_MSGDATA 	200

// Estrutura de requisição de mensagens
struct request_msg {
 	long	server_id;
 	long	client_id;
   	int	request;
   	long 	index;
	char	requestdata[MAXSIZE_MSGDATA+1];
	char    destinatario[MAXSIZE_USERNAME+1];
};

// Estrutura de resposta de mensagens
struct response_msg {
	long	client_id;
	int	    response;
	char	responsedata[MAXSIZE_MSGDATA+1];
};

// Estrutura para guardar usuários
struct user_record 
{
	long id;
	char name[MAXSIZE_USERNAME+1];
};

// Estrutura para guardar as mensagens
struct msg_record
{
	long id;
	char content[MAXSIZE_MSGDATA+1];
	char destinatario[MAXSIZE_USERNAME+1];
};

// Define as listas de usuários e mensagens
struct user_record user_table[MAX_USERS+1];
struct msg_record msg_table[MAX_MSGS+1];


// Rotinas

// Inicializa o uso das tabelas
int initialize_user_table()
{
	int i;
	for (i=0; i < MAX_USERS; i++) {
		user_table[i].id = -1;
		strcpy(user_table[i].name,"");
	}
	return -1;
}

int initialize_msg_table()
{
	int i;
	for (i=0; i < MAX_MSGS; i++) {
		msg_table[i].id = -1;
		strcpy(msg_table[i].content,"");
	}
	return -1;
}

// Adiciona usuário na tabela
int add_user_to_table(char *name, long *id)
{
	int i;
	for (i=0; i<MAX_USERS; i++) {
		if (user_table[i].id <= 0) {
			strncpy(user_table[i].name, name, MAXSIZE_USERNAME);
			user_table[i].id = id;
			return 0;
		}
	}
	return -1;
}

// Busca id do usuário
long get_user_id(char *name)
{
	int i;
	for (i=0; i<MAX_USERS; i++) {
		if (user_table[i].id > 0) {
			if (strncmp(user_table[i].name, name, MAXSIZE_USERNAME)==0) {
				return user_table[i].id;
			}
		}
	}
	return -1;
}

// Deleta usuário na tabela
int del_user_from_table(char *name)
{
	int i;
	for (i=0; i<MAX_USERS; i++) {
		if (strncmp(user_table[i].name, name, MAXSIZE_USERNAME)==0) {
			user_table[i].id = -1;
			strcpy(user_table[i].name,"");
			return 0;
		}
	}
	return -1;
}

// Indice das mensagens

// Rotina para enviar mensagem para usuarios
void send_msg_users(long *id, char *nome, char *content){
	long index = 0;
	msg_table[index].id = id;
	strcpy(msg_table[index].destinatario, nome);
	strcpy(msg_table[index].content, content);

	index++;

	printf("\nMensagem enviada!\n");

	//return 0;

}

// Rotina para listar mensagens de um usuario
void get_msgs_user(long *id){

	char nome[MAXSIZE_USERNAME+1];

	int i;
	for(i = 0; i < MAX_USERS; i++){
		if(*id == user_table[i].id){
			strncpy(nome, user_table[i].name, MAXSIZE_USERNAME);
			break;
		}
	}

	printf("\n**********************************\n");
	printf("      Mensagens para %s\n", nome);

	for(i = 0; i < MAX_MSGS; i++){
		if(strcmp(nome, msg_table[i].destinatario) == 0){
			printf("%s\n", msg_table[i].content);
		}
	}

	printf("\n**********************************\n");

}

// Rotina para listar os usuarios
void print_users(){

	printf("\n*****************************");
	printf("\n  Lista de usuários logados  ");

	int i;
	for(i = 0; i < MAX_USERS; i++){
		if(user_table[i].id != -1){
			printf("\n%s, id %ld", user_table[i].name, user_table[i].id);
		}
	}

	printf("\n*****************************\n");

}

void main()
{
	int mqid;
	struct request_msg req_msg;
	struct response_msg resp_msg;
	int stop_server;
	
	// Faz as inicialização da fila de mensagem
	mqid = msgget(MQ_REQRESP_KEY, IPC_CREAT | 0777);
	if (mqid == -1) {
   		printf("msgget() falhou\n"); exit(1); 
	} 
	printf("servidor: iniciou execucao\n");
	
	// Inicializa a tabela de usuarios
	initialize_user_table();
	initialize_msg_table();
	
	stop_server = 0;

	// Executa laco de atendimento de requisicoes dos clientes
	while (!stop_server) {
		printf("\n======================================================================\n");
		// Espera pelas requisicoes dos cliente (message type=SERVER ID)
		if (msgrcv(mqid,&req_msg,sizeof(struct request_msg)-sizeof(long),SERVER_ID,0)<0) {
			printf("msgrcv() falhou\n"); 
			exit(1);
		}
		
		printf("\nservidor: recebeu requisicao %d do cliente %ld\n",req_msg.request,req_msg.client_id);

		// Atende a requisicao
		switch(req_msg.request) {
			// Caso padrao (inicial) para todos os talkers iniciados
			case LOGIN_REQ: 
				//Busca na tabela de usuarios
				if (get_user_id(req_msg.requestdata)>0) {
					printf("\nservidor: usuario %s ja' esta' logado\n",req_msg.requestdata);
					resp_msg.response = ALREADY_LOG_RESP;
					strcpy(resp_msg.responsedata,"ERROR: user already logged\n");
				} else if (add_user_to_table(req_msg.requestdata,req_msg.client_id)==0) {
					printf("\nservidor: usuario %s aceito e logado\n",req_msg.requestdata);
					resp_msg.response = OK_RESP;
					strcpy(resp_msg.responsedata,"OK: user logged in\n");
				} else {
					printf("\nservidor: usuario %s aceito mas nao foi possivel logar\n",req_msg.requestdata);
					resp_msg.response = CANNOT_LOG_RESP;
					strcpy(resp_msg.responsedata,"ERROR: cannot log user\n");
				}	
				resp_msg.client_id = req_msg.client_id;
				break;

			case SEND:
				send_msg_users(&req_msg.client_id, &req_msg.destinatario, &req_msg.requestdata);
				resp_msg.response = OK_RESP;
				resp_msg.client_id = req_msg.client_id;
				strncpy(resp_msg.responsedata,req_msg.requestdata,MAXSIZE_MSGDATA);
				break;

			case MSGS:
				get_msgs_user(&req_msg.client_id);
				resp_msg.response = OK_RESP;
				resp_msg.client_id = req_msg.client_id;
				strncpy(resp_msg.responsedata,req_msg.requestdata,MAXSIZE_MSGDATA);
				break;

			case USERS:
				print_users();
				resp_msg.response = OK_RESP;
				break;
	
			case LOGOUT_REQ:
				if (del_user_from_table(req_msg.requestdata)==0) {
					printf("servidor: usuario %s deslogado\n",req_msg.requestdata);
					resp_msg.response = OK_RESP;
					strcpy(resp_msg.responsedata,"OK: user logged out\n");
				} else {
					printf("servidor: nao foi possivel deslogar usuario %s\n",req_msg.requestdata);
					resp_msg.response = ERROR_RESP;
					strcpy(resp_msg.responsedata,"ERROR: cannot log user out\n");
				}	
				resp_msg.client_id = req_msg.client_id;
				break;

			case STOP_SERVER_REQ:
				// Para o servidor
				stop_server = 1;
				// Prepara a mensagem de resposta
				resp_msg.response = OK_RESP;
				resp_msg.client_id = req_msg.client_id;
				strcpy(resp_msg.responsedata,"Stopping server\n");
				break;

			default:
				// Requisicao invalida, apenas prepara a mensagem de resposta
				resp_msg.response = INVALID_REQ_RESP;
				resp_msg.client_id = req_msg.client_id;
				strcpy(resp_msg.responsedata,"Invalid request\n");
				break;										
		}
		
		// Envia a resposta ao cliente
		msgsnd(mqid,&resp_msg,sizeof(struct response_msg)-sizeof(long),0);
		printf("servidor: enviou resposta %d ao cliente %ld\n",resp_msg.response,resp_msg.client_id);
	}

	// Deleta a fila de mensagens, sem deixar requisicoes pendentes
	msgctl(mqid,IPC_RMID,NULL);
	
	exit(0);	
}
