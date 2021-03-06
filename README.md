# SoalShiftSISOP20_modul3_F10

## 1. Soal 1 :
Poke*ZONE adalah sebuah game berbasis text terminal mirip dengan Pokemon GO.
### Traizone
#### Struct Game Data
```
const int NORMAL_MODE = 0;
const int POKEDEX = 1;
const int SHOP = 2;
const int CAPTURE_MODE = 3;

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
```
Struct game data berisi data-data yang banyak digunakan oleh fungsi lainnya.
- variabel gameState digunakan untuk state game sekarang. Ada 4 value yang mungkin, dimana setiap value disimpan di variabel const diatas.
- variabel isRunning digunakan untuk mengatur jalannya program, jika program ingin mematikan dirinya, dapat mengisi isRunning = 0.
- variabel cariPokemon digunakan untuk mengatur apakah gamenya sedang dalam mode mencari pokemon.
- variabel jumlahLullabyActive digunakan untuk mengetahui jumlah lullaby yang aktif.
- variabel inventory digunakan untuk menyimpan barang yang dipunyai player.
- variabel pokeBag digunakan untuk menyimpan pokemon yang dipunyai player.
- variabel sharedPokemon adalah shared memory dengan program pokezone. Digunakan untuk mengetahui wild pokemon yang ditemukan.
- variabel sharedShop adalah shared memory dengan program pokezone. Digunakan untuk mengetahui isi shop.

#### Struct Inventory
```
typedef struct Inventory_s{
	int pokeball;
	int lullabyPowder;
	int berry;
	int pokedollar;
}Inventory_t;
```
Struct inventory berisi barang yang dipunyai oleh player. Setiap barang tersebut disimpan di variabel integer.

#### Struct Poke Bag
```
typedef struct PokeBag_s{
	Pokemon_t * pokemon[7];
	int count;
}PokeBag_t;
```
Struct PokeBag digunakan untuk menyimpan semua pokemon yang sudah ditangkap oleh player. Penyimpanan ini menggunakan array, dan variabel count untuk jumlah yang sekarang di tangkap.

#### Struct Pokemon
```
typedef struct Pokemon_s{
	int released;
	char name[20];
	int type;
	int ap;
	int price;
}Pokemon_t;
```
Struct Pokemon digunakan sebagai entitas untuk setiap pokemon yang udah diambil.
- variabel name digunakan untuk menyimpan nama pokemon
- variabel type digunakan untuk menyimpan tipe pokemon, 0 berarti normal, 1 berarti rare, 2 berarti legendary
- variabel ap digunakan untuk menyimpan affection poin
- variabel price digunakan untuk mengetahui harga pokemon dalam pokedollar.

#### Struct Wild Pokemon
```
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
```
Struct Wild Pokemon digunakan untuk mengetahui data pokemon yang belum tertangkap atau pokemon yang berada di capture mode.

#### Struct Shop
```
typedef struct Shop_s{
	int isLocked;
	int lullabyPowder;
	int lullabyPowderPrice;
	int pokeball;
	int pokeballPrice;
	int berry;
	int berryPrice;
}Shop_t;
```
Struct Shop digunakan untuk mengetahui data shop yang bisa dibeli oleh player.

#### Main Function
```
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
```
Main function adalah awal mulai program. Disini akan berisi game loop, dan awal mula membuat memory dan share memory untuk GameData, Inventory, PokeBag, WildPokemon, dan Shop.
- ```srand(time(0))``` digunakan untuk setting seed randomiser.
- Untuk mengalokasikan memory GameData, Inventory, dan PokeBag digunakan fungsi ```malloc``` lalu kita isi variabel - variabel tersebut.
- Untuk WildPokemon dan Shop akan menggunakan shared memory. Menggunkan fungsi ```shmget```.
- Untuk game loop, digunakan ```while-loop``` terhadap ```isRunning``` yang ada di ```GameData```. Jadi jika ingin menghentikan gamenya, bisa mengatur isi ```isRunning``` jadi 0.
- Untuk setiap state akan mengeksekusi fungsi yang berbeda-beda. Dimana setiap fungsi akan mengambil ```GameData``` sebagai parameter.

#### generateRandom function
```
int generateRandom(int max){
	return rand()%max;
}
```
Fungsi sederhana untuk mengambil random dari 0 sampai max-1.

#### changeState function
```
void changeState(int * state, int nextState){
	*state = nextState;
}
```
Fungsi sederhana untuk mengganti state menjadi nextState.

