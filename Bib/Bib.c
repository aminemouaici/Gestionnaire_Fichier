/*
   - Amine MOUAICI: 33.33%
   - Leticia ZAID: 33.33%
   - Samy Islem OULED-YOUNES 33.33%
*/

/**
 * @file Bib.c
 * @brief Code pour la gestion d'un système de fichiers avec allocation d'inodes et de blocs.
 */

#include "Bib.h"



/**
 * @brief Alloue un inode dans le système de fichiers.
 * 
 * Cette fonction recherche un inode libre dans le bitmap des inodes et l'alloue en mettant à jour 
 * le bitmap et le superbloc. Elle renvoie l'indice de l'inode alloué ou -1 si aucun inode libre n'est 
 * disponible.
 * 
 * @param fs Pointeur vers le système de fichiers.
 * @return L'indice de l'inode alloué, ou -1 si aucun inode n'est disponible.
 */
 int allouer_inode(SystemeFichier *fs) 
 {
     // Parcourt tous les inodes disponibles
     for (int i = 0; i < MAX_INODES; i++) {
         // Vérifie si l'inode à l'indice 'i' est libre (bit à 0)
         if (!BIT_CHECK(fs->bitmap.inodes[i / 8], i % 8)) {
             // Alloue l'inode en définissant son bit à 1 (inode utilisé)
             BIT_SET(fs->bitmap.inodes[i / 8], i % 8);
             
             // Décrémente le compteur des inodes libres dans le superbloc
             fs->superbloc.inode_libres--;
             
             // Retourne l'indice de l'inode nouvellement alloué
             return i;
         }
     }
 
     // Si aucun inode n'est libre, retourne -1 pour signaler l'absence d'inodes disponibles
     return -1; // Plus d'inodes disponibles
 }



 /*************************************************************************************************************************************** */


/**
 * @brief Alloue un bloc dans le système de fichiers.
 * 
 * Cette fonction recherche un bloc libre dans le bitmap des blocs et l'alloue en mettant à jour 
 * le bitmap et le superbloc. Elle renvoie l'indice du bloc alloué ou -1 si aucun bloc libre n'est 
 * disponible.
 * 
 * @param fs Pointeur vers le système de fichiers.
 * @return L'indice du bloc alloué, ou -1 si aucun bloc n'est disponible.
 */
 int allouer_bloc(SystemeFichier *fs) {
    // Parcourt tous les blocs disponibles
    for (int i = 0; i < MAX_BLOCKS; i++) {
        // Vérifie si le bloc à l'indice 'i' est libre (bit à 0)
        if (!BIT_CHECK(fs->bitmap.blocs[i / 8], i % 8)) {
            // Alloue le bloc en définissant son bit à 1 (bloc utilisé)
            BIT_SET(fs->bitmap.blocs[i / 8], i % 8);
            
            // Décrémente le compteur des blocs libres dans le superbloc
            fs->superbloc.blocs_libres--;
            
            // Retourne l'indice du bloc nouvellement alloué
            return i;
        }
    }

    // Si aucun bloc n'est libre, retourne -1 pour signaler l'absence de blocs disponibles
    return -1; // Plus de blocs disponibles
}


/****************************************************************************************************************************************** */


/**
 * @brief Initialise le système de fichiers.
 * 
 * Cette fonction initialise le système de fichiers en configurant les bitmaps, le superbloc, et 
 * en allouant les ressources nécessaires, y compris l'inode et le bloc pour le répertoire racine.
 * Elle crée également les entrées de répertoire pour le répertoire courant.
 * 
 * @param fs Pointeur vers le système de fichiers.
 */
 void initialiser_systeme_fichier(SystemeFichier *fs) {
    // Initialisation de la structure du système de fichiers à zéro
    memset(fs, 0, sizeof(SystemeFichier));

    // Initialisation des bitmaps à zéro (aucun inode ni bloc n'est alloué)
    memset(fs->bitmap.inodes, 0, sizeof(fs->bitmap.inodes));
    memset(fs->bitmap.blocs, 0, sizeof(fs->bitmap.blocs));

    // Initialisation des informations du superbloc
    fs->superbloc.taille_fs = PARTITION_SIZE; // Taille de la partition
    fs->superbloc.nb_inodes = MAX_INODES;    // Nombre maximal d'inodes
    fs->superbloc.nb_blocs = MAX_BLOCKS / 4; // Nombre maximal de blocs
    fs->superbloc.inode_libres = MAX_INODES; // Nombre d'inodes libres disponibles
    fs->superbloc.blocs_libres = MAX_BLOCKS / 4; // Nombre de blocs libres disponibles
    fs->superbloc.premier_bloc_libre = 0;    // Indice du premier bloc libre

    // Allouer un inode pour le répertoire racine
    int inode_racine = allouer_inode(fs);
    // Allouer un bloc pour le répertoire racine
    int bloc_racine = allouer_bloc(fs);

    // Vérification si l'allocation a échoué pour l'inode ou le bloc racine
    if (inode_racine == -1 || bloc_racine == -1) {
        printf("Erreur: Impossible d'allouer le répertoire racine\n");
        exit(EXIT_FAILURE);  // Terminer le programme si l'allocation échoue
    }

    // Initialisation de l'inode pour le répertoire racine
    fs->inodes[inode_racine].id = inode_racine;
    fs->inodes[inode_racine].taille = sizeof(Repertoire); // Taille de l'inode racine
    fs->inodes[inode_racine].est_repertoire = 1;          // Indique que c'est un répertoire
    fs->inodes[inode_racine].permissions = 0755;          // Permissions par défaut
    fs->inodes[inode_racine].liens = 1;                   // Nombre de liens pointant vers cet inode
    fs->inodes[inode_racine].blocs[0] = bloc_racine;     // Bloc du répertoire racine
    fs->inodes[inode_racine].date_creation = time(NULL); // Date de création
    fs->inodes[inode_racine].date_modification = fs->inodes[inode_racine].date_creation; // Date de modification
    fs->inodes[inode_racine].inode_pere = inode_racine;  // Référence à l'inode parent
    fs->inodes[inode_racine].est_lien = 0;                // Le répertoire racine n'est pas un lien symbolique

    // Initialisation de l'entrée pour le répertoire racine dans la structure Repertoire
    fs->racine.inode_id = inode_racine;

    // Initialisation du répertoire courant (répertoire racine au départ)
    fs->repertoire_courant = inode_racine;

    // Initialisation des fichiers dans le répertoire racine (entrées . et ..)
    fs->racine.nb_fichiers = 2;  // Les deux fichiers sont "." et ".."

    // Entrée "." représentant le répertoire courant
    strcpy(fs->racine.fichiers[0].nom, ".");
    fs->racine.fichiers[0].inode_id = inode_racine;  // L'inode du répertoire racine

    // Entrée ".." représentant le répertoire parent
    strcpy(fs->racine.fichiers[1].nom, "..");
    fs->racine.fichiers[1].inode_id = inode_racine;  // Le parent du répertoire racine est lui-même

    // Marquage de l'inode racine comme utilisé dans le bitmap des inodes
    BIT_SET(fs->bitmap.inodes[0 / 8], 0 % 8);  // Marquer le premier inode comme utilisé
    fs->superbloc.inode_libres--;  // Décrémenter le nombre d'inodes libres
}


/*************************************************************************************************************************************** */



/**
 * @brief Sauvegarde le système de fichiers dans un fichier.
 * 
 * Cette fonction sauvegarde l'état du système de fichiers dans un fichier binaire pour persister 
 * les données entre les exécutions du programme.
 * 
 * @param fs Pointeur vers le système de fichiers à sauvegarder.
 */
 void sauvegarder_systeme_fichier(SystemeFichier *fs) {
    // Ouverture du fichier en mode écriture binaire ("wb")
    FILE *f = fopen(PARTITION_NAME, "wb");
    
    // Vérification si le fichier a été ouvert correctement
    if (!f) {
        // Si l'ouverture échoue, afficher un message d'erreur et quitter le programme
        perror("Erreur lors de la sauvegarde de la partition");
        exit(EXIT_FAILURE); // Terminer l'exécution si l'ouverture du fichier échoue
    }

    // Écriture de l'intégralité de la structure du système de fichiers dans le fichier
    if (fwrite(fs, sizeof(SystemeFichier), 1, f) != 1) {
        // Si l'écriture échoue, afficher un message d'erreur et fermer le fichier
        perror("Erreur d'écriture du fichier système");
        fclose(f);  // Assurer que le fichier est fermé en cas d'erreur
        exit(EXIT_FAILURE); // Terminer l'exécution si l'écriture échoue
    }

    // Fermeture du fichier après l'écriture réussie
    fclose(f);
}



/********************************************************************************************************************************** */



/**
 * @brief Charge le système de fichiers depuis un fichier.
 * 
 * Cette fonction charge un système de fichiers sauvegardé à partir d'un fichier binaire. Si le fichier 
 * n'existe pas, elle retourne 0 et signale que le système de fichiers doit être initialisé.
 * 
 * @param fs Pointeur vers le système de fichiers à charger.
 * @return 1 si le système de fichiers a été chargé avec succès, 0 sinon.
 */
 int charger_systeme_fichier(SystemeFichier *fs) {
    // Ouverture du fichier en mode lecture binaire ("rb")
    FILE *f = fopen(PARTITION_NAME, "rb");
    
    // Vérification si le fichier a été ouvert correctement
    if (!f) {
        // Si le fichier n'existe pas, afficher un message et signaler qu'un nouveau système de fichiers doit être créé
        printf("Aucune partition existante, création d'un nouveau système de fichiers.\n");
        return 0; // Retourner 0 pour indiquer que le fichier n'a pas été trouvé et qu'une nouvelle création est nécessaire
    }
    
    // Lecture du contenu du fichier dans la structure du système de fichiers
    if (fread(fs, sizeof(SystemeFichier), 1, f) != 1) {
        // Si la lecture échoue, afficher une erreur et fermer le fichier
        perror("Erreur de lecture du fichier système");
        fclose(f);  // Fermer le fichier en cas d'erreur
        return 0;   // Retourner 0 pour indiquer une erreur de lecture
    }
    
    // Fermeture du fichier après lecture réussie
    fclose(f);

    // Afficher un message indiquant que le système de fichiers a été chargé avec succès
    printf("Système de fichiers chargé depuis %s\n", PARTITION_NAME);

    // Initialiser le répertoire courant à l'inode du répertoire racine
    fs->repertoire_courant = fs->racine.inode_id;

    // Retourner 1 pour indiquer que le système de fichiers a été chargé avec succès
    return 1;
}


