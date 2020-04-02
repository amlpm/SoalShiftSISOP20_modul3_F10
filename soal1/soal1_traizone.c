#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const int NORMAL_MODE = 0;
const int POKEDEX = 1;
const int SHOP = 2;
const int CAPTURE_MODE = 3;

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

typedef struct Pokemon_s{
	int released;
	char name[20];
	int type;
	int ap;
	int price;
}Pokemon_t;

typedef struct Inventory_s{
	int pokeball;
	int lullabyPowder;
	int berry;
	int pokedollar;
}Inventory_t;

typedef struct PokeBag_s{
	Pokemon_t * pokemon[7];
	int count;
}PokeBag_t;

typedef struct GameData_s{
	int gameState;
	int isRunning;
	int cariPokemon;
	int jumlahLullabyActive;
	Inventory_t * inventory;
	PokeBag_t * pokeBag;
	WildPokemon_t * sharedPokemon;
	Shop_t * sharedShop;
}GameData_t;

typedef struct WildPokemonParam_s{
	GameData_t * gameData;
	int isRunning;
}WildPokemonParam_t;

typedef struct PokemonParam_s{
	Pokemon_t * pokemon;
	GameData_t * gameData;
}PokemonParam_t;

void * cariPokemonThread(void * param);
void * pokemonThread(void * param);
void * wildpokemonThread(void * param);
void * lullabyThread(void * param);
void addApPokemon(Pokemon_t * pokemon, int ap);
int addPokemon(PokeBag_t * pokeBag, Pokemon_t * pokemon);
Pokemon_t * tamePokemon(WildPokemon_t * wildPokemon);
void removePokemon(PokeBag_t * pokeBag, int index);
void changeState(int * state, int nextState);
void normalMode(GameData_t *);
void pokedex(GameData_t *);
void shop(GameData_t *);
void captureMode(GameData_t *);
int generateRandom(int max);

int main(){
	srand(time(0));

	Inventory_t * inventory = malloc(sizeof(Inventory_t));
	memset(inventory, 0, sizeof(Inventory_t));
	inventory->pokeball = 10;

	PokeBag_t * pokeBag = malloc(sizeof(PokeBag_t));
	memset(pokeBag, 0, sizeof(PokeBag_t));

	GameData_t * gameData = malloc(sizeof(GameData_t));
	memset(gameData, 0, sizeof(gameData));
	gameData->pokeBag = pokeBag;
	gameData->inventory = inventory;

	key_t key = 1111;
	WildPokemon_t * sharedPokemon;
	int shmid =  shmget(key, sizeof(WildPokemon_t), IPC_CREAT | 0666);
	sharedPokemon = shmat(shmid, NULL, 0);

	gameData->sharedPokemon = sharedPokemon;

	key_t key2 = 2222;
	Shop_t * sharedShop;
	int shmid2 =  shmget(key2, sizeof(Shop_t), IPC_CREAT | 0666);
	sharedShop = shmat(shmid2, NULL, 0);

	gameData->sharedShop = sharedShop;

	int * isRunning = &gameData->isRunning;
	*isRunning = 1;
	int * gameState = &gameData->gameState;
	while(*isRunning){
		if(*gameState == NORMAL_MODE){
			normalMode(gameData);
		}else if(*gameState == POKEDEX){
			pokedex(gameData);
		}else if(*gameState == SHOP){
			shop(gameData);
		}else if(*gameState == CAPTURE_MODE){
			captureMode(gameData);
		}
	}

	shmdt(sharedPokemon);
    shmctl(shmid, IPC_RMID, NULL);

	shmdt(sharedShop);
    shmctl(shmid2, IPC_RMID, NULL);

	return 0;
}

int generateRandom(int max){
	return rand()%max;
}

void addApPokemon(Pokemon_t * pokemon, int ap){
	pokemon->ap += ap;
}