#### normalMode function
```
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
```
Fungsi normal mode adalah fungsi yang akan dieksekusi saat state game adalah Normal Mode.
- Untuk 10 statement pertama, hanyalah sebagai ui dari menu tersebut.
- Lalu untuk inputnya adalah angka dimana ada 3 pilihan. Input diambil menggunakan fungsi ```scanf```.
- Untuk pilihan pertama, yaitu mencari pokemon. Jika belum mencari pokemon maka akan mulai mencari pokemon dengan memulai thread ```cariPokemonThread``` dan jika sedang mencari pokemon akan mengatur nilainya jadi 0 atau ```false```.
- Untuk pilihan kedua, yaitu pokedex. gameState akan diganti menggunakan fungsi ```changeState```.
- Untuk pilihan ketiga, yaitu shop. gameState akan diganti menggunakan fungsi ```changeState```.

#### pokedex function
```
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
```
Fungsi ini akan dieksekusi saat state gamenya adalah Pokedex.
- Untuk line pertama adalah judul.
- Selanjutnya akan ditampilkan Pokemon yang sudah ditangkap, jika tidak ada pokemon akan ditampilkan ```"Anda tidak mempunyai pokemon"```.
- Selanjutnya adalah ui untuk menampilkan aksi yang bisa dilakukan oleh player.
- Untuk input pilihan digunakan fungsi ```scanf``` dimana akan dimasukkan ke variabel choice.
- Untuk pilihan pertama, yaitu memberi makan berry. Pertama akan dicek jika mempunyai berry. Jika tidak punya maka print ```"Tidak punya berry"```. Jika punya maka setiap pokemon akan diberi 10 AP.
- Untuk pilihan kedua, yaitu melepas pokemon. Ambil input index yang mau dilepas. Jika input tidak valid maka print ```"Tidak ada yang dilepas"```. Jika valid maka akan dilepas menggunakan fungsi ```removePokemon``` lalu tambahkan price ke ```inventory->pokedollar```.
- Untuk pilihan ketiga, yaitu kembali. Maka ganti state dengan fungsi ```changeState```.

#### addPokemon function
```
int addPokemon(PokeBag_t * pokeBag, Pokemon_t * pokemon){
	if(pokeBag->count == 7)return 0;
	pokeBag->pokemon[pokeBag->count++] = pokemon;
	return 1;
}
```
Fungsi ini digunakan untuk menambahkan pokemon ke pokebag.
- Fungsi ini akan mereturn 0 jika penuh, dan mereturn 1 jika berhasil melakukan penambahan.

#### removePokemon function
```
void removePokemon(PokeBag_t * pokeBag, int index){
	pokeBag->pokemon[index-1]->released = 1;
	free(pokeBag->pokemon[index-1]);
	for(int i = index-1; i < pokeBag->count-1; i++){
		pokeBag->pokemon[i] = pokeBag->pokemon[i];
	}
	pokeBag->pokemon[pokeBag->count--] = NULL;
}
```
Fungsi ini digunakan untuk melepas pokemon dengan index tersebut di pokeBag.
- Setelah meremove, harus menggeser semua yang ada di kanan index yang di remove.

#### shop function
```
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
```
Fungsi ini akan dieksekusi saat gameState adalah Shop.
- Tampilkan isi dari shop yang didapatkan dari ```gameData->sharedShop```
- Tampilkan isi dari inventory yang didapatkan dari ```gameData->inventory```
- Tampilkan aksi yang bisa dilakukan
- Untuk input pilihan digunakan fungsi ```scanf``` dimana akan dimasukkan ke variabel choice.
- Untuk pilihan pertama, yaitu refresh. Maka tidak perlu melakukan apa - apa.
- Untuk pilihan kedua, tiga dan empat, yaitu beli item. Kita bisa check harga item dengan jumlah pokedollar, jika bisa maka dibeli.
- Untuk pilihan kelima, yaitu kembali. Maka dapat mengganti state dengan fungsi ```changeState```.

#### captureMode function
```
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
```
Fungsi ini akan dieksekusi saat gameState adalah Capture Mode.
- Buat thread WildPokemon, maka untuk simulasi kabur. Jika ```pokemon->isLocked``` itu berisi 0 maka pokemon telah kabur.
- Tampilkan data pokemon dari ```gameData->sharedPokemon```
- Tampilkan aksi yang bisa dilakukan
- Untuk input pilihan digunakan fungsi ```scanf``` dimana akan dimasukkan ke variabel choice.
- Untuk pilihan pertama, yaitu Tangkap. Maka akan mengurangi pokeball dari inventory. Jika tidak ada pokeball maka print ```"Tidak punya pokeball"```. Untuk kemungkinan ketangkap tidak pokemon, dapat diambil dari random CaptureRateA dan CaptureRateB. Jika tertangkap tetapi slot PokeBag full maka akan dilepas.
- Untuk pilihan kedua, yaitu menggunakan lullaby powder. Untuk menggunakannya, akan dijalankan thread ```lullabyThread```.
- Untuk pilihan ketiga, yaitu melepas pokemon. ```isLocked``` di set 0 dan mengganti state dengan fungsi ```changeState```.