/************************************************************************************************************************************** */


/**
 * Trouve l'inode correspondant à un chemin donné.
 * 
 * Cette fonction permet de trouver l'inode associé à un chemin dans le système de fichiers.
 * Elle prend en compte les répertoires et fichiers ainsi que les cas spéciaux "." (répertoire courant) et ".." (répertoire parent).
 * 
 * @param fs Le système de fichiers dans lequel la recherche doit être effectuée.
 * @param chemin Le chemin du fichier ou répertoire à trouver. Il peut être absolu ou relatif.
 * @return L'inode correspondant au chemin spécifié, ou -1 si le chemin est invalide.
 */
 int trouver_inode_par_chemin(SystemeFichier *fs, const char *chemin) {
    // Si le chemin est "/". Cela renvoie l'inode du répertoire racine.
    if (strcmp(chemin, "/") == 0) return fs->racine.inode_id;

    // Copie du chemin pour pouvoir le modifier sans affecter l'original.
    char chemin_cpy[MAX_PATH_LENGTH];
    strncpy(chemin_cpy, chemin, MAX_PATH_LENGTH);
    chemin_cpy[MAX_PATH_LENGTH - 1] = '\0'; // Sécurité pour éviter les dépassements de tampon

    // Initialisation du point de départ : si le chemin est absolu, commencer depuis la racine,
    // sinon à partir du répertoire courant.
    int inode_courant = (chemin[0] == '/') ? fs->racine.inode_id : fs->repertoire_courant;

    // Séparation du chemin en segments (nom de répertoire ou fichier).
    char *token = strtok(chemin_cpy, "/");
    while (token != NULL) {

        // Accéder à l'inode du répertoire courant.
        Inode *inode = &fs->inodes[inode_courant];

        // Vérification que l'inode actuel est un répertoire.
        if (!inode->est_repertoire) {
            // Si l'inode n'est pas un répertoire, afficher une erreur.
            printf("Erreur : %s n'est pas un répertoire.\n", token);
            return -1;  // Retourner -1 pour signaler une erreur
        }

        // Gestion des cas spéciaux "." et ".."
        if (strcmp(token, ".") == 0) {
            // Si le token est ".", rester dans le répertoire courant.
        } else if (strcmp(token, "..") == 0) {
            // Si le token est "..", se déplacer dans le répertoire parent.
            inode_courant = (inode_courant == fs->racine.inode_id) ? fs->racine.inode_id : inode->inode_pere;
        } else {
            // Recherche du fichier ou du répertoire spécifié dans le répertoire courant.
            Repertoire *rep = (Repertoire *)&fs->data[inode->blocs[0] * BLOCK_SIZE];
            int trouve = 0;
            for (int i = 0; i < rep->nb_fichiers; i++) {
                if (strcmp(rep->fichiers[i].nom, token) == 0) {
                    inode_courant = rep->fichiers[i].inode_id;
                    trouve = 1;
                    break;
                }
            }
            // Si l'élément spécifié n'est pas trouvé, afficher une erreur.
            if (!trouve) {
                printf("Erreur : %s introuvable dans le répertoire.\n", token);
                return -1;  // Retourner -1 pour signaler que le fichier ou répertoire n'a pas été trouvé.
            }
        }
        // Passer au prochain segment du chemin.
        token = strtok(NULL, "/");
    }

    // Retourner l'inode trouvé.
    return inode_courant;
}


/****************************************************************************************************************************************** */



/**
 * Crée un fichier ou un répertoire dans le système de fichiers.
 * 
 * Cette fonction crée un fichier ou un répertoire à un emplacement spécifié par un chemin. Si le chemin est relatif,
 * il est résolu à partir du répertoire courant. La fonction alloue un inode et un bloc pour le fichier ou répertoire,
 * puis ajoute ce dernier dans le répertoire parent.
 * 
 * @param fs Le système de fichiers dans lequel la création doit avoir lieu.
 * @param chemin Le chemin du fichier ou répertoire à créer.
 * @param est_repertoire Indicateur si le nouvel objet est un répertoire (1) ou un fichier (0).
 */

 void create_file_rep(SystemeFichier *fs, const char *chemin, int est_repertoire) {
    // Copier le chemin pour pouvoir le modifier sans affecter l'original
    char chemin_cpy[MAX_PATH_LENGTH];
    strncpy(chemin_cpy, chemin, MAX_PATH_LENGTH);

    // Séparer le chemin en répertoire parent et nom de l'objet
    char *dernier_slash = strrchr(chemin_cpy, '/');
    char nom[NAME_SIZE];
    const char *chemin_parent;

    if (dernier_slash) {
        // Le nom est après le dernier '/'
        strncpy(nom, dernier_slash + 1, NAME_SIZE);
        *dernier_slash = '\0';  // Terminer le chemin parent par un '\0'
        chemin_parent = (*chemin_cpy) ? chemin_cpy : "/";  // Le chemin parent est avant le '/'
    } else {
        // Si aucun '/' n'est trouvé, tout est dans le nom
        strncpy(nom, chemin, NAME_SIZE);
        chemin_parent = "";  // Pas de chemin parent
    }
    printf("Création de %s dans %s\n", nom, chemin_parent);

    // Trouver l'inode du répertoire parent à partir du chemin
    int inode_parent = trouver_inode_par_chemin(fs, chemin_parent);
    if (inode_parent == -1) {
        printf("Erreur : chemin parent %s introuvable.\n", chemin_parent);
        return;
    }
    printf("Inode parent: %d\n", inode_parent);

    // Accéder au répertoire parent
    Inode *inode_p = &fs->inodes[inode_parent];
    Repertoire *rep_p = (Repertoire *)&fs->data[inode_p->blocs[0] * BLOCK_SIZE];

    // Vérifier si le fichier ou répertoire existe déjà dans le répertoire parent
    for (int i = 0; i < rep_p->nb_fichiers; i++) {
        if (strcmp(rep_p->fichiers[i].nom, nom) == 0) {
            printf("Erreur : %s existe déjà.\n", nom);
            return;
        }
    }
    printf("Vérification de l'existence de %s dans %s\n", nom, chemin_parent);

    // Allouer un inode et un bloc pour le fichier ou répertoire
    int inode_fichier = allouer_inode(fs);
    int bloc_fichier = allouer_bloc(fs);
    if (inode_fichier == -1 || bloc_fichier == -1) {
        printf("Erreur : ressources insuffisantes pour créer %s\n", nom);
        return;
    }
    printf("Allocation inode: %d, bloc: %d\n", inode_fichier, bloc_fichier);

    // Initialiser le nouvel inode
    Inode *new_inode = &fs->inodes[inode_fichier];
    new_inode->id = inode_fichier;
    new_inode->taille = 0;  // Initialiser la taille à 0
    new_inode->est_repertoire = est_repertoire;  // Déterminer si c'est un répertoire ou un fichier
    new_inode->permissions = est_repertoire ? 0755 : 0777;  // Permissions (répertoire ou fichier)
    new_inode->liens = 1;  // Un seul lien au début
    new_inode->blocs[0] = bloc_fichier;
    new_inode->indirect_block= -1;
    for (int i = 1; i < NUM_DIRECT_BLOCKS; i++){
        new_inode->blocs[i] = -1;
    }
    new_inode->date_creation = time(NULL);  // Initialiser la date de création
    new_inode->date_modification = new_inode->date_creation;  // Initialiser la date de modification
    new_inode->inode_pere = inode_parent;  // L'inode parent du nouveau fichier ou répertoire
    new_inode->est_lien = 0;  // Indiquer que ce n'est pas un lien symbolique

    // Initialiser le contenu du répertoire si c'est un répertoire
    if (est_repertoire) {
        Repertoire *nouveau = (Repertoire *)&fs->data[bloc_fichier * BLOCK_SIZE];
        nouveau->inode_id = inode_fichier;
        nouveau->nb_fichiers = 0;  // Initialiser le nombre de fichiers à 0
    }

    // Ajouter le fichier ou répertoire dans le répertoire parent
    strncpy(rep_p->fichiers[rep_p->nb_fichiers].nom, nom, NAME_SIZE);
    rep_p->fichiers[rep_p->nb_fichiers].inode_id = inode_fichier;
    rep_p->nb_fichiers++;  // Augmenter le nombre de fichiers dans le répertoire

    // Afficher un message de confirmation
    printf("%s %s créé avec succès.\n", est_repertoire ? "Répertoire" : "Fichier", chemin);

    // Sauvegarder le système de fichiers après la création
    sauvegarder_systeme_fichier(fs);
}


/*********************************************************************************************************************************** */


