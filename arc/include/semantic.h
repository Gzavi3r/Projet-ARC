#ifndef SEMANTIC_H
#define SEMANTIC_H 

#include "ast.h"
#include "ts.h"
#include "erreur.h"

/**
 * @brief Coût en instructions RAM des opérations sur la pile.
 * 
 * - `COUT_EMPILER`: Coût pour empiler une valeur sur la pile.
 * - `COUT_DEPILER`: Coût pour dépiler une valeur de la pile.
 * - `COUT_ADR_SOMMET_PILE`: Coût pour obtenir l'adresse du sommet de la pile.
 */
#define COUT_EMPILER 2
#define COUT_DEPILER 2
#define COUT_ADR_SOMMET_PILE 1

/**
 * @brief Vérifie les règles sémantiques dans un arbre syntaxique abstrait (AST).
 * 
 * Cette fonction analyse l'arbre pour s'assurer que toutes les règles de sémantique
 * (comme les types, portée des variables, etc.) sont respectées.
 * 
 * @param p Pointeur vers la racine de l'arbre syntaxique abstrait.
 */
void semantic(ast * p);

/**
 * @brief Vérifie l'utilisation correcte des éléments dans un arbre syntaxique abstrait (AST).
 * 
 * Cette fonction parcourt l'arbre pour valider l'utilisation des variables, fonctions, 
 * et autres entités définies dans le programme.
 * 
 * @param p Pointeur vers la racine de l'arbre syntaxique abstrait.
 */
void semantic_utilisation(ast *p);

#endif
