#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>

#define LIGNES 10
#define COLONNES 10

#ifndef NB_THREADS
#define NB_THREADS 8 // Nombre maximum de threads à créer
#endif 

char chemin_visite[LIGNES][COLONNES];
char **Grille;
char **Chemin;
int fin =0;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;


void lire_fichier(int* entree_x, int* entree_y, int* sortie_x, int* sortie_y) {
    system("python3 generateur.py 10 10 > fichier.txt");
    FILE* file = fopen("fichier.txt", "r");

    if (file == NULL) {
        printf("Erreur d'ouverture du fichier !\n");
        return;
    }

    /* Lecture du fichier ligne par ligne  */
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            char str = fgetc(file);
            /*Si la fin de la ligne est atteinte, sortir de la boucle interne */
            if (str == '\n') {
                j--;
            } 

            /* Allouer de la mémoire pour chaque ligne */ 
            if (j == 0) {
                Grille[i] = (char*) malloc(COLONNES * sizeof(char));
            }
            /* Stocker le caractère dans la matrice */ 
            Grille[i][j] = str;

            /* Si le caractère est un chiffre, vérifier s'il s'agit de la case de départ ou de la sortie */
            if (isdigit(str)) {
                int num = str - '0';
                if (num == 2) {
                    *entree_x = i;
                    *entree_y = j;
                } else if (num == 3) {
                    *sortie_x = i;
                    *sortie_y = j;
                }
            }
        }
    }

    fclose(file);
}

void* resolution_th(void* arg) {
    
    while(!fin){
    
        int* position = (int*) arg;
        int x = position[0];
        int y = position[1];
        pthread_mutex_lock(&mutex);

        if (x < 0 || x >= LIGNES || y < 0 || y >= COLONNES) {
            pthread_mutex_unlock(&mutex);
            return (NULL);
        }

        if (Grille[x][y] == '3') {
            printf("Jackpot en (%d, %d)\n", x, y);
            Chemin[x][y] ='3';
            fin =1;
            pthread_mutex_unlock(&mutex);
            return (NULL);
        }

        if (Grille[x][y] == '0' || chemin_visite[x][y]) {
            pthread_mutex_unlock(&mutex);
            return (NULL);
        }

        chemin_visite[x][y] = '1';
        Chemin[x][y] ='x';
        pthread_mutex_unlock(&mutex);

        pthread_t threads[4];
        int num_threads = 0; // Nombre de threads créés jusqu'à présent
        int positions[4][2] = {
            {x, y+1},
            {x, y-1},
            {x+1, y},
            {x-1, y}
        };
        for (int i = 0; i < 4; i++) {
            /* Si le nombre maximum de threads n'a pas été atteint, on crée un nouveau thread */
            if (num_threads < NB_THREADS) {
                pthread_create(&threads[num_threads], NULL, resolution_th, (void*) positions[i]);
                num_threads++;
            }
            /* Sinon, on attend qu'un thread existant termine avant d'en créer un nouveau */
            else {
                pthread_join(threads[0], NULL);
                /* On décale tous les éléments du tableau d'un cran vers la gauche */
                for (int j = 0; j < NB_THREADS-1; j++) {
                    threads[j] = threads[j+1];
                }
                /* On crée le nouveau thread */
                pthread_create(&threads[NB_THREADS-1], NULL, resolution_th, (void*) positions[i]);
            }
        }

        /* On attend la fin de tous les threads restants */
        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }

        return (NULL);
    }
}

void affiche_grille(char **laby) {
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            printf("%c ", laby[i][j]);
        }
        printf("\n");
    }
}
/**
*   ROUTINE D'ALLOCATION 
*/
char **alloue_grille(){
    char **tab = (char**) malloc(LIGNES * sizeof(char*));
    for (int i = 0; i < LIGNES; i++) {
        tab[i] = (char*) malloc(COLONNES * sizeof(char));
    }
    return tab;
}

/**
*Routine de dessallocation 
*/
void desalloue_grille(char **tab) {
    for (int i = 0; i < LIGNES; i++) {
        free(tab[i]);
    }
    free(tab);
}


int main(int argc, char *argv[]) {
    /* Coordonnées de depart et d'arrivée */
    int entree_x;
    int entree_y;
    int sortie_x;
    int sortie_y;
    
    /* Matrice affichant le chemin parcourue*/
    Chemin = alloue_grille();

    /* Allocation de la Grille - Le Labyrinthe */
  	Grille = alloue_grille();

    /* Initialisation du chemin */
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            Chemin[i][j] =' ';
        }
    }
    
    /* Generation & lecture de la matrice */
    lire_fichier(&entree_x, &entree_y, &sortie_x, &sortie_y);

    /* Affichage de la matrice */ 
	affiche_grille(Grille);
    int pos_depart[2] = {entree_x, entree_y};
   	
    resolution_th((void*) pos_depart);
    Chemin[entree_x][entree_y] = '2';
    
    affiche_grille(Chemin); //Affichage du chemin vers la solution

    /* Libération de la mémoire */
    desalloue_grille(Grille);
    desalloue_grille(Chemin);
    
    return 0;
}