/**
 * @brief Trouve l'inode correspondant à un chemin pour la commande "cd".
 * 
 * Cette fonction permet de trouver l'inode associé à un chemin dans le système de fichiers. Elle gère
 * les répertoires, ainsi que les cas spéciaux comme "." pour le répertoire courant et ".." pour le répertoire parent.
 * Elle retourne l'inode correspondant au chemin final ou -1 si le chemin est invalide.
 * 
 * @param fs Le système de fichiers dans lequel la recherche doit être effectuée.
 * @param chemin Le chemin du fichier ou répertoire à trouver. Il peut être absolu ou relatif.
 * @return L'inode correspondant au chemin spécifié, ou -1 si le chemin est invalide.
 */
 int trouver_inode_par_cheminCd(SystemeFichier *fs, const char *chemin) {
    if (!chemin || strlen(chemin) == 0) return -1;  // Si le chemin est vide ou invalide, retourner -1

    // Déterminer le point de départ (absolu ou relatif)
    int inode_courant = (chemin[0] == '/') ? fs->racine.inode_id : fs->repertoire_courant;
    // Si le chemin commence par '/', on commence à la racine, sinon on part du répertoire courant.

    // Copier le chemin pour le tokeniser (analyser par segments)
    char chemin_cpy[MAX_PATH_LENGTH];
    strncpy(chemin_cpy, chemin, MAX_PATH_LENGTH - 1);
    chemin_cpy[MAX_PATH_LENGTH - 1] = '\0';  // Garantir la terminaison de chaîne

    char *token = strtok(chemin_cpy, "/");  // Séparer le chemin par '/'

    while (token != NULL) {
        if (strcmp(token, ".") == 0) {
            // "." ne change rien, il indique le répertoire courant
            token = strtok(NULL, "/");
            continue;
        }
        if (strcmp(token, "..") == 0) {
            // ".." remonte d'un niveau, sauf si déjà à la racine
            if (inode_courant != fs->racine.inode_id) {
                inode_courant = fs->inodes[inode_courant].inode_pere;
            }
            token = strtok(NULL, "/");
            continue;
        }

        // Récupérer l'inode courant
        Inode *inode = &fs->inodes[inode_courant];
        if (!inode->est_repertoire) return -1;  // Si l'inode actuel n'est pas un répertoire, le chemin est invalide

        // Charger le répertoire à partir du bloc associé à l'inode courant
        Repertoire *rep = (Repertoire *)&fs->data[inode->blocs[0] * BLOCK_SIZE];

        // Rechercher l'élément suivant du chemin dans le répertoire
        int trouve = 0;
        for (int i = 0; i < rep->nb_fichiers; i++) {
            if (strcmp(rep->fichiers[i].nom, token) == 0) {
                inode_courant = rep->fichiers[i].inode_id;
                trouve = 1;
                break;
            }
        }
        if (!trouve) return -1;  // Si l'élément n'est pas trouvé dans le répertoire, chemin invalide

        // Passer au token suivant du chemin
        token = strtok(NULL, "/");
    }

    return inode_courant;  // Retourner l'inode correspondant au chemin final
}


/************************************************************************************************************************************ */



/**
 * @brief Change le répertoire courant du système de fichiers.
 * 
 * Cette fonction permet de naviguer dans le système de fichiers simulé en changeant 
 * le répertoire courant. Elle prend en compte les chemins relatifs et absolus ainsi que
 * les commandes spéciales comme '.' (répertoire courant) et '..' (répertoire parent).
 * 
 * @param fs Pointeur vers le système de fichiers.
 * @param chemin Le chemin du répertoire cible. Peut être un chemin relatif ou absolu.
 */
 void cd(SystemeFichier *fs, const char *chemin) {
    if (strcmp(chemin, ".") == 0) {
        // Si le chemin est ".", le répertoire courant ne change pas.
        printf("Déjà dans le répertoire courant.\n");
        return;
    }

    if (strcmp(chemin, "..") == 0) {
        // Si le chemin est "..", on remonte d'un niveau dans l'arborescence
        if (fs->repertoire_courant == fs->racine.inode_id) {
            // Si déjà à la racine, on ne peut pas remonter davantage.
            printf("Déjà à la racine, impossible de remonter.\n");
            return;
        }

        // Trouver le parent en parcourant les inodes et les répertoires
        for (int i = 0; i < MAX_INODES; i++) {
            if (fs->inodes[i].est_repertoire) {
                Repertoire *rep = (Repertoire *)&fs->data[fs->inodes[i].blocs[0] * BLOCK_SIZE];
                // Chercher dans les fichiers du répertoire si le répertoire courant est un enfant
                for (int j = 0; j < rep->nb_fichiers; j++) {
                    if (rep->fichiers[j].inode_id == fs->repertoire_courant) {
                        // Remonter au répertoire parent si trouvé
                        fs->repertoire_courant = i;
                        printf("Remonté au répertoire parent.\n");
                        return;
                    }
                }
            }
        }

        // Si aucun parent n'a été trouvé, erreur
        printf("Erreur : impossible de trouver le répertoire parent.\n");
        return;
    }

    // Résolution du chemin pour trouver l'inode cible
    // On résout d'abord les liens symboliques
    int inode_cible = resoudre_lien_symbolique(fs, chemin);
    
    // Si l'inode cible est invalide ou n'est pas un répertoire, afficher une erreur
    if (inode_cible == -1 || !fs->inodes[inode_cible].est_repertoire) {
        printf("Erreur : répertoire introuvable : %s\n", chemin);
        return;
    }

    // Si le répertoire cible est trouvé et valide, on change le répertoire courant
    fs->repertoire_courant = inode_cible;
    printf("Répertoire courant changé vers : %s\n", chemin);
}


/***************************************************************************************************************************** */



/**
 * @brief Modifie les droits d'un fichier ou d'un répertoire.
 * 
 * Cette fonction permet de modifier les droits d'un fichier ou d'un répertoire spécifié
 * par le chemin donné. Les droits doivent être spécifiés en format octal à 3 chiffres (ex: 744, 755).
 * 
 * @param fs Pointeur vers le système de fichiers.
 * @param chemin Le chemin du fichier ou du répertoire dont les droits doivent être modifiés.
 * @param droits Une chaîne de caractères représentant les nouveaux droits au format octal (ex: "755").
 */
 void droit(SystemeFichier *fs, const char *chemin, const char *droits) {
    // Vérification que "droits" contient bien trois chiffres entre '0' et '7'
    if (strlen(droits) != 3 || !isdigit(droits[0]) || !isdigit(droits[1]) || !isdigit(droits[2])) {
        printf("Erreur : Les droits doivent être une valeur octale valide à 3 chiffres (ex: 744, 755)\n");
        return;
    }

    // Vérification que chaque chiffre des droits est entre 0 et 7
    if (droits[0] < '0' || droits[0] > '7' || droits[1] < '0' || droits[1] > '7' || droits[2] < '0' || droits[2] > '7') {
        printf("Erreur : Chaque chiffre des droits doit être compris entre 0 et 7.\n");
        return;
    }

    // Trouver l'inode correspondant au chemin spécifié
    int inode_index = trouver_inode_par_cheminCd(fs, chemin);
    if (inode_index == -1) {
        // Si l'inode n'est pas trouvé, afficher une erreur
        printf("Erreur : Fichier ou dossier non trouvé\n");
        return;
    }

    // Accéder à l'inode trouvé
    Inode *inode_courant = &fs->inodes[inode_index];

    // Afficher les informations actuelles des droits du fichier
    printf("Fichier trouvé : %s\n", chemin);
    printf("Permissions actuelles : %o (octal)\n", inode_courant->permissions);

    // Convertir les droits octaux (chaîne de caractères) en entier
    int droits_octal = strtol(droits, NULL, 8);

    // Modifier les droits de l'inode
    inode_courant->permissions = droits_octal;

    // Afficher les nouveaux droits après modification
    printf("Permissions après modification : %o (octal)\n", inode_courant->permissions);
}


/********************************************************************************************************************************** */



