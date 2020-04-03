#include <stdio.h>
#include <termios.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#define PORT 8080
#define MAIN_MENU 0
#define REGISTER_MENU 1
#define LOGIN_MENU 2
#define LOBBY_MENU 3
#define GAME_MENU 4

static struct termios old, new;
 
void initTermios(int echo)  {
  tcgetattr(0, &old);
  new = old;
  new.c_lflag &= ~ICANON;
  new.c_lflag &= echo ? ECHO : ~ECHO;
  tcsetattr(0, TCSANOW, &new);
}
 
void resetTermios(void) 
{
  tcsetattr(0, TCSANOW, &old);
}

char getch(){
	char ch;
  	initTermios(0);
	ch = getchar();
	resetTermios();
	return ch;
}


typedef struct GameData_s{
	int gameState;
	int isRunning;
}GameData_t;

typedef struct MatchData_s{
	int win;
	int dead;
	int playerFound;
}MatchData_t;

typedef struct ConnectionData_s{
	int connectedSocket;
    struct sockaddr_in serverAddress;
}ConnectionData_t;

typedef struct ThreadParam_s{
	GameData_t * gameData;
	MatchData_t * matchData;
	ConnectionData_t * connectionData;
}ThreadParam_t;

ConnectionData_t * connectToServer();
void disconnectFromServer(ConnectionData_t * data);
void * listenThread(void * param);
void * gameThread();
void changeState(int * state, int nextState);
void mainMenu(GameData_t *);
void registerMenu(GameData_t *, ConnectionData_t *);
void loginMenu(GameData_t *, ConnectionData_t *);
void lobbyMenu(GameData_t *, ConnectionData_t *);
void gameMenu(GameData_t *, ConnectionData_t *);
void sendString(ConnectionData_t *, const char *);
int receiveString(ConnectionData_t *, char *, size_t size);

int main(int argc, char const *argv[]) {
    pthread_t game;
	if(pthread_create(&game, NULL, &gameThread, NULL) < 0){
		printf("failed to create thread\n");
		exit(EXIT_FAILURE);
	}
	pthread_join(game, NULL);
    return 0;
}

ConnectionData_t * connectToServer(){
	ConnectionData_t * data = malloc(sizeof(ConnectionData_t));
    int  * sock = &data->connectedSocket;
    struct sockaddr_in * serv_addr = &data->serverAddress;
    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        exit(EXIT_FAILURE);
    }
  
    memset(serv_addr, '0', sizeof(struct sockaddr_in));
  
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr->sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
		exit(EXIT_FAILURE);
    }
  
    if (connect(*sock, (struct sockaddr *)serv_addr, sizeof(struct sockaddr_in)) < 0) {
        printf("\nConnection Failed \n");
		exit(EXIT_FAILURE);
    }
	return data;
}

void disconnectFromServer(ConnectionData_t * data){
	if(close(data->connectedSocket) < 0){
		printf("Error can't close socket\n");
		exit(EXIT_FAILURE);
	}
}

void sendString(ConnectionData_t * connectionData, const char * str){
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	strncpy(buffer, str, sizeof(buffer));
	if(send(connectionData->connectedSocket, buffer, sizeof(buffer), 0) < 0){
		printf("Can't send string\n");
		exit(EXIT_FAILURE);
	}
}

int receiveString(ConnectionData_t * connectionData, char * dest, size_t size){
	char buffer[1024];
	memset(buffer, 0, sizeof(0));
	int val = read(connectionData->connectedSocket, buffer, sizeof(buffer));
	if(val < 0){
		printf("failed to receive string\n");
		exit(EXIT_FAILURE);
	}
	strncpy(dest, buffer, size);
	return val;
}

void * listenThread(void * param){
    char buffer[1024];
	ThreadParam_t * parameter = (ThreadParam_t *) param;
	int * dead = &parameter->matchData->dead;
	int * win = &parameter->matchData->win;
	int * playerFound = &parameter->matchData->playerFound;
	while(!*dead && !*win){
		receiveString(parameter->connectionData, buffer, sizeof(buffer));
		if(strcmp(buffer, "player found") == 0){
			*playerFound = 1;
		}else if(strcmp(buffer, "win") == 0){
			*win = 1;
		}else{
			int health = atoi(buffer);
			if(health == 0){
				*dead = 1;
			}else{
				printf("Health : %d\n", health);
			}
		}
	}
}

