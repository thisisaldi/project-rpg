#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>

/*
Fix list:

- Adjust game stats

*/

typedef struct {
    char nama_makanan[20];
    int heal;
    int harga;
} Makanan;

typedef struct {
    char nama_senjata[20];
    int damage;
    int harga;
} Senjata;

typedef struct {
    char nama[20];
    int health;
    int xp;
    int level;
    int coin;
    Makanan* makanan[20];
    Senjata* senjata[20];
} Karakter;

char nama[20];
char stage_boss_names[5][20] = {"Ancient Dragon", "Orc King", "Minotaur", "Roshan", "Ilhamgod"};


int play, aksi, req_xp, lower_damage_multiplier, upper_damage_multiplier, temp, temp1, temp2;

/*

    play                    : Variabel untuk mengontrol bermain atau tidaknya
    aksi                    : Variabel untuk meyimpan aksi pemain
    req_xp                  : XP yang dibutuhkan untuk naik level
    lower_damage_multiplier : batas bawah damage player
    upper_damage_multiplier : batas atas damage player
    temp, temp1, temp2      : variabel temporary

*/
int loop_count = 1;
int stage = 1;
int pakai_senjata = 0;
int idle = 0;
int mati = 0;
int kabur;
int next_stage = 0;

/*  Parameter Game */
int makanan_yang_dipunyai   = 0;
int senjata_yang_dipunyai   = 0;

int goblin_xp_multiplier    = 4;
int goblin_hp_multiplier    = 15;
int goblin_low_damage;
int goblin_high_damage;

int orc_xp_multiplier       = 6;
int orc_hp_multiplier       = 18;
int orc_low_damage;
int orc_high_damage;

int golem_xp_multiplier     = 10;
int golem_hp_multiplier     = 17;
int golem_low_damage;
int golem_high_damage;

int spirit_xp_multiplier     = 20;
int spirit_hp_multiplier     = 25;
int spirit_low_damage;
int spirit_high_damage;

int stage_boss_xp_multiplier= 30;
int stage_boss_hp_multiplier= 40;

int per__stage__ = 5;

int kabur_rate              = 2;

int golem_spawn_chance      = 20;// -> 1/20
int orc_spawn_chance        = 10;// -> 1/10
int goblin_spawn_chance     = 2; // -> 1/2
int dryad_spawn_chance      = 200; // -> 1/200

int drop_apel_rate          = 4; 
int drop_roti_rate          = 10; // invert -> 9/10
int drop_pisang_rate        = 10; // invert -> 9/10
int drop_tango_rate         = 10; // invert -> 9/10

int byk_shop                = 3;

int dryad_available = 1;

/*  Inisialisasi Object   */
void __init__karakter(Karakter* self, char *nama, int health) {
    strcpy(self->nama, nama);
    self->health = health;
}

Karakter* buat_karakter(char *nama, int health) {
    Karakter* result = (Karakter*) malloc(sizeof(Karakter));
    __init__karakter(result, nama, health);
    return result;
}

void __init__makanan(Makanan* self, char *nama, int heal) {
    strcpy(self->nama_makanan, nama);
    self->heal = heal;
}

Makanan* buat_makanan(char *nama, int heal) {
    Makanan* result = (Makanan*) malloc(sizeof(Makanan));
    __init__makanan(result, nama, heal);
    return result;
}

void __init__senjata(Senjata* self, char *nama, int damage) {
    strcpy(self->nama_senjata, nama);
    self->damage = damage;
}

Senjata* buat_senjata(char *nama, int damage) {
    Senjata* result = (Senjata*) malloc(sizeof(Senjata));
    __init__senjata(result, nama, damage);
    return result;
}

/*  Metode Objek    */
void serang(Karakter* self, Karakter* lawan, int self_damage, int lawan_damage) {
    printf("\tKamu menyerang %s! %s -%d HP!\n", lawan->nama, lawan->nama, self_damage);
    lawan->health -= self_damage;
    if(lawan->health > 0) {
        printf("\t%s menyerangmu! %s -%d HP!\n", lawan->nama, self->nama, lawan_damage);
        self->health -= lawan_damage;
        if(self->health < 0) {
            self->health = 0;
        }
    }
}