int addPokemon(PokeBag_t * pokeBag, Pokemon_t * pokemon){
	if(pokeBag->count == 7)return 0;
	pokeBag->pokemon[pokeBag->count++] = pokemon;
	return 1;
}

void removePokemon(PokeBag_t * pokeBag, int index){
	pokeBag->pokemon[index-1]->released = 1;
	free(pokeBag->pokemon[index-1]);
	for(int i = index-1; i < pokeBag->count-1; i++){
		pokeBag->pokemon[i] = pokeBag->pokemon[i];
	}
	pokeBag->pokemon[pokeBag->count--] = NULL;
}

Pokemon_t * tamePokemon(WildPokemon_t * wildPokemon){
	Pokemon_t * newPokemon = malloc(sizeof(Pokemon_t));
	memset(newPokemon, 0, sizeof(newPokemon));
	strcpy(newPokemon->name, wildPokemon->name);
	newPokemon->type = wildPokemon->type;
	newPokemon->ap = 100;
	newPokemon->price = wildPokemon->price;
	return newPokemon;
}

void changeState(int * state, int nextState){
	*state = nextState;
}

void normalMode(GameData_t * gameData){
	printf("--MAIN MENU--\n");
	if(gameData->cariPokemon){
		printf("1. Berhenti Mencari\n");
	}else{
		printf("1. Cari Pokemon\n");
	}
	printf("2. Pokedex\n");
	printf("3. Shop\n");
	printf("4. Refresh\n");
	printf("Choose Number : ");
	int choice;
	scanf("%d", &choice);

	if(choice == 1){
		if(gameData->cariPokemon){
			gameData->cariPokemon = 0;
		}else{
			gameData->cariPokemon = 1;
			pthread_t thread;
			if(pthread_create(&thread, NULL, &cariPokemonThread, (void *)gameData) < 0){
				printf("can't create thread\n");
				exit(EXIT_FAILURE);
			}
		}
	}else if(choice == 2){
		changeState(&gameData->gameState, POKEDEX);
	}else if(choice == 3){
		changeState(&gameData->gameState, SHOP);
	}
}

void pokedex(GameData_t * gameData){
	printf("--POKEDEX--\n");
	if(gameData->pokeBag->count == 0){
		printf("Anda tidak mempunyai pokemon\n");
	}else{
		for(int i = 0; i < gameData->pokeBag->count; i++){
			printf("---------------\n");
			printf("%d. %s\n", i+1, gameData->pokeBag->pokemon[i]->name);
			printf("TYPE : %d\n", gameData->pokeBag->pokemon[i]->type);
			printf("AP : %d\n", gameData->pokeBag->pokemon[i]->ap);
			printf("Pokedollar : %d\n", gameData->pokeBag->pokemon[i]->price);
			printf("---------------\n");
		}
	}
	printf("Berry : %d\n", gameData->inventory->berry);
	printf("---------------\n");
	printf("1. Beri makan\n");
	printf("2. Lepas pokemon\n");
	printf("3. Kembali\n");
	printf("4. Refresh\n");
	printf("Choice : ");
	int choice;
	scanf("%d", &choice);
	if(choice == 1){
		if(gameData->inventory->berry){
			if(gameData->pokeBag->count){
				printf("Telah dikasi makan\n");
				gameData->inventory->berry -= 1;
				for(int i = 0; i < gameData->pokeBag->count; i++){
					addApPokemon(gameData->pokeBag->pokemon[i], 10);
				}
			}else{
				printf("Tidak punya pokemon\n");
			}
		}else{
			printf("Tidak punya berry\n");
		}
	}else if(choice == 2){
		printf("index pokemon yang mau dilepas : ");
		int pokemon;
		scanf("%d", &pokemon);
		if(pokemon <= 0 || pokemon > gameData->pokeBag->count){
			printf("Tidak ada yang dilepas\n");
		}else{
			int price = gameData->pokeBag->pokemon[pokemon-1]->price;
			removePokemon(gameData->pokeBag, pokemon);
			gameData->inventory->pokedollar+=price;
			printf("Pokemon dilepas, Pokedollar anda sekarang adalah %d\n", gameData->inventory->pokedollar);
		}
	}else if(choice == 3){
		changeState(&gameData->gameState, NORMAL_MODE);
	}
}

