#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

typedef struct WildPokemon_s{
	int isLocked;
	char name[20];
	int type;
	int price;
	int capRateA;
	int capRateB;
	int escapeRateA;
	int escapeRateB;
}WildPokemon_t;

typedef struct Shop_s{
	int isLocked;
	int lullabyPowder;
	int lullabyPowderPrice;
	int pokeball;
	int pokeballPrice;
	int berry;
	int berryPrice;
}Shop_t;

typedef struct GameData_s{
	int isRunning;
	WildPokemon_t * sharedPokemon;
	Shop_t * sharedShop;
}GameData_t;

void * randomizePokemonThread(void * param);
void * shopThread(void * param);
int generateRandom(int max);

int main(int argc, const char * argv[]){
	srand(time(0));

	GameData_t * gameData = malloc(sizeof(GameData_t));
	memset(gameData, 0, sizeof(GameData_t));
	gameData->isRunning = 1;

	//shared memory
	key_t key = 1111;
	WildPokemon_t * sharedPokemon;
	int shmid =  shmget(key, sizeof(WildPokemon_t), IPC_CREAT | 0666);
	sharedPokemon = shmat(shmid, NULL, 0);
	memset(sharedPokemon, 0, sizeof(sharedPokemon));

	gameData->sharedPokemon = sharedPokemon;

	key_t key2 = 2222;
	Shop_t * sharedShop;
	int shmid2 =  shmget(key2, sizeof(Shop_t), IPC_CREAT | 0666);
	sharedShop = shmat(shmid2, NULL, 0);
	memset(sharedShop, 0, sizeof(sharedShop));

	gameData->sharedShop = sharedShop;
	
	pthread_t pokemonThread;

	if(pthread_create(&pokemonThread, NULL, &randomizePokemonThread, (void*)gameData ) < 0){
		printf("can't create thread\n");
		exit(EXIT_FAILURE);
	}

	pthread_t shop;

	if(pthread_create(&shop, NULL, &shopThread, (void*)gameData ) < 0){
		printf("can't create thread\n");
		exit(EXIT_FAILURE);
	}
	printf("--POKEZONE--\n");
	printf("1. Exit\n");
	printf("Choice : ");
	int choice;
	scanf("%d", &choice);
	pid_t ppp = getpid();
	if(choice == 1){
		pid_t id = fork();
		if(id){
			char pid[10];
			snprintf(pid, sizeof(pid), "%d", ppp);
			char * argv[] = {"kill", "-9", pid, NULL};
			execv("/bin/kill", argv);
		}
	}

	pthread_join(pokemonThread, NULL);
	pthread_join(shop, NULL);

	shmdt(sharedPokemon);
    shmctl(shmid, IPC_RMID, NULL);

	shmdt(sharedShop);
    shmctl(shmid2, IPC_RMID, NULL);
	return 0;
}

int generateRandom(int max){
	return rand()%max;
}

void * randomizePokemonThread(void * param){
	GameData_t * gameData = (GameData_t *)param;
	WildPokemon_t * sharedPokemon = gameData->sharedPokemon;
	char normalName[5][20];
	strcpy(normalName[0], "Bulbasaur");
	strcpy(normalName[1], "Charmander");
	strcpy(normalName[2], "Squirtle");
	strcpy(normalName[3], "Rattata");
	strcpy(normalName[4], "Caterpie");

	char rareName[5][20];
	strcpy(rareName[0], "Pikachu");
	strcpy(rareName[1], "Eevee");
	strcpy(rareName[2], "Jigglypuff");
	strcpy(rareName[3], "Snorlax");
	strcpy(rareName[4], "Dragonite");

	char legendaryName[5][20];
	strcpy(legendaryName[0], "Mew");
	strcpy(legendaryName[1], "Mewtwo");
	strcpy(legendaryName[2], "Moltres");
	strcpy(legendaryName[3], "Zapdos");
	strcpy(legendaryName[4], "Articuno");

	while(gameData->isRunning){
		if(sharedPokemon->isLocked){
			continue;
		}
		int random = generateRandom(20);
		if(random <= 15){
			//normal
			random = generateRandom(5);
			strcpy(sharedPokemon->name, normalName[random]);
			sharedPokemon->price = 80;
			sharedPokemon->capRateA = 7;
			sharedPokemon->capRateB = 10;
			sharedPokemon->escapeRateA = 1;
			sharedPokemon->escapeRateB = 20;
			sharedPokemon->type = 0;
		}else if(random <= 18){
			//rare
			random = generateRandom(5);
			strcpy(sharedPokemon->name, rareName[random]);
			sharedPokemon->price = 100;
			sharedPokemon->capRateA = 5;
			sharedPokemon->capRateB = 10;
			sharedPokemon->escapeRateA = 1;
			sharedPokemon->escapeRateB = 10;
			sharedPokemon->type = 1;
		}else{
			//legendary
			random = generateRandom(5);
			strcpy(sharedPokemon->name, legendaryName[random]);
			sharedPokemon->price = 200;
			sharedPokemon->capRateA = 3;
			sharedPokemon->capRateB = 10;
			sharedPokemon->escapeRateA = 1;
			sharedPokemon->escapeRateB = 5;
			sharedPokemon->type = 2;
		}
		sleep(1);
	}
}

void * shopThread(void * param){
	GameData_t * gameData = (GameData_t *)param;
	gameData->sharedShop->lullabyPowder = 100;
	gameData->sharedShop->lullabyPowderPrice = 60;
	gameData->sharedShop->berry = 100;
	gameData->sharedShop->berryPrice = 5;
	gameData->sharedShop->pokeball = 100;
	gameData->sharedShop->pokeballPrice = 15;
	fflush(stdout);
	while(gameData->isRunning){
		sleep(10);
		gameData->sharedShop->berry += 10;
		if(gameData->sharedShop->berry > 200){
			gameData->sharedShop->berry = 200;
		}
		gameData->sharedShop->pokeball += 10;
		if(gameData->sharedShop->pokeball > 200){
			gameData->sharedShop->pokeball = 200;
		}
		gameData->sharedShop->lullabyPowder += 10;
		if(gameData->sharedShop->lullabyPowder > 200){
			gameData->sharedShop->lullabyPowder = 200;
		}
		fflush(stdout);
	}
}