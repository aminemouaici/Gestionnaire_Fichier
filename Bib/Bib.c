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
    for (int i = 0; i < MAX_INODES; i++) {
        if (!BIT_CHECK(fs->bitmap.inodes[i / 8], i % 8)) {
            BIT_SET(fs->bitmap.inodes[i / 8], i % 8);
            fs->superbloc.inode_libres--;
            return i;
        }
    }
    return -1; // Plus d'inodes disponibles
}

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
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (!BIT_CHECK(fs->bitmap.blocs[i / 8], i % 8)) {
            BIT_SET(fs->bitmap.blocs[i / 8], i % 8);
            fs->superbloc.blocs_libres--;
            return i;
        }
    }
    return -1; // Plus de blocs disponibles
}

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
    memset(fs, 0, sizeof(SystemeFichier));

    // Initialisation des bitmaps à zéro
    memset(fs->bitmap.inodes, 0, sizeof(fs->bitmap.inodes));
    memset(fs->bitmap.blocs, 0, sizeof(fs->bitmap.blocs));

    fs->superbloc.taille_fs = PARTITION_SIZE;
    fs->superbloc.nb_inodes = MAX_INODES;
    fs->superbloc.nb_blocs = MAX_BLOCKS / 4;
    fs->superbloc.inode_libres = MAX_INODES;
    fs->superbloc.blocs_libres = MAX_BLOCKS / 4;
    fs->superbloc.premier_bloc_libre = 0;

    // Allouer le répertoire racine
    int inode_racine = allouer_inode(fs);
    int bloc_racine = allouer_bloc(fs);

    if (inode_racine == -1 || bloc_racine == -1) {
        printf("Erreur: Impossible d'allouer le répertoire racine\n");
        exit(EXIT_FAILURE);
    }

    fs->inodes[inode_racine].id = inode_racine;
    fs->inodes[inode_racine].taille = sizeof(Repertoire);
    fs->inodes[inode_racine].est_repertoire = 1;
    fs->inodes[inode_racine].permissions = 0755;
    fs->inodes[inode_racine].liens = 1;
    fs->inodes[inode_racine].blocs[0] = bloc_racine;
    fs->inodes[inode_racine].date_creation = time(NULL);
    fs->inodes[inode_racine].date_modification = fs->inodes[inode_racine].date_creation;
    fs->inodes[inode_racine].inode_pere = inode_racine;
    fs->inodes[inode_racine].est_lien = 0;

    fs->racine.inode_id = inode_racine;

    fs->repertoire_courant = inode_racine; // Initialiser le répertoire courant à la racine
     // Initialiser le répertoire racine

     fs->racine.nb_fichiers = 2;  // . et ..

     // Initialiser les entrées . et .. dans le répertoire racine
     strcpy(fs->racine.fichiers[0].nom, ".");  // Entrée pour le répertoire courant
     fs->racine.fichiers[0].inode_id = inode_racine;  // L'inode du répertoire racine

     strcpy(fs->racine.fichiers[1].nom, "..");  // Entrée pour le répertoire parent
     fs->racine.fichiers[1].inode_id = inode_racine;  // Le parent du répertoire racine est lui-même

     // Marquer l'inode racine comme utilisée dans le bitmap
     BIT_SET(fs->bitmap.inodes[0 / 8], 0 % 8);  // Marquer le premier inode comme utilisé
     fs->superbloc.inode_libres--;  // Décrémenter le nombre d'inodes libres



}

/**
 * @brief Sauvegarde le système de fichiers dans un fichier.
 * 
 * Cette fonction sauvegarde l'état du système de fichiers dans un fichier binaire pour persister 
 * les données entre les exécutions du programme.
 * 
 * @param fs Pointeur vers le système de fichiers à sauvegarder.
 */
