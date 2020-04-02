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

## 2. Soal 2
## 3. Soal 3
## 4. Soal 4