void diserang(Karakter* self, Karakter* lawan, int rand_lawan) {
    int lawan_damage = lawan->level * rand_lawan;
    if(lawan->health > 0) {
        printf("\t%s menyerangmu! %s -%d HP!\n", lawan->nama, self->nama, lawan_damage);
        self->health -= lawan_damage;
        if(self->health < 0) {
            self->health = 0;
        }
    }
}
/*  Fungsi Randomizer Angka */
int randomizer(int bottom, int top) {
    return (rand() % (top - bottom + 1)) + bottom;
}

/*  Method-method untuk Object-object */
void makan(Karakter* self) {
    int pilihan;
    if(makanan_yang_dipunyai == 0) {
        printf("\nAnda tidak mempunyai makanan.\n");
    } else {
        printf("\nPilih makanan yang ingin anda makan.\nMakanan yang anda punya : \n");
        printf("0. Kembali\n");
        for(int i = 0; i < makanan_yang_dipunyai; i++) {
            printf("%d. %s +%d HP\n", i + 1, self->makanan[i]->nama_makanan, self->makanan[i]->heal);
        }
        printf("Pilih : "); scanf("%d", &pilihan);
        if(pilihan != 0) {
            printf("Anda memakan %s.\nAnda memulihkan %d HP.\n", self->makanan[pilihan - 1]->nama_makanan, self->makanan[pilihan - 1]->heal);
            self->health += self->makanan[pilihan-1]->heal;
            for(int i = pilihan - 1; i < makanan_yang_dipunyai - 1; i++) {
                self->makanan[i] = self->makanan[i+1];
            }
            makanan_yang_dipunyai -= 1;
        }
    }
}

int cek_senjata(Karakter* self) {
    int pilihan;
    if(senjata_yang_dipunyai == 0) {
        printf("Anda tidak mempunyai senjata.\n");
        return 0;
    } else {
        printf("Pilih senjata yang ingin anda pakai.\nSenjata yang anda punya : \n");
        printf("0. Kembali\n");
        for(int i = 0; i < senjata_yang_dipunyai; i++) {
            printf("%d. %s +%d damage multiplier\n", i + 1, self->senjata[i]->nama_senjata, self->senjata[i]->damage);
        }
        printf("Pilih : "); scanf("%d", &pilihan);
        if(pilihan != 0) {
            printf("Anda telah memilih %s\n", self->senjata[pilihan - 1]->nama_senjata);
        }
        return pilihan;
    }
}

void coin_drop(Karakter* self, int rate) {
    int coin_gain = randomizer(1, 100) * rate;
    if(rand() % 4 != 0) {
        self->coin += coin_gain;
    }
    printf("\tAnda mendapatkan %d koin!\n", coin_gain);
}

void death_event(Karakter* self, Karakter *lawan, Makanan* makanan, int drop_rate, int xp_multiplier, int req_xp) {
    printf("\t%s telah mati!\n", lawan->nama);
    coin_drop(self, lawan->level * 2);
    printf("\t+%d XP!\n", lawan->level * xp_multiplier);
    if(rand() % drop_rate != 0) {
        if(makanan_yang_dipunyai < 20) {
            printf("\t%s menjatuhkan %s!\n\tAnda mendapatkan %s!\n", lawan->nama, makanan->nama_makanan, makanan->nama_makanan);
            self->makanan[makanan_yang_dipunyai] = makanan;
            makanan_yang_dipunyai += 1;
        } else {
            printf("\t%s menjatuhkan %s!\n\tTas anda penuh, anda tidak bisa mengambil %s\n", lawan->nama, makanan->nama_makanan, makanan->nama_makanan);
        }
    }
    self->xp += lawan->level * xp_multiplier;
    if(self->xp / req_xp >= 1) {
        self->level += self->xp / req_xp;
        self->xp = self->xp % req_xp;
        printf("\tAnda telah naik level!\n");
    }
}