void sauvegarder_systeme_fichier(SystemeFichier *fs) {
    FILE *f = fopen(PARTITION_NAME, "wb");
    if (!f) {
        perror("Erreur lors de la sauvegarde de la partition");
        exit(EXIT_FAILURE);
    }
    if (fwrite(fs, sizeof(SystemeFichier), 1, f) != 1) {
        perror("Erreur d'écriture du fichier système");
        fclose(f);
        exit(EXIT_FAILURE);
    }
    fclose(f);

}

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
    FILE *f = fopen(PARTITION_NAME, "rb");
    if (!f) {
        printf("Aucune partition existante, création d'un nouveau système de fichiers.\n");
        return 0;
    }
    if (fread(fs, sizeof(SystemeFichier), 1, f) != 1) {
        perror("Erreur de lecture du fichier système");
        fclose(f);
        return 0;
    }
    fclose(f);
    printf("Système de fichiers chargé depuis %s\n", PARTITION_NAME);
    fs->repertoire_courant = fs->racine.inode_id;
    return 1;
}




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
    if (strcmp(chemin, "/") == 0) return fs->racine.inode_id;

    char chemin_cpy[MAX_PATH_LENGTH];
    strncpy(chemin_cpy, chemin, MAX_PATH_LENGTH);
    chemin_cpy[MAX_PATH_LENGTH - 1] = '\0'; // Sécurité pour éviter les dépassements de tampon

    // Initialisation du point de départ
    int inode_courant = (chemin[0] == '/') ? fs->racine.inode_id : fs->repertoire_courant;



    char *token = strtok(chemin_cpy, "/");
    while (token != NULL) {


        Inode *inode = &fs->inodes[inode_courant];
        if (!inode->est_repertoire) {
            printf("Erreur : %s n'est pas un répertoire.\n", token);
            return -1;
        }

        // Gestion des cas spéciaux . et ..
        if (strcmp(token, ".") == 0) {
            // On reste dans le répertoire courant
        } else if (strcmp(token, "..") == 0) {
            inode_courant = (inode_courant == fs->racine.inode_id) ? fs->racine.inode_id : inode->inode_pere;
        } else {
            // Recherche du fichier/dossier dans le répertoire courant
            Repertoire *rep = (Repertoire *)&fs->data[inode->blocs[0] * BLOCK_SIZE];
            int trouve = 0;
            for (int i = 0; i < rep->nb_fichiers; i++) {
                if (strcmp(rep->fichiers[i].nom, token) == 0) {
                    inode_courant = rep->fichiers[i].inode_id;
                    trouve = 1;
                    break;
                }
            }
            if (!trouve) {
                printf("Erreur : %s introuvable dans le répertoire.\n", token);
                return -1;
            }
        }
        token = strtok(NULL, "/");
    }


    return inode_courant;
}
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
    char chemin_cpy[MAX_PATH_LENGTH];
    strncpy(chemin_cpy, chemin, MAX_PATH_LENGTH);

    // Séparer le chemin en répertoire parent + nom
    char *dernier_slash = strrchr(chemin_cpy, '/');
    char nom[NAME_SIZE];
    const char *chemin_parent;

    if (dernier_slash) {
        strncpy(nom, dernier_slash + 1, NAME_SIZE);
        *dernier_slash = '\0';
        chemin_parent = (*chemin_cpy) ? chemin_cpy : "/";
    } else {
        strncpy(nom, chemin, NAME_SIZE);
        chemin_parent = "";
    }
    printf("Création de %s dans %s\n", nom, chemin_parent);

    // Trouver l'inode du répertoire parent à partir du chemin
    int inode_parent = trouver_inode_par_chemin(fs, chemin_parent);
    if (inode_parent == -1) {
        printf("Erreur : chemin parent %s introuvable.\n", chemin_parent);
        return;
    }
    printf("Inode parent: %d\n", inode_parent);


    Inode *inode_p = &fs->inodes[inode_parent];
    Repertoire *rep_p = (Repertoire *)&fs->data[inode_p->blocs[0] * BLOCK_SIZE];

    // Vérifier si le fichier existe déjà dans le répertoire parent
    for (int i = 0; i < rep_p->nb_fichiers; i++) {
        if (strcmp(rep_p->fichiers[i].nom, nom) == 0) {
            printf("Erreur : %s existe déjà.\n", nom);
            return;
        }
    }
    printf("Vérification de l'existence de %s dans %s\n", nom, chemin_parent);


    // Allouer inode et bloc pour le fichier ou répertoire
    int inode_fichier = allouer_inode(fs);
    int bloc_fichier = allouer_bloc(fs);
    if (inode_fichier == -1 || bloc_fichier == -1) {
        printf("Erreur : ressources insuffisantes pour créer %s\n", nom);
        return;
    }
    printf("Allocation inode: %d, bloc: %d\n", inode_fichier, bloc_fichier);


    Inode *new_inode = &fs->inodes[inode_fichier];
    new_inode->id = inode_fichier;
    new_inode->taille = 0;
    new_inode->est_repertoire = est_repertoire;
    new_inode->permissions = est_repertoire ? 0755 : 0777;  // Répertoire ou fichier avec permissions
    new_inode->liens = 1;
    new_inode->blocs[0] = bloc_fichier;
    new_inode->date_creation = time(NULL);
    new_inode->date_modification = new_inode->date_creation;
    new_inode->inode_pere = inode_parent;
    new_inode->est_lien = 0;

    // Initialiser le contenu du répertoire si c'est un répertoire
    if (est_repertoire) {
        Repertoire *nouveau = (Repertoire *)&fs->data[bloc_fichier * BLOCK_SIZE];
        nouveau->inode_id = inode_fichier;
        nouveau->nb_fichiers = 0;
    }

    // Ajouter le fichier ou répertoire dans le répertoire parent
    strncpy(rep_p->fichiers[rep_p->nb_fichiers].nom, nom, NAME_SIZE);
    rep_p->fichiers[rep_p->nb_fichiers].inode_id = inode_fichier;
    rep_p->nb_fichiers++;

    printf("%s %s créé avec succès.\n", est_repertoire ? "Répertoire" : "Fichier", chemin);
    sauvegarder_systeme_fichier(fs);
}


