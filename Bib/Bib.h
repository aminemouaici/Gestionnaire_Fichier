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

/** 
 * @struct Superbloc
 * @brief Structure représentant le superbloc du système de fichiers.
 */
typedef struct {
    int taille_fs;    //taille de la partition
    int nb_inodes;    //nombre maximal d'inodes dans la partition 
    int nb_blocs;     //nombre maximal de blocs dans la partition 
    int inode_libres;  //nombre d'inode non utilisés dans la partiton 
    int blocs_libres;   //nombre d'inode non utilisés dans la partiton 
    int premier_bloc_libre;   // numéro du premier bloc libre
} Superbloc;

/** 
 * @struct Bitmap
 * @brief Structure représentant le bitmap pour les inodes et les blocs.
 */
typedef struct {
    unsigned char inodes[(MAX_INODES + 7) / 8];  // Aligné sur 8
    unsigned char blocs[(MAX_BLOCKS + 7) / 8];   // Aligné sur 8
} Bitmap;


// Définit une macro pour activer (mettre à 1) le bit à la position "pos" dans la variable "byte"
#define BIT_SET(byte, pos) (byte |= (1 << pos))
// - (1 << pos) décale le bit "1" vers la gauche de "pos" positions (ex. : pos = 2 donne 00000100)
// - |= effectue un OU binaire avec le byte d'origine → cela place le bit à 1 sans toucher aux autres

// Définit une macro pour désactiver (mettre à 0) le bit à la position "pos" dans la variable "byte"
#define BIT_CLEAR(byte, pos) (byte &= ~(1 << pos))
// - (1 << pos) crée un masque avec un 1 à la position souhaitée
// - ~ inverse tous les bits (donc un 0 à "pos", 1 ailleurs)
// - &= applique ce masque en conservant les bits sauf celui à "pos" qui est mis à 0

// Définit une macro pour tester si le bit à la position "pos" est à 1 dans "byte"
#define BIT_CHECK(byte, pos) (byte & (1 << pos))
// - (1 << pos) crée un masque avec un 1 à la position "pos"
// - & vérifie si ce bit est activé → renvoie une valeur non nulle si le bit est à 1, 0 sinon


/** 
 * @struct Inode
 * @brief Structure représentant un inode dans le système de fichiers.
 */
 typedef struct {
    int id;                          // Identifiant unique de l'inode (permet de l’identifier dans la table des inodes)
    int taille;                      // Taille du fichier (en octets), ou nombre d’entrées s’il s’agit d’un répertoire
    int est_repertoire;             // Booléen : 1 si c’est un répertoire, 0 si c’est un fichier classique
    int permissions;                // Permissions d’accès (lecture, écriture, exécution) codées en binaire (type UNIX)
    int liens;                      // Nombre de liens physiques (hard links) pointant vers cet inode
    int est_lien;                   // Booléen : 1 si c’est un lien symbolique, 0 sinon
    int blocs[NUM_DIRECT_BLOCKS];   // Tableau de blocs de données directs (stockage direct des données du fichier)
    int indirect_block;             // Adresse d’un bloc indirect (pointeur vers d’autres blocs si les blocs directs sont pleins)
    int inode_pere;                 // ID de l’inode parent (utile pour la navigation dans la hiérarchie)
    time_t date_creation;           // Date de création de l’inode (format time_t)
    time_t date_modification;       // Date de la dernière modification des données associées à cet inode
} Inode;


/** 
 * @struct Repertoire
 * @brief Structure représentant un répertoire dans le système de fichiers.
 */
typedef struct {
    int inode_id;
    struct {
        char nom[NAME_SIZE];
        int inode_id;
    } fichiers[MAX_FILES];
    int nb_fichiers;
} Repertoire;

/** 
 * @struct FichierOuvert
 * @brief Structure représentant un fichier ouvert.
 */
typedef struct {
    int inode_id;
    int offset;
    int mode;
} FichierOuvert;

/** 
 * @struct LienSymbolique
 * @brief Structure représentant un lien symbolique dans le système de fichiers.
 */
typedef struct {
    int inode_id;
    char cible[MAX_PATH_LENGTH];
} LienSymbolique;

/** 
 * @struct SystemeFichier
 * @brief Structure représentant le système de fichiers complet.
 */
typedef struct {
    Superbloc superbloc;
    Bitmap bitmap;
    Inode inodes[MAX_INODES];
    Repertoire racine;
    char data[PARTITION_SIZE];  // Zone de stockage des blocs de données
    int repertoire_courant; // Inode du répertoire courant
} SystemeFichier;


/****************************Fonctions****************************/
/**
 * @brief Appels des fonctions définies dans `bib.c`.
 * 
 * Ce fichier contient la déclaration des fonctions qui seront utilisées dans
 * `bib.c` pour gérer et manipuler le système de fichiers.
 * 
 * - `allouer_inode` : Alloue un inode dans le système de fichiers.
 * - `allouer_bloc` : Alloue un bloc de données dans le système de fichiers.
 * - `initialiser_systeme_fichier` : Initialise le système de fichiers.
 * - `sauvegarder_systeme_fichier` : Sauvegarde l'état actuel du système de fichiers.
 * - `charger_systeme_fichier` : Charge le système de fichiers depuis le fichier de partition.
 * - `trouver_inode_par_chemin` : Trouve l'inode associé à un chemin donné.
 * - `create_file_rep` : Crée un fichier ou un répertoire dans le système de fichiers.
 * - `cd` : Change le répertoire courant dans le système de fichiers.
 * - `droit` : Modifie les droits d'accès d'un fichier ou répertoire.
 * - `afficher_ls_chemin` : Affiche le contenu du répertoire à un chemin donné.
 * - `supprimer_fichier` : Supprime un fichier du système de fichiers.
 * - `supprimer_repertoire` : Supprime un répertoire du système de fichiers.
 * - `verifier_permissions` : Vérifie si un inode a les permissions adéquates.
 * - `open_file` : Ouvre un fichier en lecture ou écriture.
 * - `close_file` : Ferme un fichier ouvert.
 * - `ecrire_fichier` : Écrit dans un fichier.
 * - `lire_fichier` : Lit un fichier.
 * - `creer_lien_symbolique` : Crée un lien symbolique.
 * - `resoudre_lien_symbolique` : Résout un lien symbolique pour obtenir le chemin réel.
 * - `afficher_cible_lien_symbolique` : Affiche la cible d'un lien symbolique.
 * - `mv` : Déplace ou renomme un fichier ou un répertoire.
 * - `cp` : Copie un fichier ou un répertoire.
 * - `pwd` : Affiche le chemin du répertoire courant.
 * - `creer_lien_hard` : Crée un lien dur entre deux fichiers.
 * 
 * @note Les fonctions ci-dessus sont implémentées dans le fichier `bib.c` et manipulées tout au long du projet pour gérer le système de fichiers.
 */
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