#### cariPokemon Thread
```
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
```
Thread ini akan berjalan, saat pemain menyalakan cariPokemon.
- Thread akan loopin sampai cariPokemon bernilai 0
- untuk setiap 10 detik, akan ada chance 6/10 mendapatkan pokemon. Saat mendapatkan pokemon, cariPokemon akan dinonaktifkan. Lalu state akan diganti ke capture mode.

#### wildPokemon Thread
```
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
```
Thread ini akan berjalan, saat sedang dalam capture mode.
- Untuk setiap 20 detik, akan dirandomize sesuai dengan escapeRateA dan B. dimana kemungkinan escapeRateA/escapeRateB pokemon akan kabur.  Kecuali jika ada lullaby Powder yang aktif.

#### lullaby Thread
```
void * lullabyThread(void * param){
	GameData_t * gameData = (GameData_t *)param;
	gameData->jumlahLullabyActive++;
	sleep(10);
	gameData->jumlahLullabyActive--;
}
```
Thread ini akan berjalan, saat player menggunakan lullaby powder saat capture mode.
- Menambah jumlah lullaby yang aktif, setelah 10 s, maka kurangi yang aktif. Alias lullaby powder hanya aktif selama 10 detik.

#### tamePokemon function
```
Pokemon_t * tamePokemon(WildPokemon_t * wildPokemon){
	Pokemon_t * newPokemon = malloc(sizeof(Pokemon_t));
	memset(newPokemon, 0, sizeof(newPokemon));
	strcpy(newPokemon->name, wildPokemon->name);
	newPokemon->type = wildPokemon->type;
	newPokemon->ap = 100;
	newPokemon->price = wildPokemon->price;
	return newPokemon;
}
```
Fungsi ini akan berjalan saat ada pokemon yang ditangkap, untuk mengkonversi dari ```wildPokemon_t``` ke ```Pokemon_t```.

#### addApPokemon function
```
void addApPokemon(Pokemon_t * pokemon, int ap){
	pokemon->ap += ap;
}
```
Fungsi ini akan menambahkan Affective Point dengan ap.

### Pokezone

#### GameData Struct
```
typedef struct GameData_s{
	int isRunning;
	WildPokemon_t * sharedPokemon;
	Shop_t * sharedShop;
}GameData_t;
```
Struct game data berisi data-data yang banyak digunakan oleh fungsi lainnya.
- variabel isRunning digunakan untuk mengatur jalannya program, jika program ingin mematikan dirinya, dapat mengisi isRunning = 0.

#### Struct Wild Pokemon
```
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
```
Struct Wild Pokemon digunakan untuk mengetahui data pokemon yang belum tertangkap atau pokemon yang berada di capture mode.

#### Struct Shop
```
typedef struct Shop_s{
	int isLocked;
	int lullabyPowder;
	int lullabyPowderPrice;
	int pokeball;
	int pokeballPrice;
	int berry;
	int berryPrice;
}Shop_t;
```
Struct Shop digunakan untuk mengetahui data shop yang bisa dibeli oleh player.

#### generateRandom function
```
int generateRandom(int max){
	return rand()%max;
}
```
Fungsi sederhana untuk mengambil random dari 0 sampai max-1.

#### main function
```
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
		if(!id){
			FILE * pidProcess = popen("pidof soal1_traizone", "r");
			int pida = 0;
			while(fscanf(pidProcess, "%d", &pida) != EOF){
				id = fork();
				if(!id){
					char pid[10];
					snprintf(pid, sizeof(pid), "%d", pida);
					char * argv[] = {"kill", "-9", pid, NULL};
					execv("/bin/kill", argv);
				}
			}
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
```
Fungsi ini adalah fungsi awal.
- ```srand(time(0))``` digunakan untuk setting seed randomiser.
- Mengalokasikan memori untuk GameData, Shop dan WildPokemon.
- Shop dan WildPokemon menggunakan sharedMemory
- Menampilkan list aksi
- Untuk pilihan 1, yaitu keluar. Maka dengan fork-exec membunuh process sendiri dan hasil pidof soal1_traizone.

