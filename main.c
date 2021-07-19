#include <stdio.h>
#include <stdlib.h>
#pragma GCC optimize ("O1")

typedef struct node{
    char type;
    short parent_x;
    short parent_y;
    short x;
    short y;
    short distance;
    short price;
}node;

node *heap;
int heap_size = 0;
int p_num = 0;
int celkova_dlzka = 0;

//metoda na odstranenie prvku z min haldy
void del_heapify(int i){
    int smallest = i;
    int left = 2*i + 1;
    int right = 2*i + 2;

    if (left < heap_size && heap[left].distance < heap[smallest].distance)
        smallest = left;

    if (right < heap_size && heap[right].distance < heap[smallest].distance)
        smallest = right;

    if (smallest != i) {
        node tmp;
        tmp = heap[i];
        heap[i] = heap[smallest];
        heap[smallest] = tmp;

        del_heapify(smallest);
    }
}

//metoda na upravenie min haldy, tak aby splnala poziadavky min haldy
void heapify(int i){
    int parent = (i - 1)/2;

    if(heap[parent].distance > 0){
        if(heap[i].distance < heap[parent].distance){
            node tmp;
            tmp = heap[parent];
            heap[parent] = heap[i];
            heap[i] = tmp;
            heapify(parent);
        }
    }
}

//vkladanie do min haldy
void heap_insert(node new){
    heap_size++;
    heap[heap_size-1] = new;
    heapify(heap_size-1);
}

//funkcia na vybratie prveho (najmensieho) prvku z min haldy
node heap_min(){
    node last = heap[heap_size-1];
    node smallest = heap[0];
    heap[0] = last;
    heap_size--;
    del_heapify(0);
    return smallest;
}

//metoda, ktora upravi vzdialenost od zaciatku resp. kluc prvku v halde
//nasledne upravi haldu pomocou heapify
void heap_decrease_key(node tmp, short new_key){
    int index = 0;

    for(int i = 0; i < heap_size; i++)
        if(heap[i].x == tmp.x && heap[i].y == tmp.y)
            index = i;

    heap[index].distance = new_key;

    heapify(index);
}

//funkcia na ziskavanie poctu policok, ktore prejdem
int get_num_of_nodes(node **map, int ciel_x, int ciel_y, int start_x, int start_y){
    int x = ciel_x;
    int y = ciel_y;
    int pom, num = 1;

    while(map[x][y].parent_x != map[start_x][start_y].parent_x || map[x][y].parent_y != map[start_x][start_y].parent_y){
        num++;
        pom = x;
        x = map[x][y].parent_x;
        y = map[pom][y].parent_y;
    }

    return num;
}

//funkcia, pomocou ktorej ziskavam cestu k danemu cielu
//argumentami su suradnice cieloveho policka a pocet policok, ktore chcem prejst
//f vracia pole, v ktorom su suradnice policok, ktore som presiel
int *get_path(node **map, int ciel_x, int ciel_y, int num){
    int x = ciel_x;
    int y = ciel_y;
    int pom;

    int *path = malloc(num * 2 * sizeof(int));

    for(int i = num * 2-1 ; i+1 > 0; i-=2){
        path[i] = map[x][y].x;
        path[i-1] = map[x][y].y;
        pom = x;
        x = map[x][y].parent_x;
        y = map[pom][y].parent_y;
    }
    path[num*2-1] = map[ciel_x][ciel_y].x;
    path[num*2-2] = map[ciel_x][ciel_y].y;

    return path;
}

//funckia na ziskanie ceny cesty, ktoru prechadzam
int get_path_price(node **map, int ciel_x, int ciel_y, int num){
    int path_price = 0;
    int x = ciel_x;
    int y = ciel_y;
    int pom;

    for(int i = num-1 ; i > 0; i--){
        path_price += map[x][y].price;
        pom = x;
        x = map[x][y].parent_x;
        y = map[pom][y].parent_y;
    }

    return path_price;
}

void update_neighbor(node **map, short x, short y, short par_x, short par_y, short new_dist){
    map[x][y].distance = new_dist;
    map[x][y].parent_x = par_x;
    map[x][y].parent_y = par_y;
}

