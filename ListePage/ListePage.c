#include <stdio.h>
#include <stdlib.h>
#include "ListePage.h"

// ✅ Initialisation d'une liste vide
ListePage initialiserListePage() {
    return NULL;
}

// ✅ Création d'un nouvel élément
ListePage creerPage(uint32_t bloc_debut, uint32_t numero_page) {
    ListePage nouveau = (ListePage)malloc(sizeof(struct cellulePage));
    if (!nouveau) {
        perror("Erreur d'allocation mémoire");
        exit(1);
    }
    nouveau->page.bloc_debut = bloc_debut;
    nouveau->page.numero_page = numero_page;
    nouveau->suivant = NULL;
    nouveau->precedent = NULL;
    return nouveau;
}

// ✅ Insertion ordonnée (triée par `bloc_debut`, puis `numero_page`)
ListePage insertOrdonner(ListePage liste, AdressePage page) {
    ListePage nouveau = creerPage(page.bloc_debut, page.numero_page);

    // Cas où la liste est vide ou doit être insérée en tête
    if (liste == NULL || page.bloc_debut < liste->page.bloc_debut ||
       (page.bloc_debut == liste->page.bloc_debut && page.numero_page < liste->page.numero_page)) {
        nouveau->suivant = liste;
        if (liste != NULL) {
            liste->precedent = nouveau;
        }
        return nouveau;
    }

    // Recherche de la position d'insertion
    ListePage temp = liste;
    while (temp->suivant != NULL && 
          (temp->suivant->page.bloc_debut < page.bloc_debut ||
          (temp->suivant->page.bloc_debut == page.bloc_debut && temp->suivant->page.numero_page < page.numero_page))) {
        temp = temp->suivant;
    }

    // Insertion après `temp`
    nouveau->suivant = temp->suivant;
    if (temp->suivant != NULL) {
        temp->suivant->precedent = nouveau;
    }
    temp->suivant = nouveau;
    nouveau->precedent = temp;

    return liste;
}

// ✅ Suppression d'une page spécifique
ListePage supprimerPage(ListePage liste, uint32_t bloc_debut, uint32_t numero_page) {
    ListePage temp = liste;

    // Recherche de l'élément à supprimer
    while (temp != NULL) {
        if (temp->page.bloc_debut == bloc_debut && temp->page.numero_page == numero_page) {
            // Mise à jour des pointeurs
            if (temp->precedent != NULL) {
                temp->precedent->suivant = temp->suivant;
            }
            if (temp->suivant != NULL) {
                temp->suivant->precedent = temp->precedent;
            }
            // Si c'était le premier élément, on met à jour la tête
            if (liste == temp) {
                liste = temp->suivant;
            }
            free(temp);
            return liste;
        }
        temp = temp->suivant;
    }
    return liste;
}

// ✅ Obtenir l'élément suivant
ListePage Suivant(ListePage element) {
    return (element) ? element->suivant : NULL;
}

// ✅ Obtenir l'élément précédent
ListePage Precedent(ListePage element) {
    return (element) ? element->precedent : NULL;
}

// ✅ Affichage de la liste
void afficherListePage(ListePage liste) {
    ListePage temp = liste;
    while (temp != NULL) {
        printf("Bloc: %u | Page: %u\n", temp->page.bloc_debut, temp->page.numero_page);
        temp = temp->suivant;
    }
}

// ✅ Libération de la mémoire
void libererListePage(ListePage liste) {
    ListePage temp;
    while (liste != NULL) {
        temp = liste;
        liste = liste->suivant;
        free(temp);
    }
}
