#include <unistd.h>
#include <fcntl.h>
#include "Bib/Bib.h"



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
            printf("  help          - Affiche cette aide\n");
            printf("  ls            - Liste le contenu du répertoire courant\n");
            printf("  mkdir nom     - Crée un répertoire\n");
            printf("  touch nom     - Crée un fichier vide\n");
            printf("  cd nom        - Change de répertoire\n");
            printf("  rm nom        - Supprime un fichier ou répertoire\n");
            printf("  ln -s src dst - Crée un lien symbolique\n");
            printf("  ln  src dst - Crée un lien physique\n");
            printf("  chmod mode nom- Change les permissions d'un fichier (mode en octal)\n");
            printf("  pwd           - Affiche le chemin courant\n");
            printf("  cp src dst    - Copie un fichier\n");
            printf("  mv src dst    - Déplace un fichier (supporte les chemins relatifs et absolus)\n");
            printf("  write cible nom_lien          - Création d'un lien symbolique\n");
            printf("  exit          - Quitte le programme\n");
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
        /*else if (strncmp(command, "lseek", 5) == 0) {
            if (command[5] == ' ') {  // Vérifie qu'il y a bien un espace après "ln"
                if (sscanf(command + 6, "%s %s", param1, param2) == 2) {  // Lire les deux paramètres
                    // Appel de la fonction lseek avec les paramètres extraits
                    int descripteur = open_file(&fs, param1, MODE_WRITE);
                    int offset = atoi(param2);   //generer un entier à partir d'une chaine de caractére
                    int nouveau_offset = lseek_file(&fs, descripteur,offset);
                    if (nouveau_offset != -1)
                    {
                        printf("Nouveau offset : %d\n", nouveau_offset);
                    }
                } else {
                    printf("Erreur : Syntaxe incorrecte.\n");
                }
            } else {
                printf("Erreur : Syntaxe incorrecte.\n");
            }
        }*/
        else if (strncmp(command, "rm", 2) == 0) {
            if (command[2] == ' ') {  // Vérifie qu'il y a bien un espace après "mkdir"
                sscanf(command + 3, "%s", param1);  //récuperation du paramétre
                supprimer_fichier(&fs,param1);     //execution de la commande rm
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