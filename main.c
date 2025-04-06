/*
   - Amine MOUAICI: 33.33%
   - Leticia ZAID: 33.33%
   - Samy Islem OULED-YOUNES 33.33%
*/

/**
 * @file main.c
 * @brief Système de fichiers simulé - Interface utilisateur en ligne de commande
 *
 * Ce fichier contient l'implémentation d'une interface utilisateur en ligne de commande permettant d'interagir avec un système de fichiers simulé. Les utilisateurs peuvent effectuer diverses opérations sur le système de fichiers, telles que l'affichage des fichiers, la création de répertoires, la manipulation des permissions, la gestion des fichiers et des liens, et bien plus encore.
 *
 * @author Amine MOUAICI , Leticia ZAID  , Samy Islem OULED-YOUNES
 * @date 06/04/2025
 */


#include <unistd.h>
#include <fcntl.h>
#include "Bib/Bib.h"
/**
 * @brief Fonction principale de l'interface utilisateur.
 * 
 * La fonction `main` initialise le système de fichiers, charge ou crée une nouvelle partition, puis permet à l'utilisateur de saisir des commandes. Chaque commande est interprétée et exécutée en fonction des entrées de l'utilisateur. L'interface est de type ligne de commande (CLI), et l'utilisateur peut effectuer des actions telles que :
 * - Afficher le contenu d'un répertoire
 * - Créer un fichier ou un répertoire
 * - Changer de répertoire
 * - Supprimer des fichiers ou répertoires
 * - Créer des liens physiques et symboliques
 * - Modifier les permissions des fichiers
 * - Afficher le chemin actuel
 * - Copier et déplacer des fichiers
 * 
 * @return int Retourne 0 à la fin de l'exécution.
 */


