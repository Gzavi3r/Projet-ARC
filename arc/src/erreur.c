/**
 * @file erreur.c
 * @brief Gestion des erreurs et des avertissements dans le programme.
 * 
 * Ce fichier contient des fonctions permettant de gérer les erreurs et avertissements lors de l'exécution du programme. Les erreurs sont affichées avec des informations de position (ligne et colonne) afin de faciliter le débogage. Les types d'erreurs traitées incluent les erreurs syntaxiques, de nommage, de division par zéro, de redéfinition, de mémoire, de type, d'arité, et lexicales.
 * 
 * Les fonctions suivantes sont disponibles :
 * - `warning` : Affiche un avertissement avec un message formaté.
 * - `erreur` : Affiche un message d'erreur et termine l'exécution du programme.
 * - `update_position` : Met à jour les informations de position (ligne et colonne) pour l'analyseur lexical.
 */


#include "erreur.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Contient toute les fonctions liées à la geston des erreurs
J'ai apris la gestion des positions avec yylloc sur:
    https://github.com/sunxfancy/flex-bison-examples/tree/master/error-handling
    https://stackoverflow.com/questions/656703/how-does-flex-support-bison-location-exactly

*/

int first_line ;
int last_line ;
int first_column ;
int last_column ;


/**
 * @brief Affiche un message d'avertissement formaté.
 * 
 * Cette fonction permet d'afficher un avertissement sur la sortie d'erreur standard (stderr). Le message d'avertissement est formaté avec les informations de ligne et de colonne, ainsi qu'un texte personnalisé passé en argument. Elle utilise la gestion des arguments variables pour permettre un message flexible et dynamique.
 * 
 * @param msg Message d'avertissement à afficher, qui peut contenir des spécificateurs de format.
 */
void warning(const char * msg, ...){
    char message[256]; // Buffer pour le message formaté
    // Récupération des arguments supplémentaires
    va_list args;
    va_start(args, msg);
    vsnprintf(message, sizeof(message), msg, args); // Construction du message
    va_end(args);

    fprintf(stderr, "\x1b[1m%s:%d:%d:\x1b[33m WARNING:\x1b[0m %s\n", 
            srcname, 
            first_line, 
            first_column, 
            message);
}

/*
La méthode pour passer des message formatés m'a été donnée par chatgpt

Fonction pour renvoyer les erreurs fatal 
*/
/**
 * @brief Affiche un message d'erreur et arrête l'exécution du programme.
 * 
 * Cette fonction permet d'afficher un message d'erreur sur la sortie d'erreur standard (stderr), en fonction du type d'erreur spécifié. Le message d'erreur est formaté avec les informations de ligne, colonne, ainsi que le type d'erreur et un message personnalisé passé en argument. Après avoir affiché l'erreur, la fonction termine l'exécution du programme avec un code d'erreur correspondant.
 * 
 * @param type_erreur Type d'erreur à afficher (par exemple, erreur syntaxique, erreur de nommage, etc.).
 * @param format Message d'erreur à afficher, qui peut contenir des spécificateurs de format.
 */
void erreur(int type_erreur, const char *format, ...) {
    char message[256]; // Buffer pour le message formaté

    // Récupération des arguments supplémentaires
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args); // Construction du message
    va_end(args);

    char msg_erreur[256];
    switch (type_erreur) {
        case SYNTAX_ERROR:
            strcpy(msg_erreur, "Erreur syntaxique");
            break;
        
        case NAME_ERROR:
            strcpy(msg_erreur, "Erreur nommage ");
            break;
        
        case ZeroDivision_Error:
            strcpy(msg_erreur, "Erreur division par zéro");
            break;
        
        case REDEFINITION_ERROR:
            strcpy(msg_erreur, "Erreur redéfinition");
            break;
        
        case MEMORY_ERROR:
            strcpy(msg_erreur, "Erreur mémoire");
            break;
        case TYPE_ERROR:
            strcpy(msg_erreur, "Erreur Type");
            break;
        case ARITY_ERROR:
            strcpy(msg_erreur, "Erreur d'arité");
            break;
        case LEXICAL_ERROR:
            strcpy(msg_erreur, "Erreur lexical");
            break;
        default:
            fprintf(stderr, "Erreur de case dans erreur\n");
            exit(1);
            break;
    }
    //TODO La division par 0

    // Affichage du message avec le type d'erreur en rouge
    //TODO (resolut)bug sur la position car la semantic se fait apres le yyparse et que donc la position est a la fin 
    fprintf(stderr, "\x1b[1m%s:%d:%d:\x1b[31m %s:\x1b[0m %s\n", 
            srcname, 
            first_line, 
            first_column, 
            msg_erreur, 
            message);

    exit(type_erreur);
}

/**
 * @brief Met à jour les informations de position (ligne et colonne) de l'analyseur.
 * 
 * Cette fonction met à jour les variables globales de position en fonction des informations fournies par l'analyseur lexical (par exemple, Flex/Bison). Ces informations sont utilisées pour indiquer la position des erreurs ou des avertissements dans le code source en cours d'analyse.
 * 
 * @param position Structure contenant les informations de position (première et dernière ligne, première et dernière colonne) à mettre à jour.
 */
void update_position(YYLTYPE position){
    first_line = position.first_line;
    last_line = position.last_line;
    first_column = position.first_column;
    last_column = position.last_column;
}


