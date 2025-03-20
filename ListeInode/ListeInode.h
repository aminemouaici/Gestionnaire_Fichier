#ifndef LISTE_INODE_H
#define LISTE_INODE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Bib/Bib.h"
// Structure d'un Inode


// Déclarations des fonctions
ListeInode initialiserListe();
ListeInode creerInode(const char *nom, uint8_t type, uint8_t permissions);
ListeInode insererDebut(ListeInode liste, Inode inode);
ListeInode insererFin(ListeInode liste, Inode inode);
ListeInode rechercherInode(ListeInode liste, const char *nom);
ListeInode supprimerInode(ListeInode liste, const char *nom);
void afficherListe(ListeInode liste);
void libererListe(ListeInode liste);

#endif // LISTE_INODE_H