// Interface utilisateur simple (fonction main)
int main() {
    // Créer et initialiser la partition
    SystemeFichier fs;

    if (!charger_systeme_fichier(&fs)) {    // si la partiton n'existe pas 
        initialiser_systeme_fichier(&fs);   //création d'une nouvelle partition 
    }
    
    char command[256];
    char param1[Max_SIZE_PARAM];
    char param2[Max_SIZE_PARAM];
    memset(param1, 0, sizeof(param1));
    memset(param1, 0, sizeof(param2));
    int running = 1;
    
    printf("Système de fichiers initialisé. Tapez 'help' pour voir les commandes disponibles.\n");
    
    while (running) {
        // Afficher l'invite de commande
        printf("\n[root@myfs ");
        pwd(&fs);
        printf("]$ ");
        
        // Lire la commande
        fgets(command, sizeof(command), stdin);
        
        // Supprimer le retour à la ligne
        if (command[strlen(command) - 1] == '\n') {
            command[strlen(command) - 1] = '\0';
        }
        
        // Traiter la commande
        if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) {
            running = 0;
        }
        else if (strcmp(command, "help") == 0) {
            printf("Commandes disponibles:\n");
            printf("  help                 - Affiche cette aide\n");
            printf("  ls                   - Liste le contenu du répertoire courant\n");
            printf("  mkdir nom            - Crée un répertoire\n");
            printf("  touch nom            - Crée un fichier vide\n");
            printf("  cd nom               - Change de répertoire\n");
            printf("  rm nom               - Supprime un fichier ou répertoire\n");
            printf("  ln -s src dst        - Crée un lien symbolique\n");
            printf("  ln  src dst          - Crée un lien physique\n");
            printf("  chmod mode nom       - Change les permissions d'un fichier (mode en octal)\n");
            printf("  pwd                  - Affiche le chemin courant\n");
            printf("  cp src dst           - Copie un fichier\n");
            printf("  mv src dst           - Déplace un fichier (supporte les chemins relatifs et absolus)\n");
            printf("  write nom_fichier    - Création d'un lien symbolique\n");
            printf("  exit                 - Quitte le programme\n");
        }
        else if (strncmp(command, "ls", 2) == 0) {
            
            if(command[2]=='\0'){                  // s'il n'y a pas de paramétre après ls
                afficher_ls_chemin(&fs,".");
            }else{
                if(command[2]==' '){              // Verifier l'existance d'un espace entre ls et le paramétre
                    sscanf(command + 3, "%s", param1);   //récuperation du paramétre
                    afficher_ls_chemin(&fs,param1);          //execution de la commande ls
                }else{
                    printf("Erreur : Syntaxe incorrect.\n"); 
                }
                
            }
            memset(param1, 0, sizeof(param1));  //réanitialiser le parametre
        }
        else if (strncmp(command, "mkdir", 5) == 0) {
            if (command[5] == ' ') {  // Vérifie qu'il y a bien un espace après "mkdir"
                sscanf(command + 6, "%s", param1);  //récuperation du paramétre
                create_file_rep(&fs, param1, 1);  //execution de la commande mkdir
                memset(param1, 0, sizeof(param1));  //réanitialiser le parametre
            } else {
                printf("Erreur : Syntaxe incorrect.\n");
            }
        }
        else if (strncmp(command, "cat", 3) == 0) {
            if (command[3] == ' ') {  // Vérifie qu'il y a bien un espace après "cat"
                sscanf(command + 4, "%s", param1); //récuperation du paramétre
                lire_fichier(&fs, param1);    //execution de la commande cat
                memset(param1, 0, sizeof(param1));  //réanitialiser le parametre
            } else {
                printf("Erreur : Syntaxe incorrect.\n");
            }
        }
        else if (strncmp(command, "touch", 5) == 0) {
            if (command[5] == ' ') {  // Vérifie qu'il y a bien un espace après "touch"
                sscanf(command + 6, "%s", param1); //récuperation du paramétre
                create_file_rep(&fs, param1, 0);  //execution de la commande touch
                memset(param1, 0, sizeof(param1)); //réanitialiser le parametre
            } else {
                printf("Erreur : Syntaxe incorrect.\n");
            }
        }
        else if (strncmp(command, "cd", 2) == 0) {
            if (command[2] == ' ') {  // Vérifie qu'il y a bien un espace après "cd"
                sscanf(command + 3, "%s", param1);    //récuperation du paramétre
                cd(&fs,param1);                      //execution de la commande cd
                memset(param1, 0, sizeof(param1));  //réanitialiser le parametre
            } else {
                printf("Erreur : Syntaxe incorrect.\n");
            }

        }
        else if (strncmp(command, "ln -s", 5) == 0) {
            if (command[5] == ' ') {  // Vérifie qu'il y a bien un espace après "cp"
                if (sscanf(command + 6, "%s %s", param1, param2) == 2) {  // Lire les deux paramètres
                    // Appel de la fonction "ln -s" avec les paramètres extraits
                    creer_lien_symbolique(&fs, param2, param1);
                } else {
                    printf("Erreur : Syntaxe incorrecte.\n");
                }
            } else {
                printf("Erreur : Syntaxe incorrecte.\n");
            }
        }
        else if (strncmp(command, "ln", 2) == 0) {
            if (command[2] == ' ') {  // Vérifie qu'il y a bien un espace après "ln"
                if (sscanf(command + 3, "%s %s", param1, param2) == 2) {  // Lire les deux paramètres
                    // Appel de la fonction cp avec les paramètres extraits
                    creer_lien_hard(&fs, param1, param2);
                } else {
                    printf("Erreur : Syntaxe incorrecte.\n");
                }
            } else {
                printf("Erreur : Syntaxe incorrecte.\n");
            }
        }
        else if (strncmp(command, "rm", 2) == 0) {
            if (command[2] == ' ') {  // Vérifie qu'il y a bien un espace après "mkdir"
                sscanf(command + 3, "%s", param1);  //récuperation du paramétre
                int est_repertoire= est_repertoire_par_chemin(&fs,param1);
                if(est_repertoire==1){
                    supprimer_repertoire(&fs,param1);   //execution de la commande rm pour un répertoire
                }else{
                    supprimer_fichier(&fs,param1);     //execution de la commande rm pour un fichier
                }
               
                memset(param1, 0, sizeof(param1)); //réanitialiser le parametre
            } else {
                printf("Erreur : Syntaxe incorrect.\n");
            }
        }
        else if (strncmp(command, "chmod", 5) == 0) {
            if (command[5] == ' ') {  // Vérifie qu'il y a bien un espace après "cp"
                if (sscanf(command + 6, "%s %s", param1, param2) == 2) {  // Lire les deux paramètres
                    // Appel de la fonction chmod avec les paramètres extraits
                    droit(&fs, param2, param1);
                } else {
                    printf("Erreur : Syntaxe incorrecte.\n");
                }
            } else {
                printf("Erreur : Syntaxe incorrecte.\n");
            }
        }
        else if (strncmp(command, "pwd",3) == 0) {

            if(command[3]=='\0'){   //vérifier qu'il n'y a rien après pwd
                pwd(&fs);          //execution de la fonction pwd
            }else{
                if(command[3]==' '){    //vérifier s'il y a espace apres pwd
                    sscanf(command + 4, "%s", param1);  //lecture de la chaine du charactère apres l'espace
                    if(param1[0]=='\0'){    //si la chaine du charactère apres l'espace est vide  
                        pwd(&fs);           // alors execution de la fonction pwd
                    }
                }else{ 
                    printf("Erreur : Syntaxe incorrect.\n"); 
                }
                
            }
            memset(param1, 0, sizeof(param1));    //réanitialiser le parametre
        }
        else if (strncmp(command, "cp", 2) == 0) {
            if (command[2] == ' ') {  // Vérifie qu'il y a bien un espace après "cp"
                if (sscanf(command + 3, "%s %s", param1, param2) == 2) {  // Lire les deux paramètres
                    // Appel de la fonction cp avec les paramètres extraits
                    cp(&fs, param1, param2);
                } else {
                    printf("Erreur : Syntaxe incorrecte.\n");
                }
            } else {
                printf("Erreur : Syntaxe incorrecte.\n");
            }
        }
        else if (strncmp(command, "write", 5) == 0) {
            if (command[5] == ' ') {  // Vérifie qu'il y a bien un espace après "mkdir"
                sscanf(command + 6, "%s", param1);   //récuperation du paramétre
                ecrire_fichier(&fs, param1);         //execution de la commande write
                memset(param1, 0, sizeof(param1));   //réanitialiser le parametre
            } else {
                printf("Erreur : Syntaxe incorrect.\n");
            }
        }
        else if (strncmp(command, "mv", 2) == 0) {
            if (command[2] == ' ') {  // Vérifie qu'il y a bien un espace après "cp"
                if (sscanf(command + 3, "%s %s", param1, param2) == 2) {  // Lire les deux paramètres
                    // Appel de la fonction mv avec les paramètres extraits
                    mv(&fs, param1, param2);
                } else {
                    printf("Erreur : Syntaxe incorrecte. Usage : cp <source> <destination>\n");
                }
            } else {
                printf("Erreur : Syntaxe incorrecte.\n");
            }
        }
        else {
            printf("Commande inconnue. Tapez 'help' pour voir les commandes disponibles.\n");
        }
    }
    
    // Libérer la mémoire
    sauvegarder_systeme_fichier(&fs);
    
    return 0;
}