//metoda, ktora kazdemu policku urci ako je vzdialene od daneho zaciatku
//a nastavi mu predchodcu
void dijkstra(node **map, int n, int m, int start_x, int start_y){
    //najskor nastavim predchodcov vsetkych policok na -1
    //a tiez vzdialenost od stredu na 10000
    //nasledne prvok vlozim do min haldy
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            map[i][j].parent_x = -1;
            map[i][j].parent_y = -1;
            if(i == start_x && j == start_y)
                map[i][j].distance = 0;
            else
                map[i][j].distance = 10000;
            heap_insert(map[i][j]);
        }
    }
    short alt;
    //pre kazdy prvok z haldy pozeram ci ma susedov
    //a nasledne danemu susedovi upravim predchodcu na akt. prvok
    //taktiez mu upravim vzdialenost od zaciatku a upravim haldu
    while(heap_size != 0){
        node min = heap_min();
        if(min.x > 0 && map[min.x - 1][min.y].distance == 10000){
            alt = min.distance + map[min.x - 1][min.y].price;
            if(alt < map[min.x - 1][min.y].distance){
                update_neighbor(map, min.x-1, min.y, min.x, min.y, alt);
                heap_decrease_key(map[min.x - 1][min.y], alt);
            }
        }
        if(min.x < n-1 && map[min.x + 1][min.y].distance == 10000){
            alt = min.distance + map[min.x +1][min.y].price;
            if(alt < map[min.x + 1][min.y].distance){
                update_neighbor(map, min.x+1, min.y, min.x, min.y, alt);
                heap_decrease_key(map[min.x + 1][min.y], alt);
            }
        }
        if(min.y > 0 && map[min.x][min.y-1].distance == 10000){
            alt = min.distance + map[min.x][min.y-1].price;
            if(alt < map[min.x][min.y-1].distance){
                update_neighbor(map, min.x, min.y-1, min.x, min.y, alt);
                heap_decrease_key(map[min.x][min.y-1], alt);
            }

        }
        if(min.y < m-1 && map[min.x][min.y+1].distance == 10000){
            alt = min.distance + map[min.x][min.y+1].price;
            if(alt < map[min.x][min.y+1].distance){
                update_neighbor(map, min.x, min.y+1, min.x, min.y, alt);
                heap_decrease_key(map[min.x][min.y+1], alt);
            }
        }
    }
}

//funckia, ktora vrati suradnice draka
int *najdi_draka(node **map, int n, int m){
    int *drak = malloc(2 * sizeof(int));

    for(int i = 0; i < n; i++)
        for(int j = 0; j < m; j++)
            if(map[i][j].type == 'D'){
                drak[0] = i;
                drak[1] = j;
            }

    return drak;
}

//metoda, ktora zisti pocet princezien
void pocet_princezien(node **map, int n, int m){
    for(int i = 0; i < n; i++)
        for(int j = 0; j < m; j++)
            if(map[i][j].type == 'P')
                p_num++;
}

//funkcia, ktora vrati pole so suradnicami princezien
int *najdi_princezny(node **map, int n, int m){
    int *princezny = malloc(p_num*2 * sizeof(int));
    int x = 0;

    for(int i = 0; i < 6; i++)
        princezny[i] = -1;

    for(int i = 0; i < n; i++)
        for(int j = 0; j < m; j++)
            if(map[i][j].type == 'P' && princezny[x] == -1){
                princezny[x] = i;
                princezny[x+1] = j;
                x += 2;
            }

    return princezny;
}

//funckia na zistovanie ceny daneho policka
short get_price(char c){
    if(c == 'H')
        return 2;
    else if(c == 'N')
        return -1;
    else
        return 1;
}

//funckia na vytvorenie mapy struct-ov z mapy char-ov
node **create_map(char **mapa, short n, short m){
    node **map = (node**)malloc(n * sizeof(node*));

    for (int i = 0; i < n; i++)
        map[i] = (node*)malloc(m * sizeof(node));

    //pre kazde policko nastavujem jeho cenu, typ a suradnice
    for(short i = 0; i < n; i++)
        for(short j = 0; j < m; j++){
            map[i][j].price = get_price(mapa[i][j]);
            map[i][j].type = mapa[i][j];
            map[i][j].x = i;
            map[i][j].y = j;
        }
    map[0][0].distance = 0;
    return map;
}

int get_min(int a, int b){
    if(a < b)
        return a;
    return b;
}

//funckia, ktora zisti ceny najkratsich ciest medzi princeznami
//tieto ceny ulozi do pola, ktore vrati
int *get_min_prices(node **map, short n, short m, int *princezny){
    int pr_dlzka, path_price;
    int end_x = 0, end_y = 0;
    int *min_prices = malloc(p_num*(p_num-1) * sizeof(int));
    int poc = 0;
    for(int i = 0; i < p_num*2; i+=2){
        dijkstra(map, n, m, princezny[i], princezny[i+1]);
        end_x = 0;
        end_y = 1;
        for(int j = 0; j < p_num-1; j++){
            if(end_x == i){
                end_x += 2;
                end_y += 2;
            }
            pr_dlzka = get_num_of_nodes(map, princezny[end_x], princezny[end_y], princezny[i], princezny[i+1]);
            path_price = get_path_price(map, princezny[end_x], princezny[end_y], pr_dlzka);
            min_prices[poc] = path_price;
            poc++;
            end_x += 2;
            end_y += 2;
        }
    }
    return min_prices;
}

int *get_pr_path(node **map, int n, int m, int *princezny, int price, int *end_x, int *dlzka){
    int x = 0, pr_dlzka, path_price;
    int *pr_path;
    for(int i = 0; i < p_num-1; i++){
        if(x == *end_x)
            x += 2;
        pr_dlzka = get_num_of_nodes(map, princezny[x], princezny[x+1], princezny[*end_x], princezny[*end_x+1]);
        path_price = get_path_price(map, princezny[x], princezny[x+1], pr_dlzka);
        if(price == path_price){
            celkova_dlzka += pr_dlzka;
            pr_path = get_path(map, princezny[x], princezny[x+1], pr_dlzka);
            *dlzka = pr_dlzka;
            *end_x = x;
            dijkstra(map, n, m, princezny[x], princezny[x+1]);
            return pr_path;
        }
        x += 2;
    }
    return 0;
}

