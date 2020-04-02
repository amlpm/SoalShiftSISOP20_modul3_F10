#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8080

typedef struct Player_s {
	int socket;
	int health;
	struct Player_s * otherPlayer;
}Player_t;

typedef struct Node_s{
	struct Node_s * next;
	Player_t * value;
}Node_t;

typedef struct Queue_s{
	Node_t * head;
	Node_t * tail;
	int count;
}Queue_t;

typedef struct Akun_s{
	char username[20];
	char password[20];
}Akun_t;

typedef struct AkunData_s{
	int count;
	Akun_t * akun[20];
}AkunData_t;

typedef struct ServerData_s{
	int isRunning;
	int server_fd;
	struct sockaddr_in address;
	AkunData_t * akunData;
	int addrlen;
	Queue_t * queue;
}ServerData_t;

typedef struct ThreadParam_s{
	Player_t * player;
	ServerData_t * serverData;
}ThreadParam_t;

void enqueue(Queue_t * queue, Player_t * player){
	Node_t * newNode = malloc(sizeof(Node_t));
	newNode->value = player;
	if(queue->count == 0){
		queue->head = newNode;
	}else{
		queue->tail->next = newNode;
	}
	queue->tail = newNode;
	queue->count++;
}

Player_t * dequeue(Queue_t * queue){
	if(queue->count == 0)return NULL;
	Node_t * node = queue->head;
	queue->head = node->next;
	queue->count--;
	free(node);
	return node->value;
}

void * playerThread(void * param);
void * pairThread(void * param);
void * listenThread(void * param);

void addAkun(AkunData_t * akunData, const char * user, const char * pass);
Akun_t * createAkun(const char * user, const char * pass);
void sendString(Player_t *, const char *);
int receiveString(Player_t *, char *, size_t);