void menu_shop(Senjata** daftar_barang) {
    printf("Daftar barang : \n");
    if(byk_shop == 0)
        printf("Barang di shop kosong.\n");
    printf("\n======================================================================\n");
    for(int i = 0; i < byk_shop; i++) {
        printf("%d. %s\n\t%dx damage multiplier\n\tHarga : %d\n", i + 1, daftar_barang[i]->nama_senjata, daftar_barang[i]->damage, daftar_barang[i]->harga);
    }
    printf("======================================================================\n");
}

void beli_shop(Karakter* self, Senjata** barang) {
    int pilihan = 1;
    int akad;
    while(pilihan) {
        menu_shop(barang);
        if(byk_shop == 0)
            break;
        printf("Senjata nomor berapa yang ingin anda beli? (Ketik 0 untuk kembali)\n");
        printf("Pilih : "); scanf("%d", &pilihan);
        if(pilihan > 0 && pilihan <= byk_shop) {
            if(self->coin >= barang[pilihan - 1]->harga) {
                printf("Anda yakin ingin membeli %s (%d koin)?\n(1) Ya (2) Tidak\n", barang[pilihan - 1]->nama_senjata, barang[pilihan - 1]->harga);
                printf("Pilih : "); scanf("%d", &akad);
                if(akad == 1) {
                    self->coin -= barang[pilihan - 1]->harga;
                    self->senjata[senjata_yang_dipunyai] = barang[pilihan - 1];
                    senjata_yang_dipunyai += 1;
                    byk_shop -= 1;
                    for(int i = pilihan - 1; i < byk_shop + 1; i++) {
                        barang[i] = barang[i + 1];
                    }
                    printf("Koin anda sekarang : %d\n\n", self->coin);
                }
            } else {
                printf("Koin anda tidak mencukupi.\nSilakan pilih barang yang lain.\n\n");
            }
        } else if(pilihan > byk_shop) {
            printf("Mohon masukkan input sesuai dengan pilihan yang tersedia.\n");
        } else break;
    }
}


void bar_hp(Karakter* self) {
    for(int i = 0; i < self->health; i++)
        printf("*");
}

void bertarung(Karakter* self, Karakter* lawan, int low_damage, int high_damage, int lawan_low, int lawan_high, 
            Makanan* makanan, int drop_rate, int xp_multiplier, int req_xp, int max_hp, int event) {
    int aksi, temp, temp1, temp2;
    if(!(event))
        printf("%s menyergap Anda!\n", lawan->nama);
    while(lawan->health > 0 && play) {
        printf("\n\tHP Player Lv. %d (%d/%d)\t\t: " , self->level, self->health, max_hp); bar_hp(self); printf("\n");
        printf("\tHP Lawan  Lv. %d (%d)\t\t: " , lawan->level, lawan->health); bar_hp(lawan); printf("\n");
        printf("\tAksi : (1) Serang (2) Makan (3) Kabur (4) Pergi dari game\n");
        printf("\tPilih : "); scanf("%d", &aksi);
        switch(aksi) {
            case 1: temp1 = randomizer(low_damage, high_damage);
                    temp2 = randomizer(lawan_low, lawan_high);
                    serang(self, lawan, temp1, temp2); 
                    break;
            case 2: makan(self);
                    if(self->health > max_hp) {
                        self->health = max_hp;
                    } 
                    break;
            case 3: if(rand() % kabur_rate == 0) {
                        printf("\tAnda berhasil kabur!\n");
                        kabur = 1;
                    } else {
                        printf("\tAnda gagal kabur!\n");
                        temp = randomizer(2, 4);
                        diserang(self, lawan, temp);
                    }
                    break;
            case 4: printf("Anda yakin ingin keluar dari game?\n(1) Ya (2) Tidak\n");
                    printf("Pilih : "); scanf("%d", &aksi);
                    if(aksi == 1) {
                        play = 0 ;
                    } else {
                        idle = 1;
                    }
                    break;
            default: printf("\tMohon masukkan angka sesuai dengan aksi yang diinginkan!\n");
                    break;
        }
        if(kabur) {
            break;
        }
        if(self->health <= 0) {
            printf("\nGAME OVER!\nKamu telah mati.\n");
            play = 0;
            mati = 1;
            break;
        }
        if(lawan->health <= 0) {
            death_event(self, lawan, makanan, drop_rate, xp_multiplier, req_xp);
        }
    }
}


