#ifndef CODEGEN_H
#define CODEGEN_H 

#include "ts.h"
#include "ast.h"
#include "semantic.h"

/**
 * @brief Registres spécifiques utilisés pour la génération de code.
 * Ces registres sont utilisés pour diverses opérations pendant l'exécution
 * du code généré par l'AST.
 */
#define REGISTRE_TMP 1 /**< Registre temporaire pour des calculs intermédiaires. */
#define REGISTRE_TMP_VALEUR 2 /**< Registre pour stocker temporairement une valeur. */
#define REGISTRE_TMP_ADRESSE 3 /**< Registre pour stocker temporairement une adresse. */
#define REGISTRE_RETOUR 4 /**< Registre pour sauvegarder la ligne avant un appel de fonction. */
#define REGISTRE_PILE 5 /**< Registre pointant vers le sommet de la pile. */
#define REGISTRE_PILE_APPELLE 6 /**< Registre pour sauvegarder la pile avant un appel de fonction. */
#define REGISTRE_RENVOIE 7 /**< Registre contenant la valeur de retour d'une fonction. */

/**
 * @brief Flux de sortie utilisé pour écrire le code généré.
 */
extern FILE * out;

/**
 * @brief Table des symboles utilisée pour suivre les variables et fonctions.
 */
extern ts TABSYMB;

/**
 * @brief Contexte courant utilisé dans la génération de code (par exemple, fonction actuelle).
 */
extern char CTXT[32];

/**
 * @brief Macro pour empiler une valeur sur la pile.
 * Cette macro génère les instructions pour sauvegarder la valeur d'un registre
 * sur le sommet de la pile, puis incrémente le pointeur de pile.
 */
#define EMPILER(){   \
    fprintf(out,"STORE @%d     ;on stocke au sommet de la pile\n",REGISTRE_PILE); \
    fprintf(out,"INC %d    ;on incremente le sommet de la pile\n",REGISTRE_PILE); \
} \

/**
 * @brief Macro pour dépiler une valeur de la pile.
 * Cette macro génère les instructions pour décrémenter le pointeur de pile
 * puis charger la valeur au sommet de la pile dans un registre.
 */
#define DEPILER(){   \
    fprintf(out,"DEC %d    ;On decremente le sommet de la pile\n",REGISTRE_PILE);    \
    fprintf(out,"LOAD @%d     ;On charge le sommet de la pile\n",REGISTRE_PILE);   \
} \

/**
 * @brief Macro pour obtenir l'adresse du sommet de la pile.
 * Génère l'adresse correspondant au sommet actuel de la pile.
 */
#define ADR_SOMMET_PILE(){   \
    fprintf(out,"@%d    ;On donne le sommet de la pile\n",REGISTRE_PILE); \
} \

/**
 * @brief Génère le code correspondant à un arbre syntaxique abstrait (AST).
 * 
 * @param p Pointeur vers la racine de l'AST à traduire en code.
 */
void codegen(ast * p);

/**
 * @brief Initialise les ressources nécessaires pour la génération de code.
 * Cette fonction configure l'environnement initial pour écrire le code généré.
 */
void codegen_INIT();

/**
 * @brief Imprime l'intégralité de l'AST pour débogage.
 * 
 * @param p Pointeur vers la racine de l'AST à imprimer.
 */
void PrintALL(ast * p);

#endif