#### randomizePokemon Thread
```
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
		random = generateRandom(8000);
		if(random == 0){
			sharedPokemon->capRateA-= 2;
			sharedPokemon->escapeRateA+=1;
			sharedPokemon->price+=5000;
		}
		sleep(1);
	}
}
```
Thread ini berfungsi untuk merandomize pokemon yang akan ditangkap.
- jika pokemon sedang isLocked, artinya sharedPokemon sedang dibaca. Jadi tidak diubah.
- Untuk chance 80 persen akan mendapatkan normal, dan pokemon di set sesuai tabel.
- Untuk chance 15 persen akan mendapatkan normal, dan pokemon di set sesuai tabel.
- Untuk chance 5 persen akan mendapatkan normal, dan pokemon di set sesuai tabel.
- Setelah set pokemon, dichance 1 dari 8000, data pokemon ditambah atau dikurang sesuai soal.

#### shop Thread
```
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
```
Thread ini berfungsi untuk restock shop.
- Menambah setiap item dengan 10, dan jika melebihi 200 di set 200.

## 2. Soal 2

### tapserver

#### struct ServerData
```
typedef struct ServerData_s{
	int isRunning;
	int server_fd;
	struct sockaddr_in address;
	AkunData_t * akunData;
	int addrlen;
	Queue_t * queue;
}ServerData_t;
```
struct ServerData digunakan untuk menyimpan data yang akan digunakan di semua thread.
- ```isRunning``` adalah integer untuk mengetahui status program.
- ```server_fd```, ```address```, dan ```addrlen``` adalah variabel server socket.
- ```akunData``` digunakan untuk menyimpan akun yang sudah terregister.
- ```queue``` digunakan untuk player yang mau find match.

#### struct AkunData dan Akun
```
typedef struct Akun_s{
	char username[20];
	char password[20];
}Akun_t;

typedef struct AkunData_s{
	int count;
	Akun_t * akun[20];
}AkunData_t;
```
```AkunData``` menyimpan semua akun yang sudah terregister. Sedangkan ```Akun``` menyimpan username dan password setiap user.

#### Queue
```
typedef struct Node_s{
	struct Node_s * next;
	Player_t * value;
}Node_t;c

typedef struct Queue_s{
	Node_t * head;
	Node_t * tail;
	int count;
}Queue_t;

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
```
```Queue``` ini menyimpan player. Digunakan untuk sinkronisasi find match player.

#### struct Player
```
typedef struct Player_s {
	int socket;
	int health;
	struct Player_s * otherPlayer;
}Player_t;
```
Struct ini akan dipunyai untuk setiap client. ```Player``` menyimpan socket, health, dan pointer ke musuh. Dimana di awal otherPlayer adalah ```NULL```. Lalu setelah mendapat match akan mempunyai pointer ke ```Player``` lainnya.

#### main function
```
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
```
Fungsi ini adalah awal mulai program. Fungsi ini berisi :
- Mengalokasikan memori untuk ````ServerData```` dan ```AkunData```.
- Membuat server socket
- Membuat ```queue```
- Menjalankan thread ```pair``` dan ```listen```.

#### addAkun function
```
void addAkun(AkunData_t * akunData, const char * user, const char * pass){
	FILE * akunFile = fopen("akun.txt", "a");
	fprintf(akunFile, "%s\n%s\n", user, pass);
	fclose(akunFile);
	int count = akunData->count;
	Akun_t * akun = createAkun(user, pass);
	akunData->akun[count++] = akun;
	akunData->count = count;
}
```
Fungsi ini menambahkan akun baru keA AkunData.

#### createAkun function
```
Akun_t * createAkun(const char * user, const char * pass){
	Akun_t * akun = malloc(sizeof(Akun_t));
	strcpy(akun->username, user);
	strcpy(akun->password, pass);
	return akun;
}
```
Fungsi ini membuat akun baru sesuai input user dan password.

#### sendString function
```
void sendString(Player_t * connectionData, const char * str){
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	strncpy(buffer, str, sizeof(buffer));
	if(send(connectionData->socket, buffer, sizeof(buffer), 0) < 0){
		printf("Can't send string\n");
		exit(EXIT_FAILURE);
	}
}
```
Fungsi ini mengirim string ke socket yang ada di struct ```Player_t```.

#### receiveString function
```
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
```
Fungsi ini menerima string dari socket yang ada di struct ```Player_t``` lalu memasukkannya di string dest.

#### listen thread
```
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
```
Thread ini berfungsi untuk menerima client baru. Dimana setiap client akan dibuatkan thread masing - masing.

#### pair thread
```
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
```
Thread ini berfungsi untuk memasangkan player yang sedang find match. Jika player ingin find match, maka akan enqueue ke queue. Jika ada lebih dari 2 player yang find match, maka 2 player tadi akan dipasangkan.

