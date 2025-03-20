#ifndef BIB_H_INCLUDED
#define BIB_H_INCLUDED

/**************************inclusions*******************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
//#include <windows.h>

/**************************definitions******************************/
#define TAILLE_PARTITION (10 * 1024 * 1024) // 10 Mo
#define TAILLE_BLOC 1024                     // 1 Ko par bloc
#define TAILLE_PAGE 256
#define NB_BLOCS (TAILLE_PARTITION / TAILLE_BLOC) // Nombre total de blocs


/***********************structures_definitions**********************/
typedef struct {
    uint32_t bloc_debut;  // Numéro du premier bloc libre
    uint32_t numero_page;      // Numéro du page au sein du bloc (0 à 3
    
} AdressePage;

typedef struct cellulePage{
    AdressePage page;
    struct cellulePage * suivant;
    struct cellulePage * precedent;
}*ListePage;

typedef struct { // Si c'est un fichier
    ListePage tablePage;
    uint32_t taille;
} fichier;

struct inode;
typedef  struct { // Si c'est un répertoire
    struct inode *inodes; // Liste des inodes contenus
    uint32_t nb_inodes;
} repertoire;

typedef struct inode {
    char nom[32];
    uint8_t type;        // 0 = fichier, 1 = répertoire
    uint8_t ID;
    uint8_t pére;              //ID du pére
    uint8_t permissions;
    union {
        fichier fichier;
        repertoire repertoire;
    } info;

} Inode;


typedef struct celluleInode{
    Inode inode;
    struct celluleInode * suivant;
}*ListeInode;


 

// Superbloc contenant la table des inodes et la liste des inodes libres
typedef struct {
    uint32_t taille_partition;
    uint32_t taille_bloc;
    uint32_t premier_bloc_donnees;
    ListeInode inodes;  // Liste des inodes
    ListePage listePageLibre;                                                      
} Superbloc;


/***********************Variales_Generales**************************/


#endif // BIB_H_INCLUDED