//funkica, ktora vrati pole suradnic, resp. najkratsiu cestu
int *zachran_princezne(char **mapa, short n, short m, int t, int *dlzka_cesty){
    node **map = create_map(mapa, n, m);
    heap = malloc(n*m * sizeof(node));
    int path_price = 0;
    int pr_dlzka = 0;
    int poc, end_x = 0;

    int *drak = najdi_draka(map, n, m);

    dijkstra(map, n, m, 0, 0);

    //hladam najkratsiu cestu k drakovi
    int drak_dlzka = get_num_of_nodes(map, drak[0], drak[1], 0, 0);
    celkova_dlzka += drak_dlzka;
    int *drak_cesta = get_path(map, drak[0], drak[1], drak_dlzka);

    pocet_princezien(map, n, m);
    int *princezny = najdi_princezny(map, n, m);

    int *min_prices = get_min_prices(map, n, m, princezny);

    //idem zistovat ceny najkratsich ciest od draka, ku vsetkym princeznam
    dijkstra(map, n, m, drak[0], drak[1]);

    int min, num = 0;
    int celkova_cena = 10000;
    int sum;
    int *path_prices = malloc(p_num * sizeof(int));
    for(int i = 0; i < p_num*2; i+=2){
        pr_dlzka = get_num_of_nodes(map, princezny[i], princezny[i+1], drak[0], drak[1]);
        path_price = get_path_price(map, princezny[i], princezny[i+1], pr_dlzka);
        min = get_min(min_prices[i], min_prices[i+1]);
        for(int j = 0; j < p_num*2; j++)
            if(min_prices[j] != min_prices[i] && min_prices[j] != min_prices[i+1])
                num = min_prices[j];
        sum = path_price + min + num;
        if(sum < celkova_cena){
            celkova_cena = sum;
            path_prices[0] = path_price;
            path_prices[1] = min;
            path_prices[2] = num;
        }
    }

    int *cesta = malloc(celkova_cena*2 * sizeof(int));

    //do vysledneho pola suradnic nacitam cestu k drakovi
    for(int i = 0; i < drak_dlzka*2; i++){
        cesta[i] = drak_cesta[i];
    }

    //idem zistovat najkratsiu cestu od draka k prvej princeznej
    //(nie k prvej najblizsej, ale k tej ktora je najvyhodnejsia celkovo)
    dijkstra(map, n, m, drak[0], drak[1]);

    int *pr_path;
    for(int i = 0; i < p_num*2; i+=2){
        pr_dlzka = get_num_of_nodes(map, princezny[i], princezny[i+1], drak[0], drak[1]);
        path_price = get_path_price(map, princezny[i], princezny[i+1], pr_dlzka);
        if(path_prices[0] == path_price){
            celkova_dlzka += pr_dlzka;
            pr_path = get_path(map, princezny[i], princezny[i+1], pr_dlzka);
            dijkstra(map, n, m, princezny[i], princezny[i+1]);
            end_x = i;
            break;
        }
    }
    //"zapisem" cestu od draka k prvej princeznej do celkovej cesty
    poc = 2;
    for(int i = drak_dlzka*2; i < drak_dlzka*2 + pr_dlzka*2-2; i++){
        cesta[i] = pr_path[poc];
        poc++;
    }
    int dlzka = drak_dlzka*2 + pr_dlzka*2-2;

    for(int i = 1; i < p_num; i++){
        poc = 2;
        pr_path = get_pr_path(map, n, m, princezny, path_prices[i], &end_x, &pr_dlzka);
        for(int j = dlzka; j < dlzka + pr_dlzka*2-2; j++){
            cesta[j] = pr_path[poc];
            poc++;
        }
        dlzka += pr_dlzka*2-2;
    }

    *dlzka_cesty = celkova_dlzka - p_num;
    return cesta;
}

int main() {
    short n = 7;
    short m = 7;

    char char_map[7][7] = {{'C', 'C', 'C', 'H', 'H', 'C', 'H'},
                           {'C', 'C', 'H', 'H', 'H', 'C', 'H'},
                           {'H', 'C', 'D', 'C', 'H', 'H', 'C'},
                           {'H', 'H', 'H', 'C', 'H', 'H', 'C'},
                           {'H', 'C', 'H', 'H', 'C', 'C', 'C'},
                           {'H', 'C', 'H', 'H', 'P', 'H', 'H'},
                           {'P', 'C', 'H', 'C', 'H', 'H', 'P'}};

    char **map = (char**)malloc(n * sizeof(char*));

    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            map[i] = (char*)malloc(sizeof(char));
            printf("%c ", char_map[i][j]);
        }
        printf("\n");
    }
    printf("xxxxxxxxxxxxxxxxxxx\n");
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            map[i][j] = char_map[i][j];
        }
    }

    int dlzka_cesty[50];

    int *cesta = zachran_princezne(map, n, m, 500, dlzka_cesty);

    return 0;
}