#### player thread
```
void * playerThread(void * param){

	ThreadParam_t * p = (ThreadParam_t *)param;
	ServerData_t * serverData = p->serverData;
	Player_t * player = p->player;

    char buffer[1024];
	char * win = "win";

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
				sendString(player, win);
			}
		}
	}
	
}
```
Thread ini akan berjalan untuk setiap client. Berfungsi untuk mendengarkan inputan player dan memproses data.
- jika player ingin register, akan menerima string ```"register"```. Memanggil fungsi ```addAkun```.
- Jika player ingin login, akan menerima string ```"register"```. Mengcompare dengan struct ```AkunData``` yang ada di ```ServerData```.
- Jika player ingin find, akan menerima string ```"find"```. Enqueue struct ```Player``` ke ```queue``` yang ada di ```ServerData```.
- Jika player menembak, akan menerima string ```"fire"```. Player sudah mempunyai pasangan, jadi ```otherPlayer``` di struct ```Player``` tidak lagi ```NULL```. maka nyawa ```otherPlayer``` dikurangi 10. Lalu kirim pesan tersebut ke ```otherPlayer```.
- Jika nyawa ```otherPlayer``` atau nyawa sendiri 0 maka loop selesai.
- Jika nyawa ```Player``` tidak 0 saat keluar loop maka ```Player``` menang. Maka kirim pesan ```"win"```.

### tapplayer

#### getch function
```
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
```
Fungsi ini mengambil karakter tanpa perlu menggunakan enter.

#### struct GameData
```
#define MAIN_MENU 0
#define REGISTER_MENU 1
#define LOGIN_MENU 2
#define LOBBY_MENU 3
#define GAME_MENU 4

...

typedef struct GameData_s{
	int gameState;
	int isRunning;
}GameData_t;
```
Struct ini berisi data yang dibutuhkan oleh semua fungsi dan thread.
- ```gameState``` adalah state game. Isi ```gameState``` merupakan salah satu dari makro diatas.
- isRunning adalah integer untuk mengetahui status program.

#### struct MatchData
```
typedef struct MatchData_s{
	int win;
	int dead;
	int playerFound;
}MatchData_t;
```
Struct ini berisi data untuk match saat permainan nanti.
- ```win``` merupakan integer untuk status menang. Jika 1 maka player menang, Jika 0 maka player tidak menang.
- ```dead``` merupakan integer untuk status mati. Jika 1 maka player menang, Jika 0 maka player tidak mati.
- ```playerFound``` merupakan integer untuk status ditemukan player untuk match. Jika 1 maka player menang, Jika 0 maka player tidak ditemukan.

#### struct ConnectionData
```
typedef struct ConnectionData_s{
	int connectedSocket;
	struct sockaddr_in serverAddress;
}ConnectionData_t;
```
Struct ini berisi data koneksi seperti socket dan addres server.

#### struct ThreadParam
```
typedef struct ThreadParam_s{
	GameData_t * gameData;
	MatchData_t * matchData;
	ConnectionData_t * connectionData;
}ThreadParam_t;
```
berisi parameter untuk semua thread.

#### main function
```
int main(int argc, char const *argv[]) {
    pthread_t game;
	if(pthread_create(&game, NULL, &gameThread, NULL) < 0){
		printf("failed to create thread\n");
		exit(EXIT_FAILURE);
	}
	pthread_join(game, NULL);
    return 0;
}
```
Fungsi adalah awal mula program. Fungsi ini akan menjalankan thread game;

#### ConnetToServer function
```
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
```
Fungsi ini akan connect ke server dan mengembalikan ```ConnectionData``` yang berisi socket dan address server.

#### disconnectFromServer
```
void disconnectFromServer(ConnectionData_t * data){
	if(close(data->connectedSocket) < 0){
		printf("Error can't close socket\n");
		exit(EXIT_FAILURE);
	}
}
```
Fungsi ini akan menutup socket yang ada di ```ConnectionData```.

#### sendString function
```
void sendString(ConnectionData_t * connectionData, const char * str){
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	strncpy(buffer, str, sizeof(buffer));
	if(send(connectionData->connectedSocket, buffer, sizeof(buffer), 0) < 0){
		printf("Can't send string\n");
		exit(EXIT_FAILURE);
	}
}
```
Fungsi ini mengirim string ke socket yang ada di struct ```ConnectionData```.

#### receiveString function
```
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
```
Fungsi ini menerima string dari socket yang ada di struct ```ConnectionData``` lalu memasukkannya di string dest.

#### changeState function
```
void changeState(int * state, int nextState){
	*state = nextState;
}
```
Fungsi ini mengganti ```state``` jadi ```nextState```.