void shop(GameData_t * gameData){
	printf("--SHOP--\n");
	Shop_t * sharedShop = gameData->sharedShop;
	Inventory_t * inventory = gameData->inventory;
	printf("List item : \n");
	printf("1. Lullaby Powder | Stock : %d | Price : %d Pokedollar\n", sharedShop->lullabyPowder, sharedShop->lullabyPowderPrice);
	printf("2. Pokeball | Stock : %d | Price : %d Pokedollar\n", sharedShop->pokeball, sharedShop->pokeballPrice);
	printf("3. Berry | Stock : %d | Price : %d Pokedollar\n", sharedShop->berry, sharedShop->berryPrice);
	printf("----------------\n");
	printf("You have :\n");
	printf("1. Lullaby Powder | Stock : %d\n", inventory->lullabyPowder);
	printf("2. Pokeball | Stock : %d\n", inventory->pokeball);
	printf("3. Berry | Stock : %d\n", inventory->berry);
	printf("4. Pokedollar | Stock : %d\n", inventory->pokedollar);
	printf("----------------\n");
	printf("Aksi :\n");
	printf("1. Refresh\n");
	printf("2. Beli Lullabypowder\n");
	printf("3. Beli Pokeball\n");
	printf("4. Beli Berry\n");
	printf("5. Kembali\n");
	printf("choice : ");
	int choice;
	scanf("%d", &choice);
	if(choice == 2){
		if(inventory->pokedollar < sharedShop->lullabyPowderPrice){
			printf("Pokedollar tidak cukup\n");
		}else{
			if(sharedShop->lullabyPowder){
				printf("Telah dibeli 1 lullabyPowder\n");
				inventory->lullabyPowder++;
				inventory->pokedollar -= sharedShop->lullabyPowderPrice;
			}else{
				printf("Stock lullaby powder habis\n");
			}
		}
	}else if(choice == 3){
		if(inventory->pokedollar < sharedShop->pokeballPrice){
			printf("Pokedollar tidak cukup\n");
		}else{
			if(sharedShop->pokeball){
				printf("Telah dibeli 1 pokeball\n");
				inventory->pokeball++;
				inventory->pokedollar -= sharedShop->pokeballPrice;
			}else{
				printf("Stock pokeball habis\n");
			}
		}
	}else if(choice == 4){
		if(inventory->pokedollar < sharedShop->berryPrice){
			printf("Pokedollar tidak cukup\n");
		}else{
			if(sharedShop->berry){
				printf("Telah dibeli 1 berry\n");
				inventory->berry++;
				inventory->pokedollar -= sharedShop->berryPrice;
			}else{
				printf("Stock berry habis\n");
			}
		}
	}else if(choice == 5){
		changeState(&gameData->gameState, NORMAL_MODE);
	}
}

