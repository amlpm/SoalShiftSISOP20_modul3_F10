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

#### Normal Mode

## 2. Soal 2
## 3. Soal 3
## 4. Soal 4