/**
 * @brief Affiche le contenu d'un répertoire spécifié par un chemin.
 * 
 * Cette fonction parcourt le système de fichiers pour afficher le contenu d'un répertoire donné.
 * Le chemin peut être absolu ou relatif. Si le répertoire est vide, un message sera affiché.
 * 
 * @param fs Pointeur vers le système de fichiers.
 * @param chemin Le chemin du répertoire dont le contenu doit être affiché.
 */
 void afficher_ls_chemin(SystemeFichier *fs, const char *chemin) {
    int inode_id = fs->repertoire_courant; // Par défaut, commencer à partir du répertoire courant

    // Si le chemin est absolu, commencer depuis la racine
    if (chemin[0] == '/') {
        inode_id = fs->racine.inode_id;
        chemin++; // Ignorer le premier '/' du chemin absolu
    }

    // Créer une copie modifiable de la chaîne 'chemin'
    char chemin_copy[MAX_PATH_LENGTH];
    strncpy(chemin_copy, chemin, sizeof(chemin_copy) - 1);
    chemin_copy[sizeof(chemin_copy) - 1] = '\0'; // Assurer que la chaîne est bien terminée

    char *token = strtok(chemin_copy, "/"); // Séparer le chemin en composants
    while (token != NULL) {
        // Cas où le composant est ".." (monter d'un répertoire)
        if (strcmp(token, "..") == 0) {
            // Si nous ne sommes pas déjà à la racine, on remonte d'un répertoire
            if (inode_id != fs->racine.inode_id) {
                // Accéder au répertoire courant
                Repertoire *repertoire = (Repertoire *)(fs->data + fs->inodes[inode_id].blocs[0] * BLOCK_SIZE);

                // Vérifier si le répertoire courant a un inode parent
                if (fs->inodes[inode_id].inode_pere != -1) {
                    inode_id = fs->inodes[inode_id].inode_pere; // Remonter au répertoire parent
                    printf("Remonté au répertoire parent.\n");
                } else {
                    printf("Erreur : Vous êtes déjà à la racine.\n");
                    return;
                }
            } else {
                printf("Erreur : Vous êtes déjà à la racine.\n");
                return;
            }
        }
        // Cas où le composant est "." ou un autre sous-répertoire
        else if (strcmp(token, ".") == 0) {
            // Rester dans le répertoire courant, donc rien à faire
        }
        // Sinon, chercher un sous-répertoire ou fichier avec le nom donné
        else {
            Repertoire *repertoire = (Repertoire *)(fs->data + fs->inodes[inode_id].blocs[0] * BLOCK_SIZE);

            // Chercher l'entrée correspondant au composant du chemin
            int found = 0;
            for (int i = 0; i < repertoire->nb_fichiers; i++) {
                if (strcmp(repertoire->fichiers[i].nom, token) == 0) {
                    inode_id = repertoire->fichiers[i].inode_id;
                    found = 1;

                    // Si c'est un répertoire, naviguer dedans
                    if (fs->inodes[inode_id].est_repertoire) {
                        printf("Naviguer dans le répertoire '%s'.\n", token);
                        break;
                    }
                    // Si c'est un fichier, on ne fait rien de spécial ici
                    else {
                        printf("Fichier trouvé : %s\n", token);
                        break;
                    }
                }
            }

            // Si le fichier ou répertoire n'a pas été trouvé
            if (!found) {
                printf("Erreur : Le répertoire ou fichier '%s' n'a pas été trouvé.\n", token);
                return;
            }
        }

        // Passer au prochain composant du chemin
        token = strtok(NULL, "/");
    }

    // Afficher les fichiers du répertoire cible
    Repertoire *repertoire = (Repertoire *)(fs->data + fs->inodes[inode_id].blocs[0] * BLOCK_SIZE);
    if (repertoire->nb_fichiers == 0) {
        printf("Le répertoire est vide.\n");
    } else {
        printf("Contenu du répertoire %d (%d):\n", inode_id, fs->inodes[inode_id].id);
        printf("Nom\t\tTaille\t\tPermissions\t\tInodes\n");

        for (int i = 0; i < repertoire->nb_fichiers; i++) {
            int file_inode_id = repertoire->fichiers[i].inode_id;
            Inode *file_inode = &fs->inodes[file_inode_id];

            printf("%s\t\t%d octets\t\t%o\t\t%d\n",
                   repertoire->fichiers[i].nom,
                   file_inode->taille,
                   file_inode->permissions,
                   file_inode_id);
        }
    }
}


/****************************************************************************************************************************** */



/**
 * @brief Supprime un fichier du système de fichiers.
 * 
 * Cette fonction supprime un fichier spécifié par son chemin. Elle cherche d'abord le répertoire
 * parent, puis trouve le fichier dans ce répertoire. Ensuite, elle libère l'inode et le bloc du fichier,
 * et met à jour la table des fichiers du répertoire parent.
 * 
 * @param fs Pointeur vers le système de fichiers.
 * @param chemin Le chemin absolu ou relatif du fichier à supprimer.
 */
 void supprimer_fichier(SystemeFichier *fs, const char *chemin)
 {
     // Trouver le répertoire parent et le nom du fichier
     char chemin_cpy[MAX_PATH_LENGTH];
     strncpy(chemin_cpy, chemin, MAX_PATH_LENGTH);
     char *dernier_slash = strrchr(chemin_cpy, '/');  // Recherche du dernier '/' dans le chemin
     char nom[NAME_SIZE]; // Nom du fichier à supprimer
     const char *chemin_parent; // Chemin du répertoire parent
 
     // Si un '/' est trouvé, séparer le chemin parent et le nom du fichier
     if (dernier_slash)
     {
         strncpy(nom, dernier_slash + 1, NAME_SIZE);
         *dernier_slash = '\0';  // Remplacer le '/' par un caractère nul pour obtenir le chemin parent
         chemin_parent = (*chemin_cpy) ? chemin_cpy : "/";  // Si le chemin parent est vide, le répertoire parent est la racine
     }
     else
     {
         strncpy(nom, chemin, NAME_SIZE);
         chemin_parent = "";  // Si aucun '/', cela signifie que le fichier est dans le répertoire courant
     }
 
     // Trouver l'inode du répertoire parent
     int inode_parent = trouver_inode_par_chemin(fs, chemin_parent);
     if (inode_parent == -1)
     {
         printf("Erreur : chemin parent %s introuvable.\n", chemin_parent);
         return;
     }
 
     // Récupérer l'inode et le répertoire parent
     Inode *inode_p = &fs->inodes[inode_parent];
     Repertoire *rep_p = (Repertoire *)&fs->data[inode_p->blocs[0] * BLOCK_SIZE];
 
     // Chercher le fichier à supprimer dans le répertoire
     int fichier_trouve = -1;
     for (int i = 0; i < rep_p->nb_fichiers; i++)
     {
         if (strcmp(rep_p->fichiers[i].nom, nom) == 0)
         {
             fichier_trouve = i;  // Fichier trouvé, on retient son indice
             break;
         }
     }
 
     // Si le fichier n'a pas été trouvé, afficher une erreur
     if (fichier_trouve == -1)
     {
         printf("Erreur : fichier %s introuvable.\n", nom);
         return;
     }
 
     // Libérer l'inode et le bloc du fichier
     int inode_fichier = rep_p->fichiers[fichier_trouve].inode_id;
     int bloc_fichier = fs->inodes[inode_fichier].blocs[0];
 
     // Libérer l'inode et le bloc du fichier dans le bitmap
     BIT_CLEAR(fs->bitmap.inodes[inode_fichier / 8], inode_fichier % 8);
     BIT_CLEAR(fs->bitmap.blocs[bloc_fichier / 8], bloc_fichier % 8);
 
     // Mettre à jour le nombre d'inodes et de blocs libres dans le superbloc
     fs->superbloc.inode_libres++;
     fs->superbloc.blocs_libres++;
 
     // Supprimer l'entrée du fichier dans le répertoire
     for (int i = fichier_trouve; i < rep_p->nb_fichiers - 1; i++)
     {
         rep_p->fichiers[i] = rep_p->fichiers[i + 1];  // Décaler les fichiers suivants pour combler le vide
     }
     rep_p->nb_fichiers--;  // Réduire le nombre de fichiers dans le répertoire
     
     sauvegarder_systeme_fichier(fs); //sauvegarder la partition
     printf("Fichier %s supprimé avec succès.\n", nom);
 }
 

/************************************************************************************************************************************* */


/**
 * @brief Supprime un répertoire du système de fichiers.
 * 
 * Cette fonction supprime un répertoire spécifié par son chemin. Si le répertoire n'est pas vide, 
 * elle supprime récursivement tous les fichiers et sous-répertoires qu'il contient avant de libérer
 * l'inode et le bloc du répertoire. Si aucun chemin n'est donné, le répertoire courant est supprimé.
 * 
 * @param fs Pointeur vers le système de fichiers.
 * @param chemin Le chemin absolu ou relatif du répertoire à supprimer (peut être vide pour le répertoire courant).
 */
 void supprimer_repertoire(SystemeFichier *fs, const char *chemin)
 {
     int inode_a_supprimer;
     // Si un chemin est donné, on utilise ce chemin, sinon on prend le répertoire courant
     if (chemin && strlen(chemin) > 0)
     {
         inode_a_supprimer = trouver_inode_par_chemin(fs, chemin); // Trouver l'inode du répertoire à supprimer
         if (inode_a_supprimer == -1)
         {
             // Si le répertoire n'existe pas, afficher un message d'erreur et retourner
             printf("Erreur : répertoire %s introuvable.\n", chemin);
             return;
         }
     }
     else
     {
         // Si aucun chemin n'est donné, supprimer le répertoire courant
         inode_a_supprimer = fs->repertoire_courant;
     }
 
     // Récupérer l'inode du répertoire et son contenu
     Inode *inode_r = &fs->inodes[inode_a_supprimer];
     Repertoire *rep_r = (Repertoire *)&fs->data[inode_r->blocs[0] * BLOCK_SIZE];
 
     // Vérifier si le répertoire est vide (le répertoire a plus de 2 liens => il n'est pas vide)
     if (inode_r->liens > 2)
     {
         // Suppression récursive des fichiers et sous-répertoires
         for (int i = 0; i < rep_r->nb_fichiers; i++)
         {
             int inode_fichier = rep_r->fichiers[i].inode_id;
             Inode *fichier_inode = &fs->inodes[inode_fichier];
 
             // Déclaration de chemin_sousdossier pour les sous-répertoires et fichiers
             char chemin_sousdossier[MAX_PATH_LENGTH];
 
             // Si c'est un répertoire, suppression récursive
             if (fichier_inode->est_repertoire)
             {
                 snprintf(chemin_sousdossier, MAX_PATH_LENGTH, "%s/%s", chemin ? chemin : ".", rep_r->fichiers[i].nom);
                 supprimer_repertoire(fs, chemin_sousdossier); // Appel récursif pour supprimer les sous-répertoires
             }
             else
             {
                 // Si c'est un fichier, suppression du fichier
                 snprintf(chemin_sousdossier, MAX_PATH_LENGTH, "%s/%s", chemin ? chemin : ".", rep_r->fichiers[i].nom);
                 supprimer_fichier(fs, chemin_sousdossier); // Appel à la fonction pour supprimer le fichier
             }
         }
     }
 
     // Libérer l'inode et le bloc du répertoire
     int bloc_repertoire = inode_r->blocs[0];
     BIT_CLEAR(fs->bitmap.inodes[inode_a_supprimer / 8], inode_a_supprimer % 8); // Marquer l'inode comme libre
     BIT_CLEAR(fs->bitmap.blocs[bloc_repertoire / 8], bloc_repertoire % 8); // Marquer le bloc comme libre
     fs->superbloc.inode_libres++; // Augmenter le nombre d'inodes libres
     fs->superbloc.blocs_libres++; // Augmenter le nombre de blocs libres
 
     // Supprimer l'entrée du répertoire dans le répertoire parent
     int inode_parent = fs->racine.inode_id; // Par défaut, le répertoire parent est la racine
     Inode *inode_p = &fs->inodes[inode_parent];
     Repertoire *rep_p = (Repertoire *)&fs->data[inode_p->blocs[0] * BLOCK_SIZE];
 
     // Chercher et supprimer l'entrée du répertoire à supprimer dans le répertoire parent
     int repertoire_trouve = -1;
     for (int i = 0; i < rep_p->nb_fichiers; i++)
     {
         if (rep_p->fichiers[i].inode_id == inode_a_supprimer)
         {
             repertoire_trouve = i;
             break;
         }
     }
 
     // Si le répertoire est trouvé, le supprimer de la liste
     if (repertoire_trouve != -1)
     {
         for (int i = repertoire_trouve; i < rep_p->nb_fichiers - 1; i++)
         {
             rep_p->fichiers[i] = rep_p->fichiers[i + 1]; // Décaler les entrées
         }
         rep_p->nb_fichiers--; // Réduire le nombre de fichiers dans le répertoire parent
     }
 
     sauvegarder_systeme_fichier(fs); //sauvegarder la partition
     // Afficher un message de succès
     printf("Répertoire %s supprimé avec succès.\n", chemin ? chemin : "répertoire courant");
 }
 