/**
 * Trouve l'inode correspondant à un chemin pour la commande "cd".
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
    if (!chemin || strlen(chemin) == 0) return -1;

    // Déterminer le point de départ (absolu ou relatif)
    int inode_courant = (chemin[0] == '/') ? fs->racine.inode_id : fs->repertoire_courant;

    // Copier le chemin pour le tokeniser
    char chemin_cpy[MAX_PATH_LENGTH];
    strncpy(chemin_cpy, chemin, MAX_PATH_LENGTH - 1);
    chemin_cpy[MAX_PATH_LENGTH - 1] = '\0';

    char *token = strtok(chemin_cpy, "/");

    while (token != NULL) {
        if (strcmp(token, ".") == 0) {
            // "." ne change rien, on continue
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
        if (!inode->est_repertoire) return -1; // On ne peut pas parcourir un fichier

        // Charger le répertoire
        Repertoire *rep = (Repertoire *)&fs->data[inode->blocs[0] * BLOCK_SIZE];

        // Rechercher le prochain élément du chemin
        int trouve = 0;
        for (int i = 0; i < rep->nb_fichiers; i++) {
            if (strcmp(rep->fichiers[i].nom, token) == 0) {
                inode_courant = rep->fichiers[i].inode_id;
                trouve = 1;
                break;
            }
        }
        if (!trouve) return -1; // Chemin invalide

        token = strtok(NULL, "/");
    }

    return inode_courant;
}
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
        printf("Déjà dans le répertoire courant.\n");
        return;
    }

    if (strcmp(chemin, "..") == 0) {
        if (fs->repertoire_courant == fs->racine.inode_id) {
            printf("Déjà à la racine, impossible de remonter.\n");
            return;
        }

        // Trouver le parent en parcourant la structure (pas d'info directe)
        for (int i = 0; i < MAX_INODES; i++) {
            if (fs->inodes[i].est_repertoire) {
                Repertoire *rep = (Repertoire *)&fs->data[fs->inodes[i].blocs[0] * BLOCK_SIZE];
                for (int j = 0; j < rep->nb_fichiers; j++) {
                    if (rep->fichiers[j].inode_id == fs->repertoire_courant) {
                        fs->repertoire_courant = i;  // Trouvé, on remonte au parent
                        printf("Remonté au répertoire parent.\n");
                        return;
                    }
                }
            }
        }

        printf("Erreur : impossible de trouver le répertoire parent.\n");
        return;
    }

    // Trouver l'inode du chemin demandé
    //int inode_cible = trouver_inode_par_cheminCd(fs, chemin);
    int inode_cible = resoudre_lien_symbolique(fs, chemin);
    if (inode_cible == -1 || !fs->inodes[inode_cible].est_repertoire) {
        printf("Erreur : répertoire introuvable : %s\n", chemin);
        return;
    }

    // Changement du répertoire courant
    fs->repertoire_courant = inode_cible;
    printf("Répertoire courant changé vers : %s\n", chemin);
}
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
    if (droits[0] < '0' || droits[0] > '7' || droits[1] < '0' || droits[1] > '7' || droits[2] < '0' || droits[2] > '7') {
        printf("Erreur : Chaque chiffre des droits doit être compris entre 0 et 7.\n");
        return;
    }

    // Trouver l'inode correspondant au chemin
    int inode_index = trouver_inode_par_cheminCd(fs, chemin);
    if (inode_index == -1) {
        printf("Erreur : Fichier ou dossier non trouvé\n");
        return;
    }

    Inode *inode_courant = &fs->inodes[inode_index];

    printf("Fichier trouvé : %s\n", chemin);
    printf("Permissions actuelles : %o (octal)\n", inode_courant->permissions);

    // Convertir en entier octal
    int droits_octal = strtol(droits, NULL, 8);
    inode_courant->permissions = droits_octal;

    printf("Permissions après modification : %o (octal)\n", inode_courant->permissions);
}

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

void supprimer_fichier(SystemeFichier *fs, const char *chemin)
{
    // Trouver le répertoire parent et le nom du fichier
    char chemin_cpy[MAX_PATH_LENGTH];
    strncpy(chemin_cpy, chemin, MAX_PATH_LENGTH);
    char *dernier_slash = strrchr(chemin_cpy, '/');
    char nom[NAME_SIZE];
    const char *chemin_parent;

    if (dernier_slash)
    {
        strncpy(nom, dernier_slash + 1, NAME_SIZE);
        *dernier_slash = '\0';
        chemin_parent = (*chemin_cpy) ? chemin_cpy : "/";
    }
    else
    {
        strncpy(nom, chemin, NAME_SIZE);
        chemin_parent = "";
    }

    // Trouver l'inode du parent
    int inode_parent = trouver_inode_par_chemin(fs, chemin_parent);
    if (inode_parent == -1)
    {
        printf("Erreur : chemin parent %s introuvable.\n", chemin_parent);
        return;
    }

    Inode *inode_p = &fs->inodes[inode_parent];
    Repertoire *rep_p = (Repertoire *)&fs->data[inode_p->blocs[0] * BLOCK_SIZE];

    // Chercher le fichier à supprimer
    int fichier_trouve = -1;
    for (int i = 0; i < rep_p->nb_fichiers; i++)
    {
        if (strcmp(rep_p->fichiers[i].nom, nom) == 0)
        {
            fichier_trouve = i;
            break;
        }
    }

    if (fichier_trouve == -1)
    {
        printf("Erreur : fichier %s introuvable.\n", nom);
        return;
    }

    // Libérer le bloc et l'inode du fichier
    int inode_fichier = rep_p->fichiers[fichier_trouve].inode_id;
    int bloc_fichier = fs->inodes[inode_fichier].blocs[0];

    // Libérer l'inode et le bloc
    BIT_CLEAR(fs->bitmap.inodes[inode_fichier / 8], inode_fichier % 8);
    BIT_CLEAR(fs->bitmap.blocs[bloc_fichier / 8], bloc_fichier % 8);
    fs->superbloc.inode_libres++;
    fs->superbloc.blocs_libres++;

    // Supprimer l'entrée du fichier dans le répertoire
    for (int i = fichier_trouve; i < rep_p->nb_fichiers - 1; i++)
    {
        rep_p->fichiers[i] = rep_p->fichiers[i + 1];
    }
    rep_p->nb_fichiers--;

    printf("Fichier %s supprimé avec succès.\n", nom);
}


// Fonction pour supprimer un répertoire donné (chemin absolu ou répertoire courant)
void supprimer_repertoire(SystemeFichier *fs, const char *chemin)
{
    int inode_a_supprimer;
    // Si un chemin est donné, on utilise ce chemin, sinon on prend le répertoire courant
    if (chemin && strlen(chemin) > 0)
    {
        inode_a_supprimer = trouver_inode_par_chemin(fs, chemin); // Trouver le répertoire à supprimer
        if (inode_a_supprimer == -1)
        {
            printf("Erreur : répertoire %s introuvable.\n", chemin);
            return;
        }
    }
    else
    {
        inode_a_supprimer = fs->repertoire_courant; // Sinon on supprime le répertoire courant
    }

    Inode *inode_r = &fs->inodes[inode_a_supprimer];
    Repertoire *rep_r = (Repertoire *)&fs->data[inode_r->blocs[0] * BLOCK_SIZE];

    // Vérifier si le répertoire est vide
    if (inode_r->liens > 2)
    { // Si le répertoire a plus de 2 liens, ce n'est pas vide
        // Suppression récursive des sous-répertoires et fichiers
        for (int i = 0; i < rep_r->nb_fichiers; i++)
        {
            int inode_fichier = rep_r->fichiers[i].inode_id;
            Inode *fichier_inode = &fs->inodes[inode_fichier];

            // Déclaration de chemin_sousdossier
            char chemin_sousdossier[MAX_PATH_LENGTH];

            if (fichier_inode->est_repertoire)
            {
                snprintf(chemin_sousdossier, MAX_PATH_LENGTH, "%s/%s", chemin ? chemin : ".", rep_r->fichiers[i].nom);
                supprimer_repertoire(fs, chemin_sousdossier); // Appel récursif pour supprimer les sous-dossiers
            }
            else
            {
                snprintf(chemin_sousdossier, MAX_PATH_LENGTH, "%s/%s", chemin ? chemin : ".", rep_r->fichiers[i].nom);
                supprimer_fichier(fs, chemin_sousdossier); // Supprimer le fichier
            }
        }
    }

    // Libérer l'inode et le bloc du répertoire
    int bloc_repertoire = inode_r->blocs[0];
    BIT_CLEAR(fs->bitmap.inodes[inode_a_supprimer / 8], inode_a_supprimer % 8);
    BIT_CLEAR(fs->bitmap.blocs[bloc_repertoire / 8], bloc_repertoire % 8);
    fs->superbloc.inode_libres++;
    fs->superbloc.blocs_libres++;

    // Supprimer l'entrée du répertoire dans le répertoire parent
    int inode_parent = fs->racine.inode_id; // Par défaut, on suppose que le répertoire parent est racine
    Inode *inode_p = &fs->inodes[inode_parent];
    Repertoire *rep_p = (Repertoire *)&fs->data[inode_p->blocs[0] * BLOCK_SIZE];

    // Chercher et supprimer l'entrée du répertoire dans le répertoire parent
    int repertoire_trouve = -1;
    for (int i = 0; i < rep_p->nb_fichiers; i++)
    {
        if (rep_p->fichiers[i].inode_id == inode_a_supprimer)
        {
            repertoire_trouve = i;
            break;
        }
    }

    if (repertoire_trouve != -1)
    {
        for (int i = repertoire_trouve; i < rep_p->nb_fichiers - 1; i++)
        {
            rep_p->fichiers[i] = rep_p->fichiers[i + 1];
        }
        rep_p->nb_fichiers--;
    }

    printf("Répertoire %s supprimé avec succès.\n", chemin ? chemin : "répertoire courant");
}

int verifier_permissions(Inode *inode, int mode)
{
    if (mode == MODE_READ)
    {                                                                    // Mode lecture
        return inode->permissions == 0755 || inode->permissions == 0777; // Vérifier si la permission de lecture (r) est donnée
    }
    else if (mode == MODE_WRITE)
    {                                                                    // Mode écriture
        return inode->permissions == 0755 || inode->permissions == 0777; // Vérifier si la permission d'écriture (w) est donnée
    }
    return 0;
}

int open_file(SystemeFichier *fs, const char *chemin, int mode)
{
    // Trouver l'inode du fichier
    int inode_id = trouver_inode_par_chemin(fs, chemin);
    if (inode_id == -1)
    {
        printf("Erreur : fichier %s introuvable.\n", chemin);
        return -1;
    }

    Inode *inode = &fs->inodes[inode_id];

    // Vérifier les permissions d'accès
    if ((mode == MODE_READ && !verifier_permissions(inode, MODE_READ)) ||
        (mode == MODE_WRITE && !verifier_permissions(inode, MODE_WRITE)))
    {
        printf("Erreur : permission d'accès refusée pour le fichier %s.\n", chemin);
        return -1;
    }

    // Chercher un espace libre pour ouvrir le fichier
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (fs->racine.fichiers[i].inode_id == 0)
        {
            // Initialiser un descripteur de fichier
            fs->racine.fichiers[i].inode_id = inode_id;
            fs->racine.fichiers[i].nom[0] = chemin[0]; // Ajoutez plus de détails ici pour gérer correctement le nom
            fs->racine.fichiers[i].inode_id = mode;    // Mettre le mode d'ouverture (lecture/écriture)
            printf("Fichier %s ouvert avec succès en mode %d.\n", chemin, mode);
            return i;
        }
    }

    // Si aucun espace libre n'est trouvé
    printf("Erreur : trop de fichiers ouverts.\n");
    return -1;
}

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

// Fonction pour écrire dans un fichier
void ecrire_fichier(SystemeFichier *fs, const char *chemin)
{
    // Ouvrir le fichier en mode écriture
    int descripteur = open_file(fs, chemin, MODE_WRITE);
    if (descripteur == -1)
    {
        return; // Erreur lors de l'ouverture
    }

    // // Trouver l'inode du fichier

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
    char *donnees = malloc(BLOCK_SIZE); // Allocation pour le buffer de données
    if (donnees == NULL)
    {
        printf("Erreur : impossible d'allouer de la mémoire.\n");
        close_file(fs, descripteur); // Fermer le fichier avant de quitter
        return;
    }

    // Ajouter l'ancien contenu à l'entrée (pour ne pas écraser)
    strncpy(donnees, &fs->data[inode->blocs[0] * BLOCK_SIZE], BLOCK_SIZE);

    // Lire les nouvelles données à ajouter

    int taille_donnees = 0;
    while (fgets(buffer, BLOCK_SIZE, stdin) != NULL) {
        
    
        // Vérifie si CTRL+X (ASCII 24) est présent dans la ligne
        if (strchr(buffer, 24) != NULL) {
            printf("CTRL+X détecté. Arrêt de la saisie.\n");
            break;
        }

        int len = strlen(buffer);
        if (taille_donnees + len >= BLOCK_SIZE * 10 - 1) {
            printf("Erreur : taille maximale de fichier atteinte.\n");
            break;
        }
    
        // Ajout des données AVANT de vérifier CTRL+X
        strcat(donnees, buffer);
        taille_donnees += len;
    }


    // Si les données dépassent la taille d'un bloc, on gère plusieurs blocs
    if (taille_donnees > BLOCK_SIZE)
    {
        int blocs_necessaires = (taille_donnees / BLOCK_SIZE) + (taille_donnees % BLOCK_SIZE != 0);

        for (int i = 1; i < blocs_necessaires; i++)
        {
            inode->blocs[i] = allouer_bloc(fs); // Allouer un nouveau bloc
        }

        // Écrire les données dans les blocs
        for (int i = 0; i < blocs_necessaires; i++)
        {
            int start = i * BLOCK_SIZE;
            int end = (i + 1) * BLOCK_SIZE;

            // Si c'est le dernier bloc, on écrit uniquement la partie restante des données
            if (end > taille_donnees)
                end = taille_donnees;

            strncpy(&fs->data[inode->blocs[i] * BLOCK_SIZE], donnees + start, end - start);
        }
    }
    else
    {
        // Si tout tient dans un seul bloc
        int bloc_fichier = inode->blocs[0];
        strncpy(&fs->data[bloc_fichier * BLOCK_SIZE], donnees, BLOCK_SIZE);
    }

    inode->taille = taille_donnees;        // Met à jour la taille du fichier
    inode->date_modification = time(NULL); // Mise à jour de la date de modification

    // Afficher le contenu ajouté
    printf("Données ajoutées au fichier %s :\n%s\n", chemin, donnees);
    sauvegarder_systeme_fichier(fs);

    // Libérer la mémoire allouée
    free(donnees);

    // Fermer le fichier après l'écriture
    close_file(fs, descripteur);

    printf("Écriture terminée et fichier fermé.\n");
}


// Fonction pour lire le contenu d'un fichier
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
    int bloc_fichier = inode->blocs[0];
    printf("Contenu du fichier %s :\n%s\n", chemin, &fs->data[bloc_fichier * BLOCK_SIZE]);

    printf("Fichier lu avec succès.\n");
}



// Fonction pour créer un lien symbolique
void creer_lien_symbolique(SystemeFichier *fs, const char *chemin, const char *cible) {
    char chemin_cpy[MAX_PATH_LENGTH];
    strncpy(chemin_cpy, chemin, MAX_PATH_LENGTH);

    // Séparer le chemin en répertoire parent + nom du lien
    char *dernier_slash = strrchr(chemin_cpy, '/');
    char nom[NAME_SIZE];
    const char *chemin_parent;

    if (dernier_slash) {
        strncpy(nom, dernier_slash + 1, NAME_SIZE);
        *dernier_slash = '\0';
        chemin_parent = (*chemin_cpy) ? chemin_cpy : "/";
    } else {
        strncpy(nom, chemin, NAME_SIZE);
        chemin_parent = "";
    }
    printf("Création du lien symbolique %s dans %s, cible : %s\n", nom, chemin_parent, cible);

    // Trouver l'inode du répertoire parent à partir du chemin
    int inode_parent = trouver_inode_par_chemin(fs, chemin_parent);
    if (inode_parent == -1) {
        printf("Erreur : chemin parent %s introuvable.\n", chemin_parent);
        return;
    }
    printf("Inode parent: %d\n", inode_parent);

    Inode *inode_p = &fs->inodes[inode_parent];
    Repertoire *rep_p = (Repertoire *)&fs->data[inode_p->blocs[0] * BLOCK_SIZE];

    // Vérifier si le lien symbolique existe déjà dans le répertoire parent
    for (int i = 0; i < rep_p->nb_fichiers; i++) {
        if (strcmp(rep_p->fichiers[i].nom, nom) == 0) {
            printf("Erreur : %s existe déjà.\n", nom);
            return;
        }
    }

    // Allouer inode pour le lien symbolique
    int inode_lien = allouer_inode(fs);
    if (inode_lien == -1) {
        printf("Erreur : ressources insuffisantes pour créer le lien symbolique %s\n", nom);
        return;
    }

    // Allouer un bloc pour le lien symbolique
    int bloc_lien = allouer_bloc(fs);
    if (bloc_lien == -1) {
        printf("Erreur : ressources insuffisantes pour créer le bloc du lien symbolique %s\n", nom);
        return;
    }

    printf("Allocation inode: %d, bloc: %d pour le lien symbolique\n", inode_lien, bloc_lien);

    // Initialiser l'inode du lien symbolique
    Inode *new_inode = &fs->inodes[inode_lien];
    new_inode->id = inode_lien;
    new_inode->taille = strlen(cible) + 1;  // La taille du lien symbolique est la taille de la chaîne cible
    new_inode->est_repertoire = 0;  // Ce n'est pas un répertoire
    new_inode->permissions = 0777;  // Permissions pour tous
    new_inode->liens = 1;
    new_inode->blocs[0] = bloc_lien;
    new_inode->date_creation = time(NULL);
    new_inode->date_modification = new_inode->date_creation;
    new_inode->inode_pere = inode_parent;
    new_inode->est_lien = 1;

    // Initialiser le contenu du bloc du lien symbolique avec la cible
    char *lien_data = (char *)&fs->data[bloc_lien * BLOCK_SIZE];
    strncpy(lien_data, cible, BLOCK_SIZE);

    // Ajouter le lien symbolique dans le répertoire parent
    Repertoire *rep_parent = (Repertoire *)&fs->data[inode_p->blocs[0] * BLOCK_SIZE];
    if (rep_parent->nb_fichiers >= MAX_FILES) {
        printf("Erreur : le répertoire parent est plein, impossible d'ajouter le lien symbolique.\n");
        return;
    }

    strncpy(rep_parent->fichiers[rep_parent->nb_fichiers].nom, nom, NAME_SIZE);
    rep_parent->fichiers[rep_parent->nb_fichiers].inode_id = inode_lien;
    rep_parent->nb_fichiers++;

    printf("Lien symbolique %s créé avec succès dans %s\n", nom, chemin_parent);
    printf("Inode créé pour %s : id = %d, est_lien = %d\n", nom, inode_lien, new_inode->est_lien);


}

// Fonction pour résoudre un lien symbolique
int resoudre_lien_symbolique(SystemeFichier *fs, const char *chemin) {
    int inode_id = trouver_inode_par_chemin(fs, chemin);
    if (inode_id == -1) {
        printf("Erreur : le chemin %s n'existe pas.\n", chemin);
        return -1;
    }

    Inode *inode = &fs->inodes[inode_id];

    printf("Vérification de inode->est_lien pour %s : %d\n", chemin, inode->est_lien);

    if (!inode->est_lien) {
        // Ici, ce n'est pas une erreur, on retourne l'inode trouvé.
        printf("Le chemin %s est un fichier ou un répertoire normal.\n", chemin);
        return inode_id;
    }

    // Lire la cible du lien symbolique
    char cible[MAX_PATH_LENGTH];
    char *lien_data = (char *)&fs->data[inode->blocs[0] * BLOCK_SIZE];
    strncpy(cible, lien_data, MAX_PATH_LENGTH);

    printf("Résolution du lien symbolique %s : cible = %s\n", chemin, cible);

    // Résolution récursive
    return resoudre_lien_symbolique(fs, cible);
}


void afficher_cible_lien_symbolique(SystemeFichier *fs, const char *chemin_lien) {
    // Récupérer le nom du lien et le répertoire parent
    char chemin_lien_cpy[MAX_PATH_LENGTH];
    strncpy(chemin_lien_cpy, chemin_lien, MAX_PATH_LENGTH);

    char *dernier_slash = strrchr(chemin_lien_cpy, '/');
    char nom[NAME_SIZE];
    const char *chemin_parent;

    if (dernier_slash) {
        strncpy(nom, dernier_slash + 1, NAME_SIZE);
        *dernier_slash = '\0';
        chemin_parent = (*chemin_lien_cpy) ? chemin_lien_cpy : "/";
    } else {
        strncpy(nom, chemin_lien, NAME_SIZE);
        chemin_parent = "";
    }

    // Trouver l'inode du répertoire parent
    int inode_parent = trouver_inode_par_chemin(fs, chemin_parent);
    if (inode_parent == -1) {
        printf("Erreur : chemin parent %s introuvable.\n", chemin_parent);
        return;
    }

    Inode *inode_p = &fs->inodes[inode_parent];
    Repertoire *rep_p = (Repertoire *)&fs->data[inode_p->blocs[0] * BLOCK_SIZE];

    // Chercher le fichier dans le répertoire
    int inode_lien = -1;
    for (int i = 0; i < rep_p->nb_fichiers; i++) {
        if (strcmp(rep_p->fichiers[i].nom, nom) == 0) {
            inode_lien = rep_p->fichiers[i].inode_id;
            break;
        }
    }

    if (inode_lien == -1) {
        printf("Erreur : %s n'existe pas dans le répertoire %s.\n", nom, chemin_parent);
        return;
    }

    Inode *inode_l = &fs->inodes[inode_lien];

    // Vérifier si c'est bien un lien symbolique
    if (inode_l->est_repertoire) {
        printf("Erreur : %s n'est pas un lien symbolique.\n", chemin_lien);
        return;
    }

    // Lire la cible du lien symbolique
    LienSymbolique *lien = (LienSymbolique *)&fs->data[inode_l->blocs[0] * BLOCK_SIZE];
    printf("Le lien symbolique %s pointe vers : %s\n", chemin_lien, lien->cible);
}

int mv(SystemeFichier *fs, const char *source, const char *destination) {
    // Trouver l'inode source
    int inode_source = trouver_inode_par_cheminCd(fs, source);
    if (inode_source == -1) {
        printf("Erreur : Le fichier/répertoire source n'existe pas.\n");
        return -1;
    }

    // Trouver l'inode destination
    int inode_dest = trouver_inode_par_cheminCd(fs, destination);
    if (inode_dest == -1) {
        printf("Erreur : Le dossier de destination n'existe pas.\n");
        return -1;
    }

    // Récupérer les inodes correspondants
    Inode *inode_src = &fs->inodes[inode_source];
    Inode *inode_dst = &fs->inodes[inode_dest];

    // Vérifier que la destination est bien un répertoire
    if (!inode_dst->est_repertoire) {
        printf("Erreur : La destination doit être un répertoire.\n");
        return -1;
    }

    // Trouver le parent de la source pour la supprimer de son répertoire
    Inode *inode_parent_src = &fs->inodes[inode_src->inode_pere];
    Repertoire *rep_parent_src = (Repertoire *)&fs->data[inode_parent_src->blocs[0] * BLOCK_SIZE];
    const char * src_name;
    // Retirer la source du répertoire parent
    for (int i = 0; i < rep_parent_src->nb_fichiers; i++) {
        if (rep_parent_src->fichiers[i].inode_id == inode_source) {
            src_name=rep_parent_src->fichiers[i].nom;
            // Décalage pour supprimer l'entrée du fichier
            for (int j = i; j < rep_parent_src->nb_fichiers - 1; j++) {
                rep_parent_src->fichiers[j] = rep_parent_src->fichiers[j + 1];
            }
            rep_parent_src->nb_fichiers--;
            break;
        }
    }

    // Ajouter la source dans le répertoire de destination
    Repertoire *rep_dest = (Repertoire *)&fs->data[inode_dst->blocs[0] * BLOCK_SIZE];

    if (rep_dest->nb_fichiers >= MAX_FILES) {
        printf("Erreur : Le répertoire de destination est plein.\n");
        return -1;
    }

    // Ajouter le fichier/répertoire dans la destination
    strncpy(rep_dest->fichiers[rep_dest->nb_fichiers].nom, src_name, NAME_SIZE);
    rep_dest->fichiers[rep_dest->nb_fichiers].inode_id = inode_source;
    rep_dest->nb_fichiers++;

    // Mettre à jour l'inode de la source pour pointer vers le nouveau parent
    inode_src->inode_pere = inode_dest;
    sauvegarder_systeme_fichier(fs);
    printf("Déplacement réussi : %s → %s\n", source, destination);
    return 0;
}

int cp(SystemeFichier *fs, const char *source, const char *destination) {
    // Trouver l'inode source
    int inode_source = trouver_inode_par_cheminCd(fs, source);
    if (inode_source == -1) {
        printf("Erreur : Le fichier/répertoire source n'existe pas.\n");
        return -1;
    }

    // Trouver l'inode destination
    int inode_dest = trouver_inode_par_cheminCd(fs, destination);
    if (inode_dest == -1) {
        printf("Erreur : Le dossier de destination n'existe pas.\n");
        return -1;
    }

    // Récupérer les inodes correspondants
    Inode *inode_src = &fs->inodes[inode_source];
    Inode *inode_dst = &fs->inodes[inode_dest];

    // Vérifier que la destination est bien un répertoire
    if (!inode_dst->est_repertoire) {
        printf("Erreur : La destination doit être un répertoire.\n");
        return -1;
    }

    // Trouver le parent de la source pour la supprimer de son répertoire
    Inode *inode_parent_src = &fs->inodes[inode_src->inode_pere];
    Repertoire *rep_parent_src = (Repertoire *)&fs->data[inode_parent_src->blocs[0] * BLOCK_SIZE];
    const char * src_name;
    // Retirer la source du répertoire parent
    for (int i = 0; i < rep_parent_src->nb_fichiers; i++) {
        if (rep_parent_src->fichiers[i].inode_id == inode_source) {
            src_name=rep_parent_src->fichiers[i].nom;
            break;
        }
    }

    // Ajouter la source dans le répertoire de destination
    Repertoire *rep_dest = (Repertoire *)&fs->data[inode_dst->blocs[0] * BLOCK_SIZE];

    if (rep_dest->nb_fichiers >= MAX_FILES) {
        printf("Erreur : Le répertoire de destination est plein.\n");
        return -1;
    }

    // Ajouter le fichier/répertoire dans la destination
    strncpy(rep_dest->fichiers[rep_dest->nb_fichiers].nom, src_name, NAME_SIZE);
    rep_dest->fichiers[rep_dest->nb_fichiers].inode_id = inode_source;
    rep_dest->nb_fichiers++;

    // Mettre à jour l'inode de la source pour pointer vers le nouveau parent
    inode_src->inode_pere = inode_dest;

    sauvegarder_systeme_fichier(fs);
    printf("Déplacement réussi : %s → %s\n", source, destination);
    return 0;
}

void pwd(SystemeFichier *fs) {
    char chemin[MAX_PATH_LENGTH] = "";
    int inode_actuel = fs->repertoire_courant;

    if (inode_actuel == fs->racine.inode_id) {
        printf("/"); // Cas particulier pour la racine
        return;
    }

    while (inode_actuel != fs->racine.inode_id) {
        Inode *inode = &fs->inodes[inode_actuel];

        // Trouver le répertoire parent
        int parent_inode = inode->inode_pere;
        Repertoire *parent_rep = (Repertoire *)&fs->data[fs->inodes[parent_inode].blocs[0] * BLOCK_SIZE];

        // Trouver le nom du répertoire actuel dans le parent
        char nom_rep[NAME_SIZE] = "";
        for (int i = 0; i < parent_rep->nb_fichiers; i++) {
            if (parent_rep->fichiers[i].inode_id == inode_actuel) {
                strcpy(nom_rep, parent_rep->fichiers[i].nom);
                break;
            }
        }

        // Insérer le nom du répertoire au début du chemin
        char temp[MAX_PATH_LENGTH];
        snprintf(temp, sizeof(temp), "/%s%s", nom_rep, chemin);
        strcpy(chemin, temp);

        // Remonter au répertoire parent
        inode_actuel = parent_inode;
    }

    printf("%s", chemin);
}

// Fonction pour créer un lien dur
int creer_lien_hard(SystemeFichier *fs, const char *chemin_source, const char *chemin_cible) {
    // Trouver l'inode de la source
    int inode_source = trouver_inode_par_chemin(fs, chemin_source);
    if (inode_source == -1) {
        printf("Erreur : Le fichier source n'existe pas.\n");
        return -1;
    }

    // Vérifier que la source n'est pas un répertoire
    if (fs->inodes[inode_source].est_repertoire) {
        printf("Erreur : Impossible de créer un lien dur vers un répertoire.\n");
        return -1;
    }

    // Trouver le répertoire parent de la cible
    char chemin_parent[MAX_PATH_LENGTH];
    char nom_fichier[NAME_SIZE];
    strncpy(chemin_parent, chemin_cible, MAX_PATH_LENGTH);
    char *dernier_slash = strrchr(chemin_parent, '/');
    if (dernier_slash == NULL) {
        printf("Erreur : Chemin cible invalide.\n");
        return -1;
    }

    // Séparer le nom du fichier et son chemin parent
    strcpy(nom_fichier, dernier_slash + 1);
    *dernier_slash = '\0';

    // Trouver l'inode du répertoire parent
    int inode_parent = trouver_inode_par_chemin(fs, chemin_parent);
    if (inode_parent == -1) {
        printf("Erreur : Le répertoire parent n'existe pas.\n");
        return -1;
    }

    // Vérifier que le répertoire parent a de la place
    Repertoire *rep_parent = (Repertoire *)&fs->data[fs->inodes[inode_parent].blocs[0] * BLOCK_SIZE];
    if (rep_parent->nb_fichiers >= MAX_FILES) {
        printf("Erreur : Répertoire plein.\n");
        return -1;
    }

    // Ajouter l'entrée du lien dur dans le répertoire parent
    strncpy(rep_parent->fichiers[rep_parent->nb_fichiers].nom, nom_fichier, NAME_SIZE);
    rep_parent->fichiers[rep_parent->nb_fichiers].inode_id = inode_source;
    rep_parent->nb_fichiers++;

    // Augmenter le compteur de liens de l'inode source
    fs->inodes[inode_source].liens++;
    fs->inodes[inode_source].date_modification = time(NULL);

    printf("Lien dur créé : %s -> %s\n", chemin_cible, chemin_source);
    sauvegarder_systeme_fichier(fs);
    return 0;
}

/*int lseek_file(SystemeFichier *fs, int descripteur, int offset)
{
    // Vérifier si le descripteur est valide
    if (descripteur < 0 || descripteur >= MAX_OPEN_FILES || fs->racine.fichiers[descripteur].inode_id == 0)
    {
        printf("Erreur : descripteur de fichier invalide.\n");
        return -1;
    }

    FichierOuvert *fichier = &fs->racine.fichiers[descripteur];

    // Vérifier si le fichier est ouvert en mode lecture/écriture
    if (fichier->mode != MODE_READ && fichier->mode != MODE_WRITE)
    {
        printf("Erreur : fichier non ouvert en mode lecture ou écriture.\n");
        return -1;
    }

    // Trouver l'inode du fichier
    Inode *inode = &fs->inodes[fichier->inode_id];

    // Si l'offset est inférieur à 0, c'est invalide
    if (offset < 0)
    {
        printf("Erreur : l'offset ne peut pas être inférieur à 0.\n");
        return -1;
    }

    // Si l'offset dépasse la taille du fichier, ajustez la taille du fichier pour l'ajouter
    if (offset > inode->taille)
    {
        // Si on dépasse la taille actuelle du fichier, cela signifie que l'on veut ajouter des données.
        // Vous pouvez ajuster ici pour ajouter de nouveaux blocs ou juste augmenter la taille du fichier.
        inode->taille = offset;
    }

    // Mettre à jour l'offset
    fichier->offset = offset;

    // Retourner la nouvelle position
    printf("Nouveau offset du fichier : %d\n", fichier->offset);
    return fichier->offset;
}*/