void changeState(int * state, int nextState){
	*state = nextState;
}

void * gameThread(){
	GameData_t * gameData = malloc(sizeof(GameData_t));
	memset(gameData, 0, sizeof(gameData));
	ConnectionData_t * connectionData = connectToServer();
	int * gameState = &gameData->gameState;
	int * isRunning = &gameData->isRunning;
	*isRunning = 1;
	while(*isRunning){
		switch (*gameState){
			case MAIN_MENU :
				mainMenu(gameData);
			break;
			case REGISTER_MENU :
				registerMenu(gameData, connectionData);
			break;
			case LOGIN_MENU :
				loginMenu(gameData, connectionData);
			break;
			case LOBBY_MENU :
				lobbyMenu(gameData, connectionData);
			break;
			case GAME_MENU :
				gameMenu(gameData, connectionData);
			break;
		}
	}
	disconnectFromServer(connectionData);
}

void mainMenu(GameData_t * data){
	printf("1. Login\n");
	printf("2. Register\n");
	printf("Choices : ");
	char input[20];
	scanf("%s", input);
	if(strcmp(input, "login") == 0){
		changeState(&data->gameState, LOGIN_MENU);
	}else if(strcmp(input, "register") == 0){
		changeState(&data->gameState, REGISTER_MENU);
	}
}

void registerMenu(GameData_t * gameData, ConnectionData_t * connectionData){
	char username[20];
	char password[20];
	printf("Username : ");
	scanf("%s", username);
	printf("Password : ");
	scanf("%s", password);
	sendString(connectionData, "register");
	sendString(connectionData, username);
	sendString(connectionData, password);
	char message[10];
	printf("register success\n");
	changeState(&gameData->gameState, MAIN_MENU);
}

void loginMenu(GameData_t * gameData, ConnectionData_t * connectionData){
	char username[20];
	char password[20];
	printf("Username : ");
	scanf("%s", username);
	printf("Password : ");
	scanf("%s", password);
	sendString(connectionData, "login");
	sendString(connectionData, username);
	sendString(connectionData, password);
	char message[50];
	receiveString(connectionData, message, sizeof(message));
	if(strcmp(message, "success") != 0){
		printf("login failed\n");
		changeState(&gameData->gameState, MAIN_MENU);
	}else{
		printf("login success\n");
		changeState(&gameData->gameState, LOBBY_MENU);
	}
}

void lobbyMenu(GameData_t * gameData, ConnectionData_t * connectionData){
	printf("1. Find Match\n");
	printf("2. Logout\n");
	printf("Choices : ");
	char input[20];
	scanf("%s", input);
	if(strcmp("find", input) == 0){
		sendString(connectionData, "find");
		changeState(&gameData->gameState, GAME_MENU);
	}else if(strcmp("logout", input) == 0){
		changeState(&gameData->gameState, MAIN_MENU);
	}
}

void gameMenu(GameData_t * gameData, ConnectionData_t * connectionData){
	MatchData_t * matchData = malloc(sizeof(MatchData_t));
	memset(matchData, 0, sizeof(MatchData_t));
	ThreadParam_t * threadParam = malloc(sizeof(ThreadParam_t));
	threadParam->connectionData = connectionData;
	threadParam->gameData = gameData;
	threadParam->matchData = matchData;
	int * playerFound = &matchData->playerFound;
	int * dead = &matchData->dead;
	int * win = &matchData->win;

	pthread_t listen_t;
	if(pthread_create(&listen_t, NULL, listenThread, (void *)threadParam)){
        printf("\nThread Failed \n");
        exit(EXIT_FAILURE);
	}

	while(!*playerFound){
		printf("Waiting for player ...\n");
		sleep(1);
	}

	printf("Game dimulai silahkan tap tap secepat mungkin !!\n");

	char * fire = "fire";
	while(!*dead && !*win){
		char input = getch();
		if(input == ' '){
			sendString(connectionData, fire);
			printf("hit!!\n");
		}
	}

	pthread_join(listen_t, NULL);
	
	if(*dead){
		printf("Game berakhir kamu kalah\n");
	}else if(*win){
		printf("Game berakhir kamu menang\n");
	}

	changeState(&gameData->gameState, LOBBY_MENU);
}