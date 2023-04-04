#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#define LIGNES 17
#define COLONNES 17

char **Grille;
char visited[LIGNES][COLONNES]; 


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


int resolution_seq(int x, int y) {
    if (x < 0 || x >= LIGNES || y < 0 || y >= COLONNES) {
        return 0;
    }

    if (Grille[x][y] == '3') {
        printf("Solution trouvée en : (%d, %d)\n", x, y);
        return 1;
    }

    if (Grille[x][y] == '0' || visited[x][y]) {
        return 0;
    }

    visited[x][y] = '1';

    if (resolution_seq(x, y + 1)) {
        printf("(%d, %d)\n", x, y);
        return 1;
    }

    if (resolution_seq(x, y - 1)) {
        printf("(%d, %d)\n", x, y);
        return 1;
    }

    if (resolution_seq(x + 1, y)) {
        printf("(%d, %d)\n", x, y);
        return 1;
    }

    if (resolution_seq(x - 1, y)) {
        printf("(%d, %d)\n", x, y);
        return 1;
    }

    return 0;
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

void afficheLabyr(char **Grille) {
    // Affichage de la matrice
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
        	if(Grille[i][j] == '0'){
            printf(" ");
            }else if(Grille[i][j] == '1'){
            printf("|");
            }else if(Grille[i][j] == '2'){
            printf("O");
            }else{
            printf("X");
            }
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

    /*Affichage de la case de départ et de la sortie */
    printf("Coordonnées de départ: (%d, %d)\n", entree_x, entree_y);
    printf("Sortie: (%d, %d)\n", sortie_x, sortie_y);
    printf("\n");

    /* Affichage de la matrice */ 
	affiche_grille(Grille);

    if (!resolution_seq(entree_x, entree_y)) {
        printf("Chemin non trouvé !!\n");
    }

    
    afficheLabyr(Grille);
    printf("\n");

    /* Libération de la mémoire */
    desalloue_grille(Grille);
    return 0;
}