/************************************************************************************************************************************* */



/**
 * @brief Vérifie si un inode a les permissions spécifiées.
 * 
 * Cette fonction vérifie si un inode a les permissions adéquates pour une opération donnée. 
 * Les modes valides sont `MODE_READ` (lecture) et `MODE_WRITE` (écriture).
 * 
 * @param inode Pointeur vers l'inode dont les permissions doivent être vérifiées.
 * @param mode Le mode de vérification des permissions (lecture ou écriture).
 * 
 * @return 1 si l'inode a les permissions nécessaires, 0 sinon.
 */
 int verifier_permissions(Inode *inode, int mode)
 {
     if (mode == MODE_READ)
     {                                                                    // Mode lecture
         // Vérifie si les permissions sont soit 0755 (lecture permise) soit 0777 (lecture permise pour tous)
         return inode->permissions == 0755 || inode->permissions == 0777;
     }
     else if (mode == MODE_WRITE)
     {                                                                    // Mode écriture
         // Vérifie si les permissions sont soit 0755 (écriture permise) soit 0777 (écriture permise pour tous)
         return inode->permissions == 0755 || inode->permissions == 0777;
     }
     return 0;  // Si le mode n'est pas valide, retourner 0
 }

 
 /************************************************************************************************************************************** */



/**
 * @brief Ouvre un fichier en fonction du chemin et du mode.
 * 
 * Cette fonction cherche l'inode du fichier à partir du chemin donné, vérifie les permissions d'accès 
 * et cherche un espace libre pour ouvrir le fichier. Le fichier est associé à un descripteur qui est retourné.
 * 
 * @param fs Le système de fichiers dans lequel le fichier doit être ouvert.
 * @param chemin Le chemin d'accès du fichier à ouvrir.
 * @param mode Le mode d'ouverture du fichier (lecture ou écriture).
 * 
 * @return Le descripteur de fichier en cas de succès, -1 en cas d'erreur.
 */
 int open_file(SystemeFichier *fs, const char *chemin, int mode)
 {
     // Trouver l'inode du fichier à partir du chemin donné
     int inode_id = trouver_inode_par_chemin(fs, chemin);
     if (inode_id == -1)
     {
         // Si l'inode n'est pas trouvé, afficher un message d'erreur et retourner -1
         printf("Erreur : fichier %s introuvable.\n", chemin);
         return -1;
     }
 
     Inode *inode = &fs->inodes[inode_id];
 
     // Vérifier les permissions d'accès en fonction du mode (lecture ou écriture)
     if ((mode == MODE_READ && !verifier_permissions(inode, MODE_READ)) ||
         (mode == MODE_WRITE && !verifier_permissions(inode, MODE_WRITE)))
     {
         // Si les permissions d'accès sont insuffisantes, afficher un message d'erreur et retourner -1
         printf("Erreur : permission d'accès refusée pour le fichier %s.\n", chemin);
         return -1;
     }
 
     // Chercher un espace libre pour ouvrir le fichier (trouver une place dans la liste des fichiers ouverts)
     for (int i = 0; i < MAX_FILES; i++)
     {
         if (fs->racine.fichiers[i].inode_id == 0)  // Un descripteur libre (inode_id égal à 0)
         {
             // Initialiser le descripteur de fichier avec l'inode et le mode d'ouverture
             fs->racine.fichiers[i].inode_id = inode_id;   // Associer le descripteur au fichier
             fs->racine.fichiers[i].nom[0] = chemin[0];    // Stocker le premier caractère du nom du fichier (attention, il faudrait probablement gérer toute la chaîne de caractères)
             fs->racine.fichiers[i].inode_id = mode;       // Associer le mode (lecture/écriture)
             printf("Fichier %s ouvert avec succès en mode %d.\n", chemin, mode);
             return i;  // Retourner l'indice du descripteur de fichier
         }
     }
 
     // Si aucun espace libre n'est trouvé pour ouvrir un fichier, afficher un message d'erreur et retourner -1
     printf("Erreur : trop de fichiers ouverts.\n");
     return -1;
 }

 /*********************************************************************************************************************************************** */
 

/**
 * @brief Ferme un fichier.
 * 
 * Cette fonction libère le descripteur de fichier, le marquant comme disponible pour d'autres ouvertures.
 * 
 * @param fs Le système de fichiers contenant le fichier à fermer.
 * @param descripteur Le descripteur de fichier à fermer.
 */
void close_file(SystemeFichier *fs, int descripteur)
{
    // Vérifier si le descripteur est valide
    if (descripteur < 0 || descripteur >= MAX_FILES || fs->racine.fichiers[descripteur].inode_id == 0)
    {
        printf("Erreur : descripteur de fichier invalide.\n");
        return;
    }

    // Libérer le descripteur de fichier
    fs->racine.fichiers[descripteur].inode_id = 0;
    printf("Fichier avec descripteur %d fermé avec succès.\n", descripteur);
}


/***************************************************************************************************************************** */



/**
 * @brief Écrit dans un fichier.
 * 
 * Cette fonction ouvre un fichier en mode écriture, vérifie les permissions d'écriture, 
 * et permet à l'utilisateur d'ajouter des données au fichier. Si la taille du fichier dépasse la capacité d'un bloc,
 * plusieurs blocs seront alloués pour contenir les données.
 * 
 * @param fs Le système de fichiers dans lequel le fichier doit être modifié.
 * @param chemin Le chemin du fichier à modifier.
 */

 void ecrire_fichier(SystemeFichier *fs, const char *chemin)
 {
     // Ouvrir le fichier en mode écriture
     int descripteur = open_file(fs, chemin, MODE_WRITE);
     if (descripteur == -1)
     {
         return; // Erreur lors de l'ouverture
     }
 
     // Trouver l'inode du fichier
     int inode_id = resoudre_lien_symbolique(fs, chemin);
     if (inode_id == -1)
     {
         printf("Erreur : fichier %s introuvable.\n", chemin);
         return;
     }
 
     Inode *inode = &fs->inodes[inode_id];
 
     // Vérifier les permissions d'écriture
     if (!verifier_permissions(inode, MODE_WRITE))
     {
         printf("Erreur : permission d'écriture refusée pour le fichier %s.\n", chemin);
         close_file(fs, descripteur); // Fermer le fichier avant de quitter
         return;
     }
 
     // Demander à l'utilisateur de saisir des données à ajouter
     printf("Entrez les données à ajouter au fichier (CTRL+D pour terminer) :\n");
 
     // Lecture de l'entrée de l'utilisateur
     char buffer[BLOCK_SIZE];
     char *donnees = malloc(BLOCK_SIZE * 10); // Allocation pour un plus grand buffer de données (ajusté)
     if (donnees == NULL)
     {
         printf("Erreur : impossible d'allouer de la mémoire.\n");
         close_file(fs, descripteur); // Fermer le fichier avant de quitter
         return;
     }
 
     // Ajouter l'ancien contenu à l'entrée (pour ne pas écraser)
     strncpy(donnees, &fs->data[inode->blocs[0] * BLOCK_SIZE], BLOCK_SIZE);
 
     // Lire les nouvelles données à ajouter
     int taille_donnees = strlen(donnees);  // Commence par la taille de l'ancien contenu
     while (fgets(buffer, BLOCK_SIZE, stdin) != NULL)
     {
         // Vérifie si CTRL+X (ASCII 24) est présent dans la ligne
         if (strchr(buffer, 24) != NULL)
         {
             printf("CTRL+X détecté. Arrêt de la saisie.\n");
             break;
         }
 
         int len = strlen(buffer);
         if (taille_donnees + len >= BLOCK_SIZE * 10 - 1)
         {
             printf("Erreur : taille maximale de fichier atteinte.\n");
             break;
         }
 
         // Ajout des données avant de vérifier CTRL+X
         strcat(donnees, buffer);
         taille_donnees += len;
     }
 
     // Si les données dépassent la taille d'un bloc, on gère plusieurs blocs
     int blocs_necessaires = (taille_donnees / BLOCK_SIZE) + (taille_donnees % BLOCK_SIZE != 0);
     if (blocs_necessaires > 12) {
         // Gestion de blocs indirects si plus de 12 blocs sont nécessaires
         inode->indirect_block = allouer_bloc(fs);  // Allouer un bloc indirect
         int *bloc_indirect = (int *)&fs->data[inode->indirect_block * BLOCK_SIZE];
 
         // Allouer des blocs directs et enregistrer leurs indices dans le bloc indirect
         for (int i = 0; i < blocs_necessaires - 12; i++) {
             int bloc_fichier = allouer_bloc(fs);
             bloc_indirect[i] = bloc_fichier; // Enregistrer dans le bloc indirect
         }
     }
 
     // Écrire les données dans les blocs
     int index_contenu = 0;
     for (int i = 0; i < blocs_necessaires; i++)
     {
         int start = i * BLOCK_SIZE;
         int end = (i + 1) * BLOCK_SIZE;
 
         // Si c'est le dernier bloc, on écrit uniquement la partie restante des données
         if (end > taille_donnees)
             end = taille_donnees;
 
         // Si le bloc est direct, on l'écrit directement
         if (i < 12)
         {
             int bloc_fichier = inode->blocs[i];
             strncpy(&fs->data[bloc_fichier * BLOCK_SIZE], donnees + start, end - start);
         }
         else
         {
             // Sinon, c'est un bloc indirect, on écrit dedans
             int *bloc_indirect = (int *)&fs->data[inode->indirect_block * BLOCK_SIZE];
             int bloc_fichier = bloc_indirect[i - 12];
             strncpy(&fs->data[bloc_fichier * BLOCK_SIZE], donnees + start, end - start);
         }
     }
 
     // Mise à jour de la taille du fichier et de la date de modification
     inode->taille = taille_donnees;
     inode->date_modification = time(NULL);  // Mise à jour de la date de modification
 
     // Afficher le contenu ajouté
     printf("Données ajoutées au fichier %s :\n%s\n", chemin, donnees);
     sauvegarder_systeme_fichier(fs);  // Sauvegarder les changements
 
     // Libérer la mémoire allouée
     free(donnees);
 
     // Fermer le fichier après l'écriture
     close_file(fs, descripteur);
 
     printf("Écriture terminée et fichier fermé.\n");
 }
 


