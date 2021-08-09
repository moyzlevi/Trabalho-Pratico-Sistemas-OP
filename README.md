## UFCSPA  Informática Biomédica Sistemas Operacionais 2021/1 Prof. João Gluz

### TRABALHO PRÁTICO: Programação Concorrente

Sistema CentralSession
------
O sistema CentralSession é um sistema para comunicação e troca de mensagens entre usuários que opera de
forma centralizada e é composto de dois tipos de processos:

* Processos clientes, denominados de **_speaker_**, que implementam a interface de uso do sistema pelos usuários.
* Processo servidor, denominado de **_chairman_**, que implementa e controla os serviços de comunicação entre
  os clientes.
* Os comandos são reformatados em variaveis pelo **_speaker_** e passados como mensagem para o **_chairman_**
* Toda interação entre **_speaker_** e **_chairman_** ocorre entre fila única de mensagens compartilhada por todos os processos, o **_chairmain_** deve enviar uma mensagem de feedback

Requisitos |  Necessários
--- | --- 
Login | O usuário precisa se logar com 20 caractares seu nome (sem senha), verificar se já existe o mesmo nome 
Chairman IPC fila de mensagens| O processo *chairman* irá utilizar uma fila de mensagens, código identificador *msgtyp*, código1 para servidor e PID para clientes

### Lista de comando a serem enviados do cliente para serem tratadas no servidor(todas as mensagens devem conter o PID do speaker da requisição)
* **_send_** _<nome-usuário>_ _<texto-mensagem>_ - envia mensagem de texto ao servidor
* **_msgs_** - lista as mensagens recebidas pelo cliente
* **_post_** _<texto-mensagem>_ - publica uma mensagem publica no forum de mensagens
* **_show_** - mostra todas as mensagens que foram publicadas no forum de mensagens
* **_del_ _msgs_** - deleta todas as mensagens destinadas ao usuario
* **_del_ _post_** _<número>_ - deleta a mensagem com índice _<número>_ do fórum
* **_del_ _posts_** - remove todas as mensagens do fórum
* **_users_** - mostra uma lista com todos os usuários do sistema
* **_myid_** - mostra o nome e identificador (PID) do usuário atual
* **_exit_** - sai do sistema, mas antes desloga

Sistema PeerTalk
------
O sistema PeerTalk também é um sistema para comunicação e troca de mensagens entre usuários. Mas diferente do CentralSession, 
o PeerTalk opera de forma descentralizada não tendo a necessidade de um processo central de controle como o processo chairman.
* Sistema de mensagens sem cliente-servidor, apenas cliente-cliente (_peer-to-peer_)
* A lista de usuários logados no sistema deve ser guardada em uma área de memória compartilhada por
  todos os processos peertalker. **_Área Crítica de memória compartilhada_**
Requisitos |  Necessários
--- | --- 
Login | O usuário precisa se logar com 20 caractares seu nome (sem senha), verificar se já existe o mesmo nome na memória compartilhada usando _mutexes_
Cadastro na lista| Depois de feita a verificação o PID e o nome do processo serão registrados na lista, liberar _mutex_
Mensagens| o primeiro campo **_msgtyp_** irá destinar para o processo correto (deverá conter o PID destino), outro campo deverá conter o PID remetente

* **_send_** _<nome-usuário>_ _<texto-mensagem>_ - consulta a lista de usuários, se existir envia uma mensagem para a fila geral do sistema com o PID destinatário e o texto, 
  o remetente deve estar logado, caso o contrário será mostrado erro
* **_msgs_** - retira da fila de mensagens do sistema todos as que tem o PID destinatário à ele, se não houverem mensagem, avisar
* **_del_ _msgs_** - apenas deleta as mensagens guardadas no processo local, não deleta a fila de mensagens global
* **_users_** - mostra todos os usuários do sistema
* **_myid_** - mostra o identificador do usuário atual
* **_exit_** - sai do sistema e desloga da lista

Sistema ThreadTheMatrix
------
Implemente um programa que faça multiplicação de matrizes quadradas e que utilize _threads_ para acelerar o processo. Faça outro que não utilize e compare-os.
Faça a partição da matriz C atribuindo uma linha para cada _thread_
