#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>

#define LIGNES 17
#define COLONNES 17

char chemin_visite[LIGNES][COLONNES];
char **Grille;


void lire_fichier(int* entree_x, int* entree_y, int* sortie_x, int* sortie_y) {
    system("python3 generateur.py 17 17 > fichier.txt");
    FILE* file = fopen("fichier.txt", "r");

    if (file == NULL) {
        printf("Erreur d'ouverture du fichier.\n");
        return;
    }

    // Lecture du fichier ligne par ligne 
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            char c = fgetc(file);
            // Si la fin de la ligne est atteinte, sortir de la boucle interne
            if (c == '\n') {
                j--;
            } 

            /* Allouer de la mémoire pour chaque ligne */ 
            if (j == 0) {
                Grille[i] = (char*) malloc(COLONNES * sizeof(char));
            }
            /* Stocker le caractère dans la matrice */ 
            Grille[i][j] = c;

            /* Si le caractère est un chiffre, vérifier s'il s'agit de la case de départ ou de la sortie */
            if (isdigit(c)) {
                int num = c - '0';
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
    int* position = (int*) arg;
    int x = position[0];
    int y = position[1];

    if (x < 0 || x >= LIGNES || y < 0 || y >= COLONNES) {
        return NULL;
    }

    if (Grille[x][y] == '3') {
        printf("Jackpot la solution est en (%d, %d)\n", x, y);
        return NULL;
    }

    if (Grille[x][y] == '0' || chemin_visite[x][y]) {
        return NULL;
    }

    chemin_visite[x][y] = '1';

    pthread_t threads[4];
    int positions[4][2] = {
        {x, y+1},
        {x, y-1},
        {x+1, y},
        {x-1, y}
    };
	/* Creation des Threads */
    for (int i = 0; i < 4; i++) {
        pthread_create(&threads[i], NULL, resolution_th, (void*) positions[i]);
    }

    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    return (NULL);
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

void affiche_grille(char **laby) {
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            printf("%c ", laby[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    /* Coordonnées de depart et d'arrivée */
    int entree_x;
    int entree_y;
    int sortie_x;
    int sortie_y;

    /* Allocation de la matrice */
  	Grille = alloue_grille();

    /* Generation & lecture de la matrice */
    lire_fichier(&entree_x, &entree_y, &sortie_x, &sortie_y);

    /* Affichage de la matrice */ 
	affiche_grille(Grille);
    int pos_depart[2] = {entree_x, entree_y};
   	
    resolution_th((void*) pos_depart);

    /* Libération de la mémoire */
    desalloue_grille(Grille);

    
    return 0;
}