/************************************************************************************************************************************* */



/**
 * @brief Lit le contenu d'un fichier.
 * 
 * Cette fonction permet de lire un fichier en vérifiant que le fichier existe, qu'il n'est pas un répertoire, 
 * et que l'utilisateur a les permissions nécessaires pour le lire. Le contenu du fichier est affiché à l'écran.
 * 
 * @param fs Le système de fichiers dans lequel le fichier doit être lu.
 * @param chemin Le chemin du fichier à lire.
 */
 void lire_fichier(SystemeFichier *fs, const char *chemin) {
    // Trouver l'inode en résolvant les liens symboliques s'il y en a
    int inode_id = resoudre_lien_symbolique(fs, chemin);
    if (inode_id == -1) {
        printf("Erreur : fichier %s introuvable après résolution.\n", chemin);
        return;
    }

    Inode *inode = &fs->inodes[inode_id];

    // Vérifier que ce n'est pas un lien symbolique mal résolu
    if (inode->est_lien) {
        printf("Erreur : %s est encore un lien symbolique après résolution.\n", chemin);
        return;
    }

    // Vérifier si c'est un répertoire
    if (inode->est_repertoire) {
        printf("Erreur : %s est un répertoire, impossible de le lire comme un fichier.\n", chemin);
        return;
    }

    // Vérifier les permissions de lecture
    if (!verifier_permissions(inode, MODE_READ)) {
        printf("Erreur : permission de lecture refusée pour le fichier %s.\n", chemin);
        return;
    }

    // Vérifier que le fichier a bien du contenu
    if (inode->blocs[0] == -1) {
        printf("Erreur : fichier %s vide ou sans bloc de données.\n", chemin);
        return;
    }

    // Lire et afficher le contenu du fichier
    printf("Contenu du fichier %s :\n", chemin);
    
    // Lire les blocs directs
    for (int i = 0; i < NUM_DIRECT_BLOCKS; i++) {
        if (inode->blocs[i] != -1) {
            printf("%s\n", &fs->data[inode->blocs[i] * BLOCK_SIZE]);
        }
    }

    // Si le fichier utilise un bloc indirect, le lire
    if (inode->indirect_block != -1) {
        // Lecture du bloc indirect
        int *bloc_indirect = (int *)&fs->data[inode->indirect_block * BLOCK_SIZE];
        
        // Parcourir les indices du bloc indirect et afficher les blocs de données associés
        for (int i = 0; i < BLOCK_SIZE / sizeof(int); i++) {
            if (bloc_indirect[i] != -1) {
                printf("%s\n", &fs->data[bloc_indirect[i] * BLOCK_SIZE]);
            }
        }
    }

    printf("Fichier lu avec succès.\n");
}



/********************************************************************************************************************************************** */



/**
 * @brief Crée un lien symbolique dans un système de fichiers.
 * 
 * Cette fonction permet de créer un lien symbolique dans le répertoire spécifié par le chemin. 
 * Un inode est alloué pour le lien symbolique, les vérifications sont effectuées pour s'assurer que le lien n'existe pas déjà,
 * et le lien est ajouté au répertoire parent. Le lien symbolique pointe vers un fichier cible.
 * 
 * @param fs Le système de fichiers dans lequel le lien symbolique doit être créé.
 * @param chemin Le chemin complet où le lien symbolique doit être créé.
 * @param cible Le chemin du fichier ou répertoire vers lequel le lien symbolique pointe.
 */
 void creer_lien_symbolique(SystemeFichier *fs, const char *chemin, const char *cible) {
    // Copie du chemin pour éviter de modifier l'original
    char chemin_cpy[MAX_PATH_LENGTH];
    strncpy(chemin_cpy, chemin, MAX_PATH_LENGTH);

    // Séparer le chemin en répertoire parent et le nom du lien symbolique
    char *dernier_slash = strrchr(chemin_cpy, '/'); // Recherche le dernier '/' dans le chemin
    char nom[NAME_SIZE];  // Nom du lien symbolique
    const char *chemin_parent;  // Chemin du répertoire parent

    // Si le chemin contient un '/', on sépare le nom du lien et le répertoire parent
    if (dernier_slash) {
        strncpy(nom, dernier_slash + 1, NAME_SIZE);  // Nom du lien symbolique
        *dernier_slash = '\0';  // Isoler le répertoire parent
        chemin_parent = (*chemin_cpy) ? chemin_cpy : "/"; // Si le répertoire est vide, il faut utiliser "/"
    } else {
        strncpy(nom, chemin, NAME_SIZE);  // Pas de '/' => le chemin est juste le nom
        chemin_parent = "";
    }

    // Afficher des informations sur le lien symbolique à créer
    printf("Création du lien symbolique %s dans %s, cible : %s\n", nom, chemin_parent, cible);

    // Trouver l'inode du répertoire parent à partir du chemin
    int inode_parent = trouver_inode_par_chemin(fs, chemin_parent);
    if (inode_parent == -1) {
        printf("Erreur : chemin parent %s introuvable.\n", chemin_parent);
        return;
    }

    Inode *inode_p = &fs->inodes[inode_parent];  // Récupérer l'inode du répertoire parent
    Repertoire *rep_p = (Repertoire *)&fs->data[inode_p->blocs[0] * BLOCK_SIZE];  // Récupérer le répertoire parent

    // Vérifier si le lien symbolique existe déjà dans le répertoire parent
    for (int i = 0; i < rep_p->nb_fichiers; i++) {
        if (strcmp(rep_p->fichiers[i].nom, nom) == 0) {  // Si le fichier existe déjà
            printf("Erreur : %s existe déjà.\n", nom);
            return;  // Le lien symbolique ne sera pas créé
        }
    }

    // Allouer un inode pour le lien symbolique
    int inode_lien = allouer_inode(fs);
    if (inode_lien == -1) {
        printf("Erreur : ressources insuffisantes pour créer le lien symbolique %s\n", nom);
        return;  // Si l'allocation échoue, on arrête
    }

    // Allouer un bloc pour le lien symbolique
    int bloc_lien = allouer_bloc(fs);
    if (bloc_lien == -1) {
        printf("Erreur : ressources insuffisantes pour créer le bloc du lien symbolique %s\n", nom);
        return;  // Si l'allocation échoue, on arrête
    }

    // Initialiser l'inode du lien symbolique
    Inode *new_inode = &fs->inodes[inode_lien];
    new_inode->id = inode_lien;
    new_inode->taille = strlen(cible) + 1;  // La taille du lien symbolique est la taille de la chaîne cible
    new_inode->est_repertoire = 0;  // Ce n'est pas un répertoire
    new_inode->permissions = 0777;  // Permissions pour tous
    new_inode->liens = 1;  // Il est pointé par un lien
    new_inode->blocs[0] = bloc_lien;
    new_inode->date_creation = time(NULL);  // Date de création du lien symbolique
    new_inode->date_modification = new_inode->date_creation;  // Date de modification
    new_inode->inode_pere = inode_parent;  // L'inode parent du lien symbolique
    new_inode->est_lien = 1;  // Cet inode est un lien symbolique

    // Initialiser le contenu du bloc du lien symbolique avec la cible
    char *lien_data = (char *)&fs->data[bloc_lien * BLOCK_SIZE];
    strncpy(lien_data, cible, BLOCK_SIZE);  // Le bloc contient la cible du lien symbolique

    // Ajouter le lien symbolique au répertoire parent
    Repertoire *rep_parent = (Repertoire *)&fs->data[inode_p->blocs[0] * BLOCK_SIZE];
    if (rep_parent->nb_fichiers >= MAX_FILES) {  // Si le répertoire est plein
        printf("Erreur : le répertoire parent est plein, impossible d'ajouter le lien symbolique.\n");
        return;
    }

    // Ajouter le lien symbolique au répertoire
    strncpy(rep_parent->fichiers[rep_parent->nb_fichiers].nom, nom, NAME_SIZE);
    rep_parent->fichiers[rep_parent->nb_fichiers].inode_id = inode_lien;
    rep_parent->nb_fichiers++;  // Incrémenter le nombre de fichiers du répertoire

    // Afficher les informations de création
    printf("Lien symbolique %s créé avec succès dans %s\n", nom, chemin_parent);
    printf("Inode créé pour %s : id = %d, est_lien = %d\n", nom, inode_lien, new_inode->est_lien);
}


