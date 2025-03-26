#ifndef LISTE_PAGE_H
#define LISTE_PAGE_H

#include <stdint.h>
#include "../Bib/Bib.h"


// Déclarations des fonctions
ListePage initialiserListePage();
ListePage creerPage(uint32_t bloc_debut, uint32_t numero_page);
ListePage insertOrdonner(ListePage liste, AdressePage page);
ListePage supprimerPage(ListePage liste, uint32_t bloc_debut, uint32_t numero_page);
ListePage Suivant(ListePage element);
ListePage Precedent(ListePage element);
void afficherListePage(ListePage liste);
void libererListePage(ListePage liste);

#endif // LISTE_PAGE_H