#### game Thread
```
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
```
Thread ini berjalan sebagai thread utama. Berisi :
- Mengalokasikan memori ```GameData```.
- Connect ke server.
- Game Loop.
- Menjalankan state sesuai ```gameState```.
- Disconnect from server.

#### mainMenu function
```
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
```
Fungsi ini akan berjalan, saat ```gameState``` merupakan ```MAIN_MENU```.
- Menampilkan aksi
- Menerima pilihan pemain
- Untuk input ```"login"```, maka state berubah jadi ```LOGIN_MENU```.
- Untuk input ```"register"```, maka state berubah jadi ```REGISTER_MENU```.

#### registerMenu function
```
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
```
Fungsi ini akan berjalan, saat ```gameState``` merupakan ```REGISTER_MENU```.
- Menerima input ```username``` dan ```password```.
- Mengirim pesan ```"register"``` ke server.
- Mengirim ```username``` dan ```password``` ke server.
- Kembali ke main menu.

#### loginMenu function
```
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
```
Fungsi ini akan berjalan, saat ```gameState``` merupakan ```LOGIN_MENU```.
- Menerima input ```username``` dan ```password```.
- Mengirim pesan ```"login"``` ke server.
- Mengirim ```username``` dan ```password``` ke server.
- Kembali ke main menu jika gagal.
- Pindah ke lobby menu jika berhasil.

#### lobbyMenu function
```
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
```
Fungsi ini akan berjalan, saat ```gameState``` merupakan ```LOBBY_MENU```.
- Menampilkan aksi
- Menerima pilihan pemain
- Untuk input ```"find"```, maka state berubah jadi ```GAME_MENU``` dan mengirim pesan ```"find"``` ke server.
- Untuk input ```"logout"```, maka state berubah jadi ```MAIN_MENU```.

#### gameMenu function
```
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
```
Fungsi ini akan berjalan, saat ```gameState``` merupakan ```GAME_MENU```.
- Membuat thread ```listen``` untuk mendengarkan pesan dari server.
- jika belum menemukan player, maka menunggu player dan menampilkan pesan ```"Waiting for player ..."```.
- Jika sudah menemukan player, maka akan menunggu input dengan fungsi ```getch```. Jika input sama dengan spasi maka kirim pesan ```"fire"``` ke server. Loopin akan selesai jika mati atau menang.
- Jika mati maka tampilkan ```"Game berakhir kamu kalah"```
- Jika menang maka tampilkan ```"Game berakhir kamu menang"```
- Ubah state menjadi ```LOBBY_MENU```.

#### listen Thread
```
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
```
Fungsi ini berjalan untuk mendengarkan input server.
- Jika mendapat pesan ```"player found"```, maka set ```playerFound``` jadi 1.
- Jika mendapat pesan ```"win"```, maka set ```win``` jadi 1.
- Jika mendapat pesan ```"int"```, maka tampilkan nyawa jika nyawanya bukan 0, jika 0 maka set ```dead``` jadi 1.
- Loop sampai mati atau menang.

## 3. Soal 3
```
const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}
``` 
- Fungsi untuk mengembalikan ekstensi dari file, di hitung mulai . (misalkan Archive.ZIP, maka return ekstensinya berupa ZIP)
- Apabila tidak menemukan ., maka return kosong (tidak ada ekstensinya)

```
int isFileExists(char *path)
{
    printf("%s\n", path);
    char f[100];
    snprintf(f, sizeof(f), "/home/amelia/soal3/soal3/%s", path);
    DIR *directory = opendir("/home/amelia/soal3/soal3"); //specify the directory name
    struct dirent * direntp;
    int i = 0;
    while((direntp = readdir(directory)) != NULL){
        struct stat   buffer;   
        int exist = stat(f, &buffer);
        if(exist == 0) {
            printf("ada\n");
            return 1;
        }
        else {
            printf("Tidak ada\n");
            return 0;
        }
    }
}
```
- Fungsi untuk mengecek apakah file yang diinput ada atau tidak pa adirektori menggunakan dirent.h
- Awalnya membuka directory dulu menggunakan ```DIR *directory = opendir("/home/amelia/soal3/soal3");``` dan membaca isinya
- Menggunakan fungsi stat () untuk membuat daftar properti file yang diidentifikasi oleh path ```int exist = stat(f, &buffer);```
- Apabila exist = 0, maka file ada dan return 1. Sebaliknya, apabila file tidak ada, maka return 0