/************************************************************************************************************************************** */



/**
 * @brief Résout un lien symbolique et retourne son inode.
 * 
 * Cette fonction résout récursivement un lien symbolique en suivant la chaîne de cibles
 * jusqu'à ce qu'un fichier ou répertoire normal soit trouvé. 
 * Cela permet de retrouver le fichier ou répertoire réel vers lequel le lien symbolique pointe, 
 * en résolvant tous les liens symboliques intermédiaires le cas échéant.
 * 
 * @param fs Le système de fichiers contenant le lien symbolique.
 * @param chemin Le chemin du lien symbolique à résoudre.
 * 
 * @return L'inode du fichier ou répertoire pointé par le lien symbolique, ou -1 en cas d'erreur.
 */
 int resoudre_lien_symbolique(SystemeFichier *fs, const char *chemin) {
    // Trouver l'inode du chemin spécifié dans le système de fichiers
    int inode_id = trouver_inode_par_chemin(fs, chemin);
    
    // Vérifier si l'inode existe, si non, afficher une erreur et retourner -1
    if (inode_id == -1) {
        printf("Erreur : le chemin %s n'existe pas.\n", chemin);
        return -1;
    }

    // Récupérer l'inode correspondant à l'ID trouvé
    Inode *inode = &fs->inodes[inode_id];

    // Afficher un message pour indiquer si le fichier est un lien symbolique ou non
    printf("Vérification de inode->est_lien pour %s : %d\n", chemin, inode->est_lien);

    // Si l'inode n'est pas un lien symbolique, retourner simplement l'inode trouvé
    if (!inode->est_lien) {
        // Ce n'est pas une erreur, le fichier ou répertoire est normal, on retourne son inode
        printf("Le chemin %s est un fichier ou un répertoire normal.\n", chemin);
        return inode_id;
    }

    // Si l'inode est un lien symbolique, récupérer sa cible
    char cible[MAX_PATH_LENGTH];
    char *lien_data = (char *)&fs->data[inode->blocs[0] * BLOCK_SIZE];
    strncpy(cible, lien_data, MAX_PATH_LENGTH);  // Copier la cible dans la variable cible

    // Afficher la cible du lien symbolique
    printf("Résolution du lien symbolique %s : cible = %s\n", chemin, cible);

    // Appeler récursivement la fonction pour résoudre le lien symbolique, si la cible est aussi un lien symbolique
    return resoudre_lien_symbolique(fs, cible);
}



/**********************************************************************************************************************************************/



/**
 * @brief Déplace un fichier ou un répertoire d'un emplacement source vers un emplacement de destination.
 * 
 * Cette fonction déplace un fichier ou répertoire dans le système de fichiers. Si la destination est un répertoire, 
 * elle ajoutera le fichier/répertoire à ce répertoire, tout en retirant le fichier/répertoire de son répertoire parent.
 * 
 * @param fs Le système de fichiers sur lequel l'opération est effectuée.
 * @param source Le chemin du fichier/répertoire source à déplacer.
 * @param destination Le chemin du répertoire de destination.
 * 
 * @return Retourne 0 si le déplacement a réussi, sinon -1 en cas d'erreur.
 * 
 * @note Cette fonction vérifie que la destination est bien un répertoire et que le répertoire parent de la source
 * existe avant de procéder.
 */
/**
 * @brief Déplace un fichier ou un répertoire d'un emplacement à un autre.
 * 
 * Cette fonction déplace un fichier ou un répertoire depuis un chemin source vers un chemin de destination.
 * Le fichier ou répertoire source est retiré de son répertoire parent et ajouté au répertoire de destination.
 * Si la destination n'est pas un répertoire ou si des erreurs surviennent, un message d'erreur est affiché.
 * 
 * @param fs Le système de fichiers contenant les fichiers à déplacer.
 * @param source Le chemin du fichier/répertoire source à déplacer.
 * @param destination Le chemin du répertoire destination où le fichier/répertoire sera déplacé.
 * 
 * @return 0 si le déplacement a réussi, -1 en cas d'erreur.
 */
 int mv(SystemeFichier *fs, const char *source, const char *destination) {
    // Trouver l'inode du fichier/répertoire source
    int inode_source = trouver_inode_par_cheminCd(fs, source);
    if (inode_source == -1) {
        // Si l'inode source n'existe pas, afficher une erreur
        printf("Erreur : Le fichier/répertoire source n'existe pas.\n");
        return -1;
    }

    // Trouver l'inode du répertoire de destination
    int inode_dest = trouver_inode_par_cheminCd(fs, destination);
    if (inode_dest == -1) {
        // Si l'inode destination n'existe pas, afficher une erreur
        printf("Erreur : Le dossier de destination n'existe pas.\n");
        return -1;
    }

    // Récupérer les inodes de la source et de la destination
    Inode *inode_src = &fs->inodes[inode_source];
    Inode *inode_dst = &fs->inodes[inode_dest];

    // Vérifier si la destination est un répertoire
    if (!inode_dst->est_repertoire) {
        printf("Erreur : La destination doit être un répertoire.\n");
        return -1;
    }

    // Trouver le parent du fichier/répertoire source pour pouvoir le retirer
    Inode *inode_parent_src = &fs->inodes[inode_src->inode_pere];
    Repertoire *rep_parent_src = (Repertoire *)&fs->data[inode_parent_src->blocs[0] * BLOCK_SIZE];
    const char *src_name;

    // Retirer la source du répertoire parent
    for (int i = 0; i < rep_parent_src->nb_fichiers; i++) {
        // Chercher le fichier dans le répertoire parent en comparant l'inode
        if (rep_parent_src->fichiers[i].inode_id == inode_source) {
            // Conserver le nom du fichier pour pouvoir l'ajouter dans le répertoire destination
            src_name = rep_parent_src->fichiers[i].nom;
            // Décalage pour supprimer l'entrée du fichier du répertoire parent
            for (int j = i; j < rep_parent_src->nb_fichiers - 1; j++) {
                rep_parent_src->fichiers[j] = rep_parent_src->fichiers[j + 1];
            }
            rep_parent_src->nb_fichiers--;
            break;
        }
    }

    // Ajouter le fichier/répertoire source dans le répertoire de destination
    Repertoire *rep_dest = (Repertoire *)&fs->data[inode_dst->blocs[0] * BLOCK_SIZE];

    // Vérifier si le répertoire de destination a de la place
    if (rep_dest->nb_fichiers >= MAX_FILES) {
        printf("Erreur : Le répertoire de destination est plein.\n");
        return -1;
    }

    // Ajouter le fichier/répertoire dans le répertoire de destination
    strncpy(rep_dest->fichiers[rep_dest->nb_fichiers].nom, src_name, NAME_SIZE);
    rep_dest->fichiers[rep_dest->nb_fichiers].inode_id = inode_source;
    rep_dest->nb_fichiers++;

    // Mettre à jour l'inode de la source pour pointer vers le nouveau répertoire parent
    inode_src->inode_pere = inode_dest;

    // Sauvegarder les changements dans le système de fichiers
    sauvegarder_systeme_fichier(fs);

    // Afficher un message de réussite
    printf("Déplacement réussi : %s → %s\n", source, destination);
    return 0;
}



/*******************************************************************************************************************************************/


/**
 * @brief Copie un fichier ou un répertoire d'un emplacement source vers un emplacement de destination.
 * 
 * Cette fonction copie un fichier ou répertoire dans le système de fichiers. La source est dupliquée dans le répertoire 
 * de destination sans supprimer l'original dans son répertoire d'origine.
 * 
 * @param fs Le système de fichiers sur lequel l'opération est effectuée.
 * @param source Le chemin du fichier/répertoire source à copier.
 * @param destination Le chemin du répertoire de destination.
 * 
 * @return Retourne 0 si la copie a réussi, sinon -1 en cas d'erreur.
 * 
 * @note Cette fonction vérifie que la destination est bien un répertoire et que le répertoire parent de la source
 * existe avant de procéder.
 */
 int cp(SystemeFichier *fs, const char *source, const char *destination) {
    // Trouver l'inode correspondant à la source à partir du chemin
    int inode_source = trouver_inode_par_cheminCd(fs, source);
    if (inode_source == -1) {
        // Si l'inode source est introuvable, afficher une erreur et retourner -1
        printf("Erreur : Le fichier/répertoire source n'existe pas.\n");
        return -1;
    }

    // Trouver l'inode correspondant à la destination à partir du chemin
    int inode_dest = trouver_inode_par_cheminCd(fs, destination);
    if (inode_dest == -1) {
        // Si l'inode destination est introuvable, afficher une erreur et retourner -1
        printf("Erreur : Le dossier de destination n'existe pas.\n");
        return -1;
    }

    // Récupérer les inodes de la source et de la destination
    Inode *inode_src = &fs->inodes[inode_source];
    Inode *inode_dst = &fs->inodes[inode_dest];

    // Vérifier que la destination est bien un répertoire
    if (!inode_dst->est_repertoire) {
        // Si la destination n'est pas un répertoire, afficher une erreur et retourner -1
        printf("Erreur : La destination doit être un répertoire.\n");
        return -1;
    }

    // Trouver le répertoire parent de la source pour accéder à son nom
    Inode *inode_parent_src = &fs->inodes[inode_src->inode_pere];
    Repertoire *rep_parent_src = (Repertoire *)&fs->data[inode_parent_src->blocs[0] * BLOCK_SIZE];
    const char *src_name;

    // Chercher le nom du fichier/répertoire source dans le répertoire parent
    for (int i = 0; i < rep_parent_src->nb_fichiers; i++) {
        if (rep_parent_src->fichiers[i].inode_id == inode_source) {
            src_name = rep_parent_src->fichiers[i].nom;
            break;
        }
    }

    // Accéder au répertoire de destination pour y ajouter le fichier/répertoire copié
    Repertoire *rep_dest = (Repertoire *)&fs->data[inode_dst->blocs[0] * BLOCK_SIZE];

    // Vérifier si le répertoire de destination est plein
    if (rep_dest->nb_fichiers >= MAX_FILES) {
        // Si le répertoire de destination est plein, afficher une erreur et retourner -1
        printf("Erreur : Le répertoire de destination est plein.\n");
        return -1;
    }

    // Ajouter la source dans le répertoire de destination
    strncpy(rep_dest->fichiers[rep_dest->nb_fichiers].nom, src_name, NAME_SIZE);
    rep_dest->fichiers[rep_dest->nb_fichiers].inode_id = inode_source;
    rep_dest->nb_fichiers++;

    // Mettre à jour l'inode de la source pour qu'il pointe vers le nouveau répertoire parent (destination)
    inode_src->inode_pere = inode_dest;

    // Sauvegarder l'état du système de fichiers après l'opération de copie
    sauvegarder_systeme_fichier(fs);
    // Afficher un message de succès
    printf("Déplacement réussi : %s → %s\n", source, destination);
    return 0;
}


