#include <unistd.h>
#include <fcntl.h>
#include "Bib/Bib.h"



// Interface utilisateur simple (fonction main)
int main() {
    // Créer et initialiser la partition
    SystemeFichier fs;

    if (!charger_systeme_fichier(&fs)) {
        initialiser_systeme_fichier(&fs);
    }
    
    char command[256];
    char param1[Max_SIZE_PARAM];
    char param2[Max_SIZE_PARAM];
    memset(param1, 0, sizeof(param1));
    memset(param1, 0, sizeof(param2));
    int running = 1;
    
    //printf("Système de fichiers initialisé. Tapez 'help' pour voir les commandes disponibles.\n");
    
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
            printf("  chmod mode nom- Change les permissions d'un fichier (mode en octal)\n");
            printf("  pwd           - Affiche le chemin courant\n");
            printf("  cp src dst    - Copie un fichier\n");
            printf("  mv src dst    - Déplace un fichier (supporte les chemins relatifs et absolus)\n");
            printf("  exit          - Quitte le programme\n");
        }
        else if (strncmp(command, "ls", 2) == 0) {
            
            if(command[2]=='\0'){
                afficher_ls_chemin(&fs,".");
            }else{
                if(command[2]==' '){
                    sscanf(command + 3, "%s", param1);
                    afficher_ls_chemin(&fs,param1);
                }else{
                    printf("Erreur : Syntaxe incorrect.\n"); 
                }
                
            }
            memset(param1, 0, sizeof(param1));
        }
        else if (strncmp(command, "mkdir", 5) == 0) {
            if (command[5] == ' ') {  // Vérifie qu'il y a bien un espace après "mkdir"
                sscanf(command + 6, "%s", param1);
                create_file_rep(&fs, param1, 1);
                memset(param1, 0, sizeof(param1));
            } else {
                printf("Erreur : Syntaxe incorrect.\n");
            }
        }
        else if (strncmp(command, "cat", 3) == 0) {
            if (command[3] == ' ') {  // Vérifie qu'il y a bien un espace après "mkdir"
                sscanf(command + 4, "%s", param1);
                lire_fichier(&fs, param1);
                memset(param1, 0, sizeof(param1));
            } else {
                printf("Erreur : Syntaxe incorrect.\n");
            }
        }
        else if (strncmp(command, "touch", 5) == 0) {
            if (command[5] == ' ') {  // Vérifie qu'il y a bien un espace après "mkdir"
                sscanf(command + 6, "%s", param1);
                create_file_rep(&fs, param1, 0);
                memset(param1, 0, sizeof(param1));
            } else {
                printf("Erreur : Syntaxe incorrect.\n");
            }
        }
        else if (strncmp(command, "cd", 2) == 0) {
            if (command[2] == ' ') {  // Vérifie qu'il y a bien un espace après "mkdir"
                sscanf(command + 3, "%s", param1);
                cd(&fs,param1);
                memset(param1, 0, sizeof(param1));
            } else {
                printf("Erreur : Syntaxe incorrect.\n");
            }

        }
        else if (strncmp(command, "ln -s", 5) == 0) {
            if (command[5] == ' ') {  // Vérifie qu'il y a bien un espace après "cp"
                if (sscanf(command + 6, "%s %s", param1, param2) == 2) {  // Lire les deux paramètres
                    // Appel de la fonction cp avec les paramètres extraits
                    creer_lien_symbolique(&fs, param2, param1);
                } else {
                    printf("Erreur : Syntaxe incorrecte.\n");
                }
            } else {
                printf("Erreur : Syntaxe incorrecte.\n");
            }
        }
        else if (strncmp(command, "rm", 2) == 0) {
            if (command[2] == ' ') {  // Vérifie qu'il y a bien un espace après "mkdir"
                sscanf(command + 3, "%s", param1);
                supprimer_fichier(&fs,param1);
                memset(param1, 0, sizeof(param1));
            } else {
                printf("Erreur : Syntaxe incorrect.\n");
            }
        }
        else if (strncmp(command, "chmod", 5) == 0) {
            if (command[5] == ' ') {  // Vérifie qu'il y a bien un espace après "cp"
                if (sscanf(command + 6, "%s %s", param1, param2) == 2) {  // Lire les deux paramètres
                    // Appel de la fonction cp avec les paramètres extraits
                    droit(&fs, param2, param1);
                } else {
                    printf("Erreur : Syntaxe incorrecte.\n");
                }
            } else {
                printf("Erreur : Syntaxe incorrecte.\n");
            }
        }
        else if (strncmp(command, "pwd",3) == 0) {

            if(command[3]=='\0'){
                pwd(&fs);
            }else{
                if(command[3]==' '){
                    sscanf(command + 4, "%s", param1);
                    if(param1[0]=='\0'){
                        pwd(&fs);
                    }
                }else{
                    printf("Erreur : Syntaxe incorrect.\n"); 
                }
                
            }
            memset(param1, 0, sizeof(param1));
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
                sscanf(command + 6, "%s", param1);
                ecrire_fichier(&fs, param1);
                memset(param1, 0, sizeof(param1));
            } else {
                printf("Erreur : Syntaxe incorrect.\n");
            }
        }
        else if (strncmp(command, "mv", 2) == 0) {
            if (command[2] == ' ') {  // Vérifie qu'il y a bien un espace après "cp"
                if (sscanf(command + 3, "%s %s", param1, param2) == 2) {  // Lire les deux paramètres
                    // Appel de la fonction cp avec les paramètres extraits
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
/*
#include <unistd.h>
#include <fcntl.h>
#include "Bib/Bib.h"
int main()
{
    SystemeFichier fs;

    if (!charger_systeme_fichier(&fs))
    {
        initialiser_systeme_fichier(&fs);
    }

    // Créer un fichier
    create_file_rep(&fs, "/fichier3.txt", 0); // Crée un fichier normal

    ecrire_fichier(&fs, "/fichier3.txt"); // L'utilisateur va saisir le contenu

    // Lire à nouveau le fichier après ajout
    lire_fichier(&fs, "/fichi3.txt");

    // Ouvrir le fichier en mode lecture
    int descripteur = open_file(&fs, "/fichier3.txt", MODE_WRITE);

    // Test de lseek pour déplacer à l'offset 100
    int nouveau_offset = lseek_file(&fs, descripteur, 100);
    if (nouveau_offset != -1)
    {
        printf("Nouveau offset : %d\n", nouveau_offset);
    }

    // Test pour un offset au-delà de la taille actuelle
    nouveau_offset = lseek_file(&fs, descripteur, 500);
    if (nouveau_offset != -1)
    {
        printf("Nouveau offset : %d\n", nouveau_offset);
    }

    // Sauvegarder le système de fichiers
    sauvegarder_systeme_fichier(&fs);

    return 0;
}*/