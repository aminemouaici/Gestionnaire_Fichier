#ifndef BIB_H_INCLUDED
#define BIB_H_INCLUDED

/**************************inclusions*******************************/
/** 
 * @file bib.h
 * @brief Déclarations des structures et des fonctions pour la gestion d'un système de fichiers.
 * 
 * Ce fichier définit les structures nécessaires à l'implémentation d'un système de fichiers de type Unix.
 * Il contient également les prototypes des fonctions permettant la gestion des fichiers, répertoires, et liens symboliques.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

/**************************definitions******************************/
/** 
 * @def MAX_INODES
 * @brief Nombre maximal d'inodes dans le système de fichiers.
 */
#define MAX_INODES 100   // Nombre maximal d'inodes
/** 
 * @def MAX_BLOCKS
 * @brief Nombre maximal de blocs de données dans le système de fichiers.
 */
#define MAX_BLOCKS 1024  // Nombre maximal de blocs de données
/** 
 * @def BLOCK_SIZE
 * @brief Taille d'un bloc en octets.
 */
#define BLOCK_SIZE 4096  // Taille d'un bloc en octets
/** 
 * @def MAX_LINKS
 * @brief Nombre maximal de liens physiques pour un fichier.
 */
#define MAX_LINKS 10  // Nombre maximal de liens physiques
/** 
 * @def NUM_DIRECT_BLOCKS
 * @brief Nombre de blocs directs par inode (comme dans Unix).
 */
#define NUM_DIRECT_BLOCKS 12  // Nombre de blocs directs par inode (comme dans Unix)
/** 
 * @def INDIRECT_BLOCKS
 * @brief Nombre de blocs indirects par inode.
 */
#define INDIRECT_BLOCKS 1     // Nombre de blocs indirects par inode
/** 
 * @def MAX_FILES
 * @brief Nombre maximal de fichiers dans un répertoire.
 */
#define MAX_FILES 32  // Nombre max de fichiers dans un répertoire
/** 
 * @def NAME_SIZE
 * @brief Taille maximale d'un nom de fichier.
 */
#define NAME_SIZE 32  // Taille max d'un nom de fichier
/** 
 * @def MAX_PATH_LENGTH
 * @brief Longueur maximale du chemin d'un lien symbolique.
 */
#define MAX_PATH_LENGTH 256 // Longueur max du chemin du lien
/** 
 * @def MAX_OPEN_FILES
 * @brief Nombre maximal de fichiers ouverts simultanément.
 */
#define MAX_OPEN_FILES 16  // Nombre maximal de fichiers ouverts
/** 
 * @def Max_SIZE_PARAM
 * @brief Taille maximale d'un paramètre pour certaines fonctions.
 */
#define Max_SIZE_PARAM 500  //taille maximal d'un paramétre
/** 
 * @def PARTITION_NAME
 * @brief Nom du fichier représentant la partition.
 */
#define PARTITION_NAME "mnt.fs" // Nom du fichier de la partition
/** 
 * @def PARTITION_SIZE
 * @brief Taille allouée à la partition (un quart de la taille totale).
 */
#define PARTITION_SIZE (MAX_BLOCKS / 4 * BLOCK_SIZE) // Taille allouée à la partition
/** 
 * @def MODE_READ
 * @brief Mode de lecture pour les fichiers ouverts.
 */
#define MODE_READ 0
/** 
 * @def MODE_WRITE
 * @brief Mode d'écriture pour les fichiers ouverts.
 */
#define MODE_WRITE 1

/***********************structures_definitions**********************/

// Définition du Superbloc
typedef struct {
    int taille_fs;
    int nb_inodes;
    int nb_blocs;
    int inode_libres;
    int blocs_libres;
    int premier_bloc_libre;
} Superbloc;

// Définition du Bitmap
typedef struct {
    unsigned char inodes[(MAX_INODES + 7) / 8];  // Aligné sur 8
    unsigned char blocs[(MAX_BLOCKS + 7) / 8];   // Aligné sur 8
} Bitmap;

#define BIT_SET(byte, pos) (byte |= (1 << pos))
#define BIT_CLEAR(byte, pos) (byte &= ~(1 << pos))
#define BIT_CHECK(byte, pos) (byte & (1 << pos))

// Définition de l'Inode
typedef struct {
    int id;
    int taille;
    int est_repertoire;
    int permissions;
    int liens;
    int est_lien;
    int blocs[NUM_DIRECT_BLOCKS];  // Blocs directs
    int indirect_block;      // Bloc indirect
    int inode_pere;                // Référence à l'inode parent
    time_t date_creation;
    time_t date_modification;
} Inode;

// Définition du Répertoire
typedef struct {
    int inode_id;
    struct {
        char nom[NAME_SIZE];
        int inode_id;
    } fichiers[MAX_FILES];
    int nb_fichiers;
} Repertoire;

// Définition du Fichier Ouvert
typedef struct {
    int inode_id;
    int offset;
    int mode;
} FichierOuvert;

// Définition du Lien Symbolique
typedef struct {
    int inode_id;
    char cible[MAX_PATH_LENGTH];
} LienSymbolique;

// Structure principale du Système de Fichiers
typedef struct {
    Superbloc superbloc;
    Bitmap bitmap;
    Inode inodes[MAX_INODES];
    Repertoire racine;
    char data[PARTITION_SIZE];  // Zone de stockage des blocs de données
    int repertoire_courant; // Inode du répertoire courant
} SystemeFichier;


/****************************Fonctions****************************/
int allouer_inode(SystemeFichier *fs);
int allouer_bloc(SystemeFichier *fs);
void initialiser_systeme_fichier(SystemeFichier *fs);
void sauvegarder_systeme_fichier(SystemeFichier *fs);
int charger_systeme_fichier(SystemeFichier *fs);
int trouver_inode_par_chemin(SystemeFichier *fs, const char *chemin);
void create_file_rep(SystemeFichier *fs, const char *chemin, int est_repertoire);
int trouver_inode_par_cheminCd(SystemeFichier *fs, const char *chemin);
void cd(SystemeFichier *fs, const char *chemin);
void droit(SystemeFichier *fs, const char *chemin, const char *droits) ;
void afficher_ls_chemin(SystemeFichier *fs, const char *chemin);
void supprimer_fichier(SystemeFichier *fs, const char *chemin);
void supprimer_repertoire(SystemeFichier *fs, const char *chemin);
int verifier_permissions(Inode *inode, int mode);
int open_file(SystemeFichier *fs, const char *chemin, int mode);
void close_file(SystemeFichier *fs, int descripteur);
void ecrire_fichier(SystemeFichier *fs, const char *chemin);
void lire_fichier(SystemeFichier *fs, const char *chemin);
void creer_lien_symbolique(SystemeFichier *fs, const char *chemin, const char *cible);
int resoudre_lien_symbolique(SystemeFichier *fs, const char *chemin);
void afficher_cible_lien_symbolique(SystemeFichier *fs, const char *chemin_lien);
int mv(SystemeFichier *fs, const char *source, const char *destination);
int cp(SystemeFichier *fs, const char *source, const char *destination);
void pwd(SystemeFichier *fs);
int creer_lien_hard(SystemeFichier *fs, const char *chemin_source, const char *chemin_cible) ;
//int lseek_file(SystemeFichier *fs, int descripteur, int offset);

#endif // BIB_H_INCLUDED
