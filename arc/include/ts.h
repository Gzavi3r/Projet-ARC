#ifndef TS_H
#define TS_H 

#include "erreur.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAILLE_MAX_TS 128

/**
 * @brief Initialise une table des symboles.
 * 
 * Cette macro initialise toutes les cellules de la table des symboles en les mettant à des
 * valeurs par défaut (vides ou non initialisées).
 * 
 * @param t Table des symboles (de type `ts`) à initialiser.
 */
#define INIT_TS(t) {							\
    for(int i = 0; i < TAILLE_MAX_TS; i++) {	\
        t[i].context[0] = '\0'; 		\
        t[i].id[0] = '\0';					\
        t[i].adresse = -1;         \
        t[i].type = -1;          \
        t[i].debut = -1;     \
        t[i].nb_param = 0;     \
        t[i].Est_init = 0;     \
        t[i].Est_utiliser = 0;     \
        t[i].Possede_renvoie = 0;     \
        t[i].Est_check = 0;   \
    }	                        \
  }								\

/**
 * @struct ts_cellule
 * @brief Représente une cellule de la table des symboles.
 * 
 * Cette structure contient toutes les informations nécessaires pour décrire
 * un symbole dans la table des symboles.
 * 
 * - `context`: La zone où la cellule est définie ("GLOBAL" ou nom de la fonction si local).
 * - `id`: L'identificateur du symbole.
 * - `adresse`: L'adresse mémoire associée au symbole.
 * - `type`: Le type du symbole (0 = entier, 1 = fonction).
 * - `debut`: La ligne de début pour une fonction.
 * - `nb_param`: Nombre de paramètres pris par une fonction.
 * - `Est_init`: Indique si une variable est initialisée (1 si oui, 0 sinon).
 * - `Est_utiliser`: Indique si une variable est utilisée quelque part dans le code (1 si oui, 0 sinon).
 * - `Possede_renvoie`: Indique si une fonction possède une valeur de retour (1 si oui, 0 sinon).
 * - `Est_check`: Indique si la variable a été analysée par `semantic_utilisation` (1 si oui, 0 sinon).
 */
struct ts_cellule {
    char context[32];
    char id[32];
    int adresse;
    int type;
    int debut;
    int nb_param;
    int Est_init;
    int Est_utiliser;
    int Possede_renvoie;
    int Est_check;
};

typedef struct ts_cellule ts[TAILLE_MAX_TS];

/**
 * @brief Recherche un identifiant dans la table des symboles.
 * 
 * @param tabsymb Table des symboles.
 * @param context Contexte de recherche (global ou local).
 * @param id Identifiant à rechercher.
 * @return Index de l'identifiant dans la table, ou -1 s'il n'est pas trouvé.
 */
int ts_recherche_id(ts tabsymb, char* context, char* id);

/**
 * @brief Ajoute un nouvel identifiant dans la table des symboles.
 * 
 * @param tabsymb Table des symboles.
 * @param context Contexte de l'identifiant (global ou local).
 * @param id Identifiant à ajouter.
 * @return Index de la nouvelle entrée, ou -1 si une erreur se produit.
 */
int ts_ajouter_id(ts tabsymb, char* context, char* id);

/**
 * @brief Ajoute une fonction dans la table des symboles.
 * 
 * @param tabsymb Table des symboles.
 * @param id Identifiant de la fonction à ajouter.
 * @return Index de la nouvelle entrée, ou -1 si une erreur se produit.
 */
int ts_ajouter_fonction(ts tabsymb, char* id);

/**
 * @brief Recherche une fonction dans la table des symboles.
 * 
 * @param tabsymb Table des symboles.
 * @param id Identifiant de la fonction à rechercher.
 * @return Index de la fonction dans la table, ou -1 si elle n'est pas trouvée.
 */
int ts_recherche_fonction(ts tabsymb, char* id);

/**
 * @brief Affiche le contenu de la table des symboles.
 * 
 * Cette fonction imprime toutes les entrées de la table des symboles, y compris
 * leur contexte, identifiant, adresse, type, etc.
 * 
 * @param tabsymb Table des symboles à afficher.
 */
void PrintTS(ts tabsymb);

#endif