void captureMode(GameData_t * gameData){
	WildPokemon_t * pokemon = gameData->sharedPokemon;
	pokemon->isLocked = 1;
	pthread_t wild;

	WildPokemonParam_t * param = malloc(sizeof(WildPokemonParam_t));
	param->gameData = gameData;
	param->isRunning = 1;

	if(pthread_create(&wild, NULL, &wildpokemonThread, (void *)param) < 0){
		printf("can't create thread\n");
		exit(EXIT_FAILURE);
	}

	int choice;
	while(pokemon->isLocked){
		printf("--CAPTURE MODE--\n");
		printf("Pokemon :\n");
		printf("Nama : %s\n", pokemon->name);
		printf("Type : %d\n", pokemon->type);
		printf("Price : %d\n", pokemon->price);
		printf("Aksi :\n");
		printf("1. Tangkap\n");
		printf("2. Gunakan lullaby powder\n");
		printf("3. Keluar\n");
		printf("choice : ");
		scanf("%d", &choice);
		if(!pokemon->isLocked){
			printf("Pokemon telah kabur\n");
			break;
		}
		if(choice == 1){

			if(gameData->inventory->pokeball > 0){
				gameData->inventory->pokeball--;
				int random = generateRandom(pokemon->capRateB);
				if(random < pokemon->capRateA + gameData->jumlahLullabyActive * 2){

					param->isRunning = 0;

					Pokemon_t * tamedPokemon = tamePokemon(pokemon);
					if(addPokemon(gameData->pokeBag, tamedPokemon)){
						pthread_t t;
						PokemonParam_t * param = malloc(sizeof(PokemonParam_t));
						param->gameData = gameData;
						param->pokemon = tamedPokemon;
						if(pthread_create(&t, NULL, &pokemonThread, param) < 0){
							printf("can't create thread\n");
							exit(EXIT_FAILURE);
						}
						printf("Tertangkap\n");
						changeState(&gameData->gameState, NORMAL_MODE);
						pokemon->isLocked = 0;
					}else{
						printf("Tertangkap akan tetapi slot full jadi terlepas\n");
						free(tamedPokemon);
					}

				}else{
					printf("Tidak kena\n");
				}

			}else{
				printf("Tidak punya pokeball\n");
			}

		}else if(choice == 2){
			
			if(gameData->inventory->lullabyPowder){

				gameData->inventory->lullabyPowder--;
				pthread_t lullaby;
				if(pthread_create(&lullaby, NULL, &lullabyThread, gameData) < 0){
					printf("can't create thread\n");
					exit(EXIT_FAILURE);
				}

			}else{
				printf("Tidak punya lullaby powder\n");
			}

		}else if(choice == 3){
			pokemon->isLocked = 0;
			changeState(&gameData->gameState, NORMAL_MODE);
		}
	}
	pokemon->isLocked = 0;
}

void * wildpokemonThread(void * param){
	WildPokemonParam_t * parameter = (WildPokemonParam_t *)param;
	GameData_t * gameData = parameter->gameData;
	while (gameData->isRunning && parameter->isRunning){
		sleep(20);
		if(!parameter->isRunning || !gameData->isRunning){
			break;
		}
		if(gameData->jumlahLullabyActive) continue;
		int random = generateRandom(gameData->sharedPokemon->escapeRateB);
		if(random < gameData->sharedPokemon->escapeRateA){
			gameData->sharedPokemon->isLocked = 0;
			break;
		}
	}
	
}

void * cariPokemonThread(void * param){
	GameData_t * gameData = (GameData_t *) param;
	while(gameData->cariPokemon){
		if(generateRandom(10) <= 5){
			gameData->cariPokemon = 0;
			changeState(&gameData->gameState, CAPTURE_MODE);
			break;
		}
		sleep(10);
	}
}

void * pokemonThread(void * param){
	PokemonParam_t * p = (PokemonParam_t *)param;
	while (p->gameData->isRunning && !p->pokemon->released){
		sleep(10);
		if(p->gameData->gameState == CAPTURE_MODE)continue;
		p->pokemon->ap -= 10;
		if(p->pokemon->ap == 0){
			int random = generateRandom(10);
			if(random == 0){
				p->pokemon->ap = 50;
			}else{
				//lepas
				for(int i = 0; i < p->gameData->pokeBag->count; i++){
					if(p->gameData->pokeBag->pokemon[i] == p->pokemon){
						removePokemon(p->gameData->pokeBag, i);
						break;
					}
				}
			}
		}
	}
	
}

void * lullabyThread(void * param){
	GameData_t * gameData = (GameData_t *)param;
	gameData->jumlahLullabyActive++;
	sleep(10);
	gameData->jumlahLullabyActive--;
}