void dryad_interaction(Karakter* self, Karakter* lawan, Senjata* oxidice, int low_damage, int high_damage, int lawan_low, int lawan_high, 
            Makanan* makanan, int drop_rate, int xp_multiplier, int req_xp, int max_hp, int event) {
    int aksi;
    printf("\n\t!!!RARE EVENT!!!\n\n");
    printf("Anda bertemu dengan Dryad!\n");
    printf("Dryad: \"Halo manusia, bersediakah kamu menolong kami?\"\n");
    printf("Dryad: \"Bisakah kau tolong kami mengalahkan Evil Spirit yang mengganggu kami?\"\n");
    printf("Aksi : (1) Bantu (2) Serang Dryad (3) Kabur\n");
    printf("Pilih : "); scanf("%d", &aksi);
    switch(aksi) {
        case 1: printf("Anda bertemu dengan Evil Spirit!!\n");
                bertarung(self, lawan, low_damage, high_damage, lawan_low, lawan_high, 
                    makanan, drop_rate, xp_multiplier, req_xp, max_hp, event);
                if(lawan->health <= 0) {
                    if(senjata_yang_dipunyai < 20) {
                            printf("\tDryad memberikan anda %s sebagai terima kasih!\n\tAnda mendapatkan %s!\n",  oxidice->nama_senjata, oxidice->nama_senjata);
                            self->senjata[senjata_yang_dipunyai] = oxidice;
                            senjata_yang_dipunyai += 1;
                    } else {
                            printf("\tDryad memberikan anda %s sebagai terima kasih!\n\tSenjata awal anda terganti dengan %s!\n", oxidice->nama_senjata, oxidice->nama_senjata);
                    }
                    printf("Dryad memberikan anda 10000 Koin!\n");
                    self->coin += 10000;
                    dryad_available = 0;
                }
                break;
        case 2: printf("Anda menyerang Dryad!\nDryad murka!\n");
                printf("Anda terkena debuff! Level anda berkurang!\n");
                if(self->level > 5)
                    self->level -= 5;
                else 
                    self->level = 1;
        case 3: printf("Anda kabur!\n"); break;
    }
}


