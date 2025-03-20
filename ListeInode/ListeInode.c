#include "ListeInode.h"

// Initialisation d'une liste vide
ListeInode initialiserListe() {
    return NULL;
}

// Création d'un nouvel inode
ListeInode creerInode(const char *nom, uint8_t type, uint8_t permissions) {
    ListeInode nouveau = (ListeInode)malloc(sizeof(struct celluleInode));
    if (!nouveau) {
        perror("Erreur d'allocation mémoire");
        exit(1);
    }
    strcpy(nouveau->inode.nom, nom);
    nouveau->inode.type = type;
    nouveau->inode.permissions = permissions;
    nouveau->suivant = NULL;
    return nouveau;
}

// Insertion en tête
ListeInode insererDebut(ListeInode liste, Inode inode) {
    ListeInode nouveau = (ListeInode)malloc(sizeof(struct celluleInode));
    if (!nouveau) {
        perror("Erreur d'allocation mémoire");
        exit(1);
    }
    nouveau->inode = inode;
    nouveau->suivant = liste;
    return nouveau;
}

// Insertion en fin
ListeInode insererFin(ListeInode liste, Inode inode) {
    ListeInode nouveau = (ListeInode)malloc(sizeof(struct celluleInode));
    if (!nouveau) {
        perror("Erreur d'allocation mémoire");
        exit(1);
    }
    nouveau->inode = inode;
    nouveau->suivant = NULL;
    if (liste == NULL) {
        return nouveau;
    }
    ListeInode temp = liste;
    while (temp->suivant != NULL) {
        temp = temp->suivant;
    }
    temp->suivant = nouveau;
    return liste;
}

// Recherche d'un inode par nom
ListeInode rechercherInode(ListeInode liste, const char *nom) {
    ListeInode temp = liste;
    while (temp != NULL) {
        if (strcmp(temp->inode.nom, nom) == 0) {
            return temp;
        }
        temp = temp->suivant;
    }
    return NULL;
}

// Suppression d'un inode par nom
ListeInode supprimerInode(ListeInode liste, const char *nom) {
    if (liste == NULL) return NULL;
    if (strcmp(liste->inode.nom, nom) == 0) {
        ListeInode temp = liste->suivant;
        free(liste);
        return temp;
    }
    ListeInode courant = liste;
    while (courant->suivant != NULL && strcmp(courant->suivant->inode.nom, nom) != 0) {
        courant = courant->suivant;
    }
    if (courant->suivant != NULL) {
        ListeInode temp = courant->suivant;
        courant->suivant = temp->suivant;
        free(temp);
    }
    return liste;
}

// Affichage de la liste des inodes
void afficherListe(ListeInode liste) {
    ListeInode temp = liste;
    while (temp != NULL) {
        printf("Nom: %s | Type: %s | Permissions: %o\n", 
               temp->inode.nom, 
               temp->inode.type == 0 ? "Fichier" : "Répertoire",
               temp->inode.permissions);
        temp = temp->suivant;
    }
}

// Libération de la liste (évite les fuites mémoire)
void libererListe(ListeInode liste) {
    ListeInode temp;
    while (liste != NULL) {
        temp = liste;
        liste = liste->suivant;
        free(temp);
    }
}
