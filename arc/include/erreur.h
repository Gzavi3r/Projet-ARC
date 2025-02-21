#ifndef ERREUR_H
#define ERREUR_H 

#include "parser.h"
#include "ast.h"

/**
 * @brief Nom de la source en cours de traitement.
 * Utilisé pour identifier le fichier source lors de la génération des erreurs.
 */
extern char srcname[64];

/**
 * @brief Affiche un avertissement non bloquant avec un message formaté.
 * 
 * @param msg Format du message d'avertissement (similaire à printf).
 * @param ... Arguments optionnels pour le format du message.
 */
void warning(const char * msg, ...);

/**
 * @brief Génère un message d'erreur critique et termine l'exécution si nécessaire.
 * 
 * @param type_erreur Type d'erreur, basé sur les valeurs définies dans l'énumération.
 * @param format Format du message d'erreur (similaire à printf).
 * @param ... Arguments optionnels pour le format du message.
 */
void erreur(int type_erreur, const char *format, ...);

/**
 * @brief Met à jour la position actuelle (ligne et colonne) dans le code source.
 * 
 * @param position Structure contenant les nouvelles coordonnées dans le fichier source.
 */
void update_position(YYLTYPE position);

/**
 * @brief Types d'erreurs possibles, inspirés des erreurs de Python.
 * 
 * - `SYNTAX_ERROR`: Erreur de syntaxe.
 * - `LEXICAL_ERROR`: Erreur lexicale.
 * - `NAME_ERROR`: Utilisation d'un nom non défini.
 * - `ZeroDivision_Error`: Division par zéro.
 * - `REDEFINITION_ERROR`: Redéfinition d'une variable ou fonction existante.
 * - `MEMORY_ERROR`: Erreur d'allocation mémoire.
 * - `TYPE_ERROR`: Incompatibilité de type.
 * - `ARITY_ERROR`: Nombre d'arguments incorrect lors d'un appel de fonction.
 */
enum {SYNTAX_ERROR, LEXICAL_ERROR, NAME_ERROR, ZeroDivision_Error, 
      REDEFINITION_ERROR, MEMORY_ERROR, TYPE_ERROR, ARITY_ERROR};

#endif