```
void* isDirExists(void *arg) {
    struct stat st = {0};
    char dirName[100];
    snprintf(dirName, sizeof(dirName), "/home/amelia/amelia/%s", get_filename_ext(fileName));
    if (stat(dirName, &st) == -1)
    {
        mkdir(dirName, 0700);
    }
}
```
- Fungsi untuk mengecek apakah direktori (yang bernama sesuai ekstensi file) ada dan me
- Sama seperti isFileExist, menggunakan dirent.h
- Awalnya membuka directory dulu menggunakan ```DIR *directory = opendir("/home/amelia/soal3/soal3");``` dan membaca isinya
- Menggunakan fungsi stat () untuk membuat daftar properti file yang diidentifikasi oleh path ```stat(ext, &st) == -1```
- Apabila tidak ada file yang ter identifikasi, maka artinya direktori tidak ada dan otomatis akan membuat direktori sesuai ekstensi

```
void* moveFile (void *arg) {
    FILE *fptr1, *fptr2; 
    char file[100], dest[100], c; 
  
    // Open one file for reading 
    snprintf(file, sizeof(file), "/home/amelia/soal3/soal3/%s", fileName);
    fptr1 = fopen(file, "r"); 
    if (fptr1 == NULL) 
    { 
        printf("Cannot open file %s \n", file); 
    }
  
    // Open another file for writing 
    snprintf(dest, sizeof(dest), "/home/amelia/amelia/%s/%s", get_filename_ext(fileName), fileName);
    fptr2 = fopen(dest, "w"); 

    while ((c = fgetc(fptr1)) != EOF) fputc(c, fptr2);
    
    remove(file);
    printf("Contents copied to %s\n", dest); 
}
```
- Fungsi move file untuk memindahkan file di direktori 'soal3' ke direktori sekarang
- Menggunakan fopen fclose untuk membuka file dan fputc untuk menulis isi dari file
- Setelah file berhasil di pindah, gunakan remove(file) untuk menghapus file di direktori 'kategori'
- snprintf untuk menggabungkan array buffer yang nantinya dipakai untuk path pada file dan destination. ```snprintf(file, sizeof(file), "/home/amelia/soal3/soal3/%s", fileName);``` dan ```snprintf(dest, sizeof(dest), "/home/amelia/amelia/%s/%s", get_filename_ext(fileName), fileName);```

```
    printf("Masukkan Opsi : ");
    scanf("%s", x);
    strcpy(a, "-f");
    strcpy(b, "-d");
```
- Untuk memasukkan opsi command -f dan -d, * tidak perlu karena bukan merupakan string.
- -f dan -d dimasukkan ke string menggunakan ```strcpy(a, "-f");``` dan ```strcpy(b, "-d");```

```
if(strcmp(x, a)==0) {
	printf("Masukkan nama file : ");
	scanf("%s", fileName);
    if(isFileExists(fileName)){
        int* p;
        int * p1; 
        int x, y;

        pthread_create(&threads[0], NULL, isDirExists, (void*)(p)); 
        pthread_create(&threads[1], NULL, moveFile, (void*)(p)); 

        pthread_join(threads[0], NULL);
        pthread_join(threads[1], NULL);
    } 
}
```
- Untuk opsi -f. Menggunakan strcmp untuk membandingkan input command dengan -f / -d
- Masukkan nama file, lalu cek apakah nama file ada di direktori menggunakan fungsi ```isFileExists()```
- Menggunakan thread untuk membuat direktori dan thread untuk memindahkan file

## 4. Soal 4

### 4a
Untuk Shared Memory nya
```
    key_t key = 1234;
    int shmid = shmget(key, sizeof(int)*rC*cC, IPC_CREAT | 0666);
    matC = (int *)shmat(shmid, NULL, 0);
```
- untuk pertukaran data antara program 4a dan 4b 

Deklarasi isi Matriks A dan Matriks B
```
    matA[0][0] = 1;
	matA[0][1] = 2;
	matA[1][0] = 2;
	matA[1][1] = 1;
	matA[2][0] = 2;
	matA[2][1] = 1;
	matA[3][0] = 2;
	matA[3][1] = 1;

	matB[0][0] = 2;
	matB[0][1] = 1;
	matB[0][2] = 3;
	matB[0][3] = 4;
	matB[0][4] = 1;
	matB[1][0] = 2;
	matB[1][1] = 3;
	matB[1][2] = 2;
	matB[1][3] = 1;
	matB[1][4] = 2;
```
- di tulis satu per satu elemen matriks nya 

Deklarasi thread nya 
```
pthread_t threads[4]; 
  
    for (int i = 0; i < 4; i++) { 
        int* p; 
        pthread_create(&threads[i], NULL, multi, (void*)(p)); 
    } 
  
    for (int i = 0; i < 4; i++)  
        pthread_join(threads[i], NULL);   
```  
- Dibuat thread sebanyak jumlah perkalian matriks (karena ada 4 x 2 dan 2 x 5, maka ada 4 thread. Masing - masing thread mengoperasikan 1 baris 1 kolom)
- Masing - masing thread di join