int main() {

    printf("Masukkan nama karakter anda : ");
    fgets(nama, sizeof(nama), stdin);
    nama[strcspn(nama, "\n")] = 0;

    /*  Buat Object */
    Karakter* main_char = buat_karakter(nama, 40);
    Karakter* evil_spirit = buat_karakter("Evil Spirit", 10);
    Karakter* goblin = buat_karakter("Goblin", 12);
    Karakter* orc = buat_karakter("Orc Merdono", 18);
    Karakter* golem = buat_karakter("Mud Golem", 25);
    Karakter* stage_boss = buat_karakter("Orc King", 20);

    Makanan* apel = buat_makanan("Apel", 8);
    Makanan* roti = buat_makanan("Roti", 15);
    Makanan* pisang = buat_makanan("Pisang", 10);
    Makanan* tango = buat_makanan("Tango", 25);
    Makanan* salve = buat_makanan("Healing Salve", 100);
    Makanan* golden_apple = buat_makanan("Golden Apple", 1000);

    Senjata* oxidice = buat_senjata("Oxidice", 8);

    Senjata* stage_1 = buat_senjata("War Axe", 1);
    Senjata* stage_2 = buat_senjata("Blade of Despair", 2);
    Senjata* stage_3 = buat_senjata("Silver Edge", 3);
    Senjata* stage_4 = buat_senjata("Manta Style", 4);
    Senjata* stage_5 = buat_senjata("Desolator", 6);
    Senjata* stage_6 = buat_senjata("Meteor Hammer", 7);
    Senjata* stage_7 = buat_senjata("Radiance", 9);
    Senjata* stage_8 = buat_senjata("Divine Rapier", 10);
    Senjata* drop_senjata[8] = {stage_1, stage_2, stage_3, stage_4, stage_5, stage_6, stage_7, stage_8};

    Senjata* shop_1 = buat_senjata("Chunchunmaru", 6);  shop_1->harga = 15000;
    Senjata* shop_2 = buat_senjata("Aghanim Blade", 8); shop_2->harga = 30000;
    Senjata* shop_3 = buat_senjata("AWP :D", 12);       shop_3->harga = 50000;

    byk_shop = 3;
    Senjata* shop_list[3] = {shop_1, shop_2, shop_3};


    main_char->xp = 0;
    main_char->level = 1;
    main_char->coin = 0;

    /*  Randomize Seed  */
    srand(time(NULL));

    // printf("\n======================================================================\n");

    // printf("Peraturan:\n");
    // printf("1. Anda hanya bisa berjalan ke sub-stage selanjutnya jika sudah tidak ada monster yang menyerang anda.\n");
    
    // printf("\n======================================================================\n");

    do {
        printf("Mulai bermain?\n(1) Ya (2) Tidak\n");
        printf("Pilih (1) / (2): "); scanf("%d", &play);
    } while(play != 1);
    
    while(play && !(mati)) {

        int max_hp = 34 + (main_char->level  * 6);

        if(next_stage) {
            loop_count = 1;
            stage++;
        }

        next_stage = 0;
        kabur = 0;

        if(pakai_senjata) {
            lower_damage_multiplier = 1 * (main_char->level + main_char->senjata[pakai_senjata - 1]->damage);
            upper_damage_multiplier = 3 * (main_char->level + main_char->senjata[pakai_senjata - 1]->damage);
        } else {
            lower_damage_multiplier = 1 * (main_char->level);
            upper_damage_multiplier = 3 * (main_char->level);
        }
        
        
        /*  Monster Stats Parameter     */

        goblin->level       = stage;
        goblin->health      = 10 + (goblin->level - 1) * goblin_hp_multiplier;
        
        orc->level          = stage;
        orc->health         = 15 + (orc->level - 1) * orc_hp_multiplier;

        golem->level        = stage - 1;
        golem->health       = golem->level * golem_hp_multiplier;

        stage_boss->level   = stage;
        stage_boss->health  = 30 + ((stage_boss->level - 1) * stage_boss_hp_multiplier);

        evil_spirit->level  = stage;
        evil_spirit->health = 30 + ((evil_spirit->level - 1) * spirit_hp_multiplier);
        
        /*  Monster Damage Parameter    */
        goblin_low_damage = 2 + ((goblin->level - 1) * 6);
        goblin_high_damage = 3 + ((goblin->level - 1) * 6);

        orc_low_damage = 2 + ((orc->level - 1) * 8);
        orc_high_damage = 4 + ((orc->level - 1) * 8);

        golem_low_damage = 3 + ((golem->level - 1) * 10);
        golem_high_damage = 6 + ((golem->level - 1) * 10);

        spirit_low_damage = 1 + ((evil_spirit->level - 1) * 10);
        spirit_high_damage = 2 + ((evil_spirit->level - 1) * 10);

        req_xp = 3 + main_char->level * 3;
        
        printf("\n======================================================================\n");
        printf("Stage %d (%d/%d)\n", stage, loop_count, per__stage__);
        printf("Nama\t\t: %s\n", main_char->nama);
        printf("LV\t\t: %d (%d XP/%d XP)\n", main_char->level, main_char->xp, req_xp);
        printf("HP (%d/%d)\t: ", main_char->health, max_hp);
        bar_hp(main_char);
        printf("\n");
        printf("Koin\t\t: %d\n", main_char->coin);
        printf("Damage\t\t: %d - %d\n", lower_damage_multiplier, upper_damage_multiplier);
        if(pakai_senjata)
        printf("Senjata yang dipakai: \n%s +%d damage multiplier\n", main_char->senjata[pakai_senjata - 1], main_char->senjata[pakai_senjata - 1]->damage);
        printf("======================================================================\n\n");


        if((rand() % dryad_spawn_chance == 0) && dryad_available) { // Dryad event
            dryad_interaction(main_char, evil_spirit, oxidice, lower_damage_multiplier, upper_damage_multiplier, spirit_low_damage, spirit_high_damage,
                golden_apple, 1, spirit_xp_multiplier, req_xp, max_hp, 1);
        } else if((rand() % golem_spawn_chance == 0) && (stage > 1)) { // Golem spawning
            bertarung(main_char, golem, lower_damage_multiplier, upper_damage_multiplier, golem_low_damage, golem_high_damage,
                roti, drop_tango_rate, golem_xp_multiplier, req_xp, max_hp, 0);
        } else if(rand() % orc_spawn_chance == 0)  { // orc spawn
            bertarung(main_char, orc, lower_damage_multiplier, upper_damage_multiplier, orc_low_damage, orc_high_damage,
                roti, drop_roti_rate, orc_xp_multiplier, req_xp, max_hp, 0);
        } else if(rand() % goblin_spawn_chance != 0) { // goblin spawn
            bertarung(main_char, goblin, lower_damage_multiplier, upper_damage_multiplier, goblin_low_damage, goblin_high_damage,
                pisang, drop_pisang_rate, goblin_xp_multiplier, req_xp, max_hp, 0);
        } else {
            printf("Aksi : (1) Jalan (2) Makan (3) Lihat tas senjata (4) Menu shop (5) Pergi dari game\n");
            printf("Pilih : "); scanf("%d", &aksi);
            
            switch(aksi) {
                case 1: if(rand() % drop_apel_rate == 0) {
                            printf("Anda menemukan apel!\n");
                            main_char->makanan[makanan_yang_dipunyai] = apel;
                            makanan_yang_dipunyai += 1;
                        } else {
                            printf("Anda tidak menemukan apa-apa.\n");
                        }
                        if(loop_count % per__stage__ != 0)
                            loop_count++;
                        break;
                case 2: makan(main_char);
                        if(main_char->health > max_hp) {
                            main_char->health = max_hp;
                        } 
                        break;
                case 3: pakai_senjata = cek_senjata(main_char);
                        break;
                case 4: beli_shop(main_char, shop_list);
                        break;
                case 5: printf("Anda yakin ingin keluar dari game?\n(1) Ya (2) Tidak\n");
                        printf("Pilih : "); scanf("%d", &aksi);
                        if(aksi == 1) {
                            play = 0;
                        }
                        break;
                default:    printf("\tMohon masukkan angka sesuai dengan aksi yang diinginkan!\n");
                            break;
            }
        }
        if(loop_count % per__stage__ == 0 && play != 0) {
            printf("\nHP Boss (%d) : ", stage_boss->health); bar_hp(stage_boss); printf("\n");
            printf("Lawan Stage boss sekarang untuk naik ke stage selanjutnya?\nAksi: (1) Ya (2) Tidak\n");
            printf("Pilih : "); scanf("%d", &aksi);
            switch(aksi) {
                case 1: printf("%s telah muncul!\n", stage_boss_names[stage % 5]);
                        while(stage_boss->health > 0 && play != 0) {    
                            printf("\n\tHP Player Lv. %d (%d/%d)\t\t: " , main_char->level, main_char->health, max_hp); bar_hp(main_char); printf("\n");
                            printf("\tHP Lawan  Lv. %d (%d)\t\t: " , stage_boss->level, stage_boss->health); bar_hp(stage_boss); printf("\n");
                            printf("\tAksi : (1) Serang (2) Makan (3) Kabur (4) Pergi dari game\n");
                            printf("\tPilih : "); scanf("%d", &aksi);
                            switch(aksi) {
                                case 1: temp1 = randomizer(lower_damage_multiplier, upper_damage_multiplier);
                                        temp2 = randomizer(5 * stage_boss->level, 12 * stage_boss->level);
                                        printf("\tKamu menyerang %s! %s -%d HP!\n", stage_boss_names[stage % 5], stage_boss_names[stage % 5], temp1);
                                        stage_boss->health -= temp1;
                                        if(stage_boss->health > 0) {
                                            printf("\t%s menyerangmu! %s -%d HP!\n", stage_boss_names[stage % 5], main_char->nama, temp2);
                                            main_char->health -= temp2;
                                            if(main_char->health < 0) {
                                                main_char->health = 0;
                                            }
                                        }
                                        break;
                                case 2: makan(main_char);
                                        if(main_char->health > max_hp) {
                                            main_char->health = max_hp;
                                        }
                                        break;
                                case 3: if(rand() % 3 == 0) {
                                            printf("\tAnda berhasil kabur!\n");
                                            kabur = 1;
                                        } else {
                                            printf("\tAnda gagal kabur!\n");
                                            temp = randomizer(4, 8);
                                            diserang(main_char, stage_boss, temp);
                                        }
                                        break;
                                case 4: play = 0;
                                        printf("\tAnda pergi dari game.\nGame selesai.\n");
                                        break;
                                default: printf("\tMohon masukkan angka sesuai dengan aksi yang diinginkan!\n");
                                        break;
                            }
                            if(kabur) {
                                break;
                            }
                            if(main_char->health <= 0) {
                                printf("\nGAME OVER!\nKamu telah mati.\n");
                                play = 0;
                                mati = 1;
                                break;
                            }
                            if(stage_boss->health <= 0) {
                                int coin_gain = randomizer(500, 750) * stage_boss->level;
                                printf("\t%s telah mati!\n", stage_boss_names[stage % 5]);
                                printf("\tAnda mendapatkan %d koin!\n", coin_gain);
                                printf("\t+%d XP!\n", stage_boss->level * stage_boss_xp_multiplier);
                                if(senjata_yang_dipunyai < 20) {
                                        printf("\t%s menjatuhkan %s!\n\tAnda mendapatkan %s!\n", stage_boss_names[stage % 5], drop_senjata[stage-1], drop_senjata[stage-1]);
                                        main_char->senjata[senjata_yang_dipunyai] = drop_senjata[stage-1];
                                        senjata_yang_dipunyai += 1;
                                } else {
                                        printf("\t%s menjatuhkan %s!\n\tTas anda penuh, anda tidak bisa mengambil %s\n", stage_boss->nama, drop_senjata[stage-1], drop_senjata[stage-1]);
                                }
                                if(makanan_yang_dipunyai < 20) {
                                    printf("\t%s menjatuhkan %s!\n\tAnda mendapatkan %s!\n", stage_boss->nama, salve->nama_makanan, salve->nama_makanan);
                                    main_char->makanan[makanan_yang_dipunyai] = salve;
                                    makanan_yang_dipunyai += 1;
                                } else {
                                    printf("\t%s menjatuhkan %s!\n\tTas anda penuh, anda tidak bisa mengambil %s\n", stage_boss->nama, salve->nama_makanan, salve->nama_makanan);
                                }
                                main_char->xp += stage_boss->level * stage_boss_xp_multiplier;
                                main_char->coin += coin_gain;
                                if(main_char->xp / req_xp >= 1) {
                                        main_char->level += main_char->xp / req_xp;
                                        main_char->xp = main_char->xp % req_xp;
                                        printf("\tAnda telah naik level!\n");
                                }
                                loop_count++;
                                next_stage = 1;
                            }
                        }
                        break;
                    case 2: break;
            }
        } 
    }

    if(mati) {
        printf("\nTekan tombol apa saja untuk keluar dari program.\n");
        getch();
    }

    return 0;
}