int main(int argc, char const *argv[]) {
	ServerData_t * serverData = malloc(sizeof(ServerData_t));
	memset(serverData, 0, sizeof(serverData));

	AkunData_t * akunData = malloc(sizeof(AkunData_t));
	memset(akunData, 0, sizeof(akunData));

	char user[20];
	char pass[20];
	FILE * akunFile = fopen("akun.txt", "r");
	int count = 0;
	while(fscanf(akunFile, "%s%s", user, pass) != EOF){
		Akun_t * akun = createAkun(user, pass);
		akunData->akun[count++] = akun;
	}
	akunData->count = count;
	serverData->akunData = akunData;

	fclose(akunFile);

	int * server_fd = &serverData->server_fd;
	struct sockaddr_in * address = &serverData->address;
	int * addrlen = &serverData->addrlen;
	*addrlen = sizeof(*address);
    int opt = 1;

    if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons( PORT );
      
    if (bind(*server_fd, (struct sockaddr *)address, *addrlen)<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

	Queue_t * que = malloc(sizeof(Queue_t));
	memset(que, 0, sizeof(Queue_t));
	serverData->queue = que;
	int * isRunning = &serverData->isRunning;
	*isRunning = 1;

	pthread_t pair;
	if(pthread_create(&pair, NULL, &pairThread, (void *)serverData) < 0){
		printf("failed to create thread\n");
		exit(EXIT_FAILURE);
	}

	pthread_t listen;
	if(pthread_create(&listen, NULL, &listenThread, serverData) < 0){
		printf("failed to create thread\n");
		exit(EXIT_FAILURE);
	}

	pthread_join(pair, NULL);
	pthread_join(listen, NULL);

    return 0;
}

Akun_t * createAkun(const char * user, const char * pass){
	Akun_t * akun = malloc(sizeof(Akun_t));
	strcpy(akun->username, user);
	strcpy(akun->password, pass);
	return akun;
}

void addAkun(AkunData_t * akunData, const char * user, const char * pass){
	FILE * akunFile = fopen("akun.txt", "a");
	fprintf(akunFile, "%s\n%s\n", user, pass);
	fclose(akunFile);
	int count = akunData->count;
	Akun_t * akun = createAkun(user, pass);
	akunData->akun[count++] = akun;
	akunData->count = count;
}

void sendString(Player_t * connectionData, const char * str){
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	strncpy(buffer, str, sizeof(buffer));
	if(send(connectionData->socket, buffer, sizeof(buffer), 0) < 0){
		printf("Can't send string\n");
		exit(EXIT_FAILURE);
	}
}

int receiveString(Player_t * connectionData, char * dest, size_t size){
	char buffer[1024];
	memset(buffer, 0, sizeof(0));
	int val = read(connectionData->socket, buffer, sizeof(buffer));
	if(val < 0){
		printf("failed to receive string\n");
		exit(EXIT_FAILURE);
	}
	strncpy(dest, buffer, size);
	return val;
}

void * playerThread(void * param){

	ThreadParam_t * p = (ThreadParam_t *)param;
	ServerData_t * serverData = p->serverData;
	Player_t * player = p->player;

    char buffer[1024];

	while(serverData->isRunning){
		int val = receiveString(player, buffer, sizeof(buffer));
		if(val == 0)break;
		if(strcmp(buffer, "register") == 0){
			char username[20];
			char password[20];
			receiveString(player, username, sizeof(username));
			receiveString(player, password, sizeof(password));
			addAkun(serverData->akunData, username, password);
		}else if(strcmp(buffer, "login") == 0){
			char username[20];
			char password[20];
			receiveString(player, username, sizeof(username));
			receiveString(player, password, sizeof(password));
			int login = 0;
			for(int i = 0; i < serverData->akunData->count; i++){
				if(strcmp(username, serverData->akunData->akun[i]->username) == 0 && strcmp(password, serverData->akunData->akun[i]->password) == 0){
					login = 1;
					break;
				}
			}
			if(login){
				sendString(player, "success");
				printf("Auth success\n");
				fflush(stdout);
			}else{
				sendString(player, "failed");
				printf("Auth failed\n");
				fflush(stdout);
			}
		}else if(strcmp(buffer, "find") == 0){
			enqueue(serverData->queue, player);
			while(player->otherPlayer == NULL){
				sleep(1);
			}
			sendString(player, "player found");
		}else if(strcmp(buffer, "fire") == 0){
			player->otherPlayer->health -= 10;
			char buffer[10];
			snprintf(buffer, sizeof(buffer), "%d", player->otherPlayer->health);
			sendString(player->otherPlayer, buffer);
			if(player->otherPlayer->health == 0){
				break;
			}
		}
	}
	char * win = "win";
	if(player->health != 0){
		sendString(player, win);
	}

}

void * pairThread(void * param){
	ServerData_t * serverData = (ServerData_t *)param;
	while(serverData->isRunning){
		if(serverData->queue->count >= 2){
			Player_t * a = dequeue(serverData->queue);
			Player_t * b = dequeue(serverData->queue);
			a->otherPlayer = b;
			b->otherPlayer = a;
		}
	}
}

void * listenThread(void * param){
	ServerData_t * serverData = (ServerData_t*)param;
	int * server_fd = &serverData->server_fd;
	struct sockaddr_in * address = &serverData->address;
	int * addrlen = &serverData->addrlen;
	*addrlen = sizeof(*address);
	pthread_t t;

	while(serverData->isRunning){

		if (listen(*server_fd, 3) < 0) {
			perror("listen");
			exit(EXIT_FAILURE);
		}

		ThreadParam_t * param = malloc(sizeof(ThreadParam_t));
		param->serverData = serverData;

		Player_t * player = malloc(sizeof(Player_t));
		memset(player, 0, sizeof(player));
		param->player = player;
		player->health = 100;

		if ((player->socket = accept(*server_fd, (struct sockaddr *)address, (socklen_t*)addrlen))<0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}

		if(pthread_create(&t, NULL, &playerThread, (void *)param) < 0){
			perror("thread fail");
			exit(EXIT_FAILURE);
		}
	}
}