/********************************************************************************************************************************** */

/**
 * @brief Affiche le chemin absolu du répertoire courant.
 * 
 * Cette fonction affiche le chemin complet du répertoire courant dans le système de fichiers,
 * en remontant à partir du répertoire courant jusqu'à la racine.
 * 
 * @param fs Le système de fichiers sur lequel l'opération est effectuée.
 * 
 * @return Aucune valeur n'est retournée. L'affichage se fait directement dans la sortie standard.
 * 
 * @note Cette fonction gère le cas particulier de la racine du système de fichiers.
 */
 void pwd(SystemeFichier *fs) {
    // Initialisation du chemin, commence comme une chaîne vide
    char chemin[MAX_PATH_LENGTH] = "";
    // On commence par le répertoire courant
    int inode_actuel = fs->repertoire_courant;

    // Cas particulier : Si le répertoire courant est la racine, afficher simplement "/"
    if (inode_actuel == fs->racine.inode_id) {
        printf("/"); // Cas particulier pour la racine
        return;
    }

    // Tant que nous ne sommes pas à la racine, remonter dans les répertoires parents
    while (inode_actuel != fs->racine.inode_id) {
        // Récupérer l'inode du répertoire actuel
        Inode *inode = &fs->inodes[inode_actuel];

        // Trouver l'inode du répertoire parent
        int parent_inode = inode->inode_pere;
        // Récupérer les données du répertoire parent
        Repertoire *parent_rep = (Repertoire *)&fs->data[fs->inodes[parent_inode].blocs[0] * BLOCK_SIZE];

        // Chercher le nom du répertoire actuel dans la liste des fichiers du répertoire parent
        char nom_rep[NAME_SIZE] = "";
        for (int i = 0; i < parent_rep->nb_fichiers; i++) {
            if (parent_rep->fichiers[i].inode_id == inode_actuel) {
                strcpy(nom_rep, parent_rep->fichiers[i].nom);
                break;
            }
        }

        // Ajouter le nom du répertoire actuel au début du chemin (format /nom_rep)
        char temp[MAX_PATH_LENGTH];
        snprintf(temp, sizeof(temp), "/%s%s", nom_rep, chemin);
        strcpy(chemin, temp);

        // Remonter au répertoire parent
        inode_actuel = parent_inode;
    }

    // Afficher le chemin complet du répertoire courant
    printf("%s", chemin);
}


/************************************************************************************************************************************/




/**
 * @brief Crée un lien dur vers un fichier existant dans un répertoire cible.
 * 
 * Cette fonction crée un lien dur pour un fichier dans le répertoire cible, ce qui signifie que plusieurs noms
 * de fichiers (chemins) peuvent pointer vers le même fichier dans le système de fichiers.
 * 
 * @param fs Le système de fichiers sur lequel l'opération est effectuée.
 * @param chemin_source Le chemin du fichier source pour lequel le lien dur sera créé.
 * @param chemin_cible Le chemin du fichier cible où le lien dur sera créé.
 * 
 * @return Retourne 0 si la création du lien dur a réussi, sinon -1 en cas d'erreur.
 * 
 * @note Cette fonction vérifie que le fichier source n'est pas un répertoire et que le répertoire parent cible existe 
 * avant de créer le lien dur. Le compteur de liens de l'inode source est également mis à jour.
 */
 int creer_lien_hard(SystemeFichier *fs, const char *chemin_source, const char *chemin_cible) {
    // Trouver l'inode du fichier source à partir de son chemin
    int inode_source = trouver_inode_par_chemin(fs, chemin_source);
    if (inode_source == -1) {
        // Si le fichier source n'existe pas, retourner une erreur
        printf("Erreur : Le fichier source n'existe pas.\n");
        return -1;
    }

    // Vérifier si le fichier source est un répertoire (un lien dur ne peut pas être créé pour un répertoire)
    if (fs->inodes[inode_source].est_repertoire) {
        printf("Erreur : Impossible de créer un lien dur vers un répertoire.\n");
        return -1;
    }

    // Extraire le chemin du répertoire parent et le nom du fichier cible à partir du chemin complet du fichier cible
    char chemin_parent[MAX_PATH_LENGTH];
    char nom_fichier[NAME_SIZE];
    strncpy(chemin_parent, chemin_cible, MAX_PATH_LENGTH);
    char *dernier_slash = strrchr(chemin_parent, '/');
    if (dernier_slash == NULL) {
        // Si le chemin cible est invalide (aucun slash trouvé), retourner une erreur
        printf("Erreur : Chemin cible invalide.\n");
        return -1;
    }

    // Séparer le nom du fichier cible du chemin parent
    strcpy(nom_fichier, dernier_slash + 1);
    *dernier_slash = '\0';  // Remplacer le dernier slash par la fin de chaîne pour obtenir le chemin parent

    // Trouver l'inode du répertoire parent
    int inode_parent = trouver_inode_par_chemin(fs, chemin_parent);
    if (inode_parent == -1) {
        // Si le répertoire parent n'existe pas, retourner une erreur
        printf("Erreur : Le répertoire parent n'existe pas.\n");
        return -1;
    }

    // Vérifier si le répertoire parent a suffisamment de place pour ajouter un fichier (maximum de fichiers)
    Repertoire *rep_parent = (Repertoire *)&fs->data[fs->inodes[inode_parent].blocs[0] * BLOCK_SIZE];
    if (rep_parent->nb_fichiers >= MAX_FILES) {
        // Si le répertoire est plein, retourner une erreur
        printf("Erreur : Répertoire plein.\n");
        return -1;
    }

    // Ajouter l'entrée du fichier lié dur dans le répertoire parent
    strncpy(rep_parent->fichiers[rep_parent->nb_fichiers].nom, nom_fichier, NAME_SIZE);
    rep_parent->fichiers[rep_parent->nb_fichiers].inode_id = inode_source;
    rep_parent->nb_fichiers++;  // Augmenter le nombre de fichiers dans le répertoire

    // Incrémenter le compteur de liens de l'inode source pour refléter l'ajout du lien dur
    fs->inodes[inode_source].liens++;
    fs->inodes[inode_source].date_modification = time(NULL);  // Mettre à jour la date de modification de l'inode

    // Afficher un message indiquant que le lien dur a été créé avec succès
    printf("Lien dur créé : %s -> %s\n", chemin_cible, chemin_source);

    // Sauvegarder le système de fichiers après avoir effectué l'opération
    sauvegarder_systeme_fichier(fs);
    
    // Retourner 0 pour indiquer que l'opération a réussi
    return 0;
}

/****************************************************************************************************************************** */


/**
 * @brief Vérifie si l'inode, représenté par son chemin, est un répertoire.
 * 
 * Cette fonction prend en entrée un chemin de fichier ou de répertoire, cherche l'inode correspondant
 * dans le système de fichiers, et vérifie si cet inode est un répertoire.
 * 
 * Si l'inode existe et est un répertoire, la fonction retourne 1. Sinon, elle retourne 0.
 * 
 * @param fs Le système de fichiers sur lequel l'opération est effectuée.
 * @param chemin Le chemin du fichier ou répertoire à vérifier. Il s'agit du chemin absolu ou relatif à partir de la racine.
 * 
 * @return 1 si l'inode est un répertoire, 0 sinon.
 * 
 * @note Cette fonction dépend de la présence d'une fonction externe `trouver_inode_par_chemin` pour 
 * localiser l'inode à partir du chemin fourni.
 */

int est_repertoire_par_chemin(SystemeFichier *fs, const char *chemin) {
    // Trouver l'inode correspondant au chemin
    int inode_id = trouver_inode_par_chemin(fs, chemin);
    if (inode_id == -1) {
        // Si l'inode n'existe pas, retourner 0
        return 0;
    }

    // Vérifier si l'inode trouvé est un répertoire
    if (fs->inodes[inode_id].est_repertoire) {
        return 1;  // C'est un répertoire
    } else {
        return 0;  // Ce n'est pas un répertoire
    }
}