Fungsi Perkalian Matriks nya
```
void* multi(void* arg) 
{ 
    int core = step_i++; 
    for (int i = core * 4 / 4; i < (core + 1) * 4 / 4; i++)  {
        for (int j = 0; j < 5; j++)  {
            for (int k = 0; k < 2; k++) {  
                result[i][j] += matA[i][k] * matB[k][j]; 
            }
        }
    }
} 
```
- Masing - masing thread menghitung 1 / 4 dari perkalian matriks nya

Print hasil perkalian matriksnya 
```
printf("Multiplication of A and B : \n");
    for (int i = 0; i < rC; i++) { 
        for (int j = 0; j < cC; j++)  {
            matC[i*cC+j] = result[i][j];
            printf("%d ", matC[i*cC+j]);
        }
        printf("\n");
    } 
```
- result matriks di masukkan ke matriks C, dan di print

### 4b
Untuk shared memory memory nya
```
    key_t key = 1234;
    int shmid = shmget(key, sizeof(int)*rC*cC, IPC_CREAT | 0666);
    matC = (int *)shmat(shmid, NULL, 0);
```

Print hasil perkalian matriks dan program 4a
```
printf("Perkalian Matriks A dan Matriks B : \n");
    for (int i = 0; i < rC; i++) { 
        for (int j = 0; j < cC; j++)  {
            printf("%d ", matC[i*cC+j]); 
            arr[i][j] = matC[i*cC+j];
        }
        printf("\n");
    } 
```
- Hasil dari print matriks C di masukkan ke arr untuk di pakai saat menambahkan masing - masing elemen matriks

Deklarasi thread untuk penambahan elemen masing" matriks
```
    pthread_t threads[2]; 
    int* p;

    pthread_create(&threads[0], NULL, plus, (void*)(p));
    pthread_join(threads[0], NULL); 
```
- membuat thread untuk menghitung penambahan elemen matriks
- Join thread

Fungsi untuk menambahkan masing - masing elemen hasil perkalian matriks
```
void *plus(void *arg) {
    printf("\nHasil Penambahan Matriks : \n");
    for (int i = 0; i < 4; i++)  {
        for (int j = 0; j < 5; j++) {  
            int n = arr[i][j];
            for (int k = 1; k <= n; k ++) {
                res += k;
            }
            arr[i][j] = res;
            printf("%d\t", res);
            res = 0;
        }
        printf("\n");
    }
    pthread_exit(0);
}
```
- Menghitung penambahan di mulai dari 1 hingga n (elemen arr[i][j]), lalu hasilnya += k dan di masukkan ke arr kembali. arr kembali di set 0
- Apabila sudah selesai melakukan operasi, exit thread
- print hasil penambahan elemen matriks


### 4c
```
int main () {
int main() 
{ 
	int p1[2];

	pid_t p;
    int count=0; 
    DIR * directory = opendir("/home/amelia/amel/SoalShiftSISOP20_modul3_F10/soal4");
    struct dirent * de;
    struct stat mystat;

	p = fork(); 

	if (p < 0) 
	{ 
		fprintf(stderr, "fork Failed" ); 
		return 1; 
	} 

	// Parent process 
	else if (p > 0) 
	{
        dup2(p1[0],0);
	    while((de = readdir(directory)) != NULL){
            if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;
            if(de->d_type == DT_REG || de->d_type == DT_DIR){
                count++;
            }
        }
        printf("%d\n", count);
	} 

	// child process 
	else
	{ 
		dup2(p1[1], 1);
        close(p1[0]); 
        char buf[512];
        directory = opendir("/home/amelia/amel/SoalShiftSISOP20_modul3_F10/soal4");
        while((de = readdir(directory)) != NULL)
        {
            sprintf(buf, "%s/%s", "/home/amelia/amel/SoalShiftSISOP20_modul3_F10/soal4", de->d_name);
            stat(buf, &mystat);
            printf("%zu\t %s\n",mystat.st_size, de->d_name);
        }
        closedir(directory);
 	} 
} 
}
```
- Array 2 dimensi p , dimana p[0] untuk reading dan p[1] untuk writing
- Menggunakan duplicate untuk meng input hasil dari child dan dimasukkan ke proses parent
- Pada proses child, membuka directory dan meng list apa saja file yang ada di dalamnya 
- Hasil nama - nama dari file yang ada di direktori sekarang menjadi input untuk proses parent
- Pada parent process, menduplikat hasil dari child proses. Apabila directory berupa . atau .., maka tidak dihitung. Count ++ untuk menghitung jumlah file di direktori dan di print