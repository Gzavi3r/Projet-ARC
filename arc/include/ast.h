#ifndef AST_H
#define AST_H

#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Définitions des codes de couleurs pour les sorties texte */
#define  TXT_RED    "\x1b[31m"
#define  TXT_GREEN  "\x1b[32m"
#define  TXT_BLUE   "\x1b[34m"
#define  TXT_BOLD   "\x1b[1m"
#define  TXT_NULL   "\x1b[0m"

/* Macro pour initialiser un nœud AST */
#define INIT_NOEUD(p)   if ((p = malloc(sizeof(ast))) == NULL)	\
    ErrorAst("Échec de l'allocation mémoire");			\
  else {							\
    p->type = 0;						\
    p->type_str[0] = '\0';					\
    p->valeur = 0;          \
    p->position.first_line = yylloc.first_line;         \
    p->position.last_line = yylloc.last_line;           \
    p->position.first_column = yylloc.first_column;     \
    p->position.last_column = yylloc.last_column;       \
    p->op = 0;          \
    p->nom_id[0] = '\0';   \
    p->codelen = 0;        \
    p->suivant[0] = NULL;	\
    p->suivant[1] = NULL;	\
    p->suivant[2] = NULL;	\
    p->nb_param = 0;        \
    p->nb_declaration = 0;  \
  }

/* Types de nœuds de l'AST */
enum {
    AST_NB = 256, AST_OP, AST_OP_UNAIRE, AST_BOOL, AST_ECRIRE, AST_LIRE,
    AST_LIST_INSTR, AST_ID, AST_LIST_ID, AST_LIST_EXP, AST_AFFECT,
    AST_TQ, AST_FAIRE_TQ, AST_POUR, AST_SI, AST_FONCTION, AST_DECLA,
    AST_PARAM, AST_DECLA_VAR, AST_DECLA_FONCTION, AST_APPELLE_FONCTION,
    AST_RENVOIE, AST_PROGRAMME
};

/* Opérateurs booléens */
enum { OP_ET = 512, OP_OU, OP_NON, OP_SUPEG, OP_INFEG, OP_DIFF };

/* Structure représentant un nœud de l'AST */
typedef struct ast {
    int type;                     /* Type du nœud */
    char type_str[32];            /* Représentation textuelle du type */
    int valeur;                   /* Valeur associée au nœud (si applicable) */
    YYLTYPE position;             /* Position dans le fichier source */
    int op;                       /* Opérateur si applicable */
    char nom_id[32];              /* Nom de l'identifiant */
    int codelen;                  /* Longueur du code généré */
    struct ast* suivant[3];       /* Fils du nœud */
    int nb_param;                 /* Nombre de paramètres */
    int nb_declaration;           /* Nombre de déclarations */
} ast;

/* Fonctionnalités associées à l'AST */

/**
 * @brief Crée un nœud pour un programme contenant deux parties principales.
 */
ast * CreerNoeudPROGRAMME(ast* p1, ast* p2);

/**
 * @brief Crée une feuille contenant un nombre.
 */
ast * CreerFeuilleNB(int nb);

/**
 * @brief Crée une feuille contenant un identifiant.
 */
ast * CreerFeuilleID(char* id);

/**
 * @brief Crée un nœud représentant une instruction d'écriture.
 */
ast * CreerNoeudECRIRE(ast * exp);

/**
 * @brief Crée un nœud représentant une instruction de lecture.
 */
ast * CreerNoeudLIRE();

/**
 * @brief Crée un nœud pour un opérateur binaire.
 */
ast * CreerNoeudOP(int operateur, ast* p1, ast* p2);

/**
 * @brief Crée un nœud pour un opérateur unaire.
 */
ast * CreerNoeudOP_UNAIRE(int operateur, ast* p1);

/**
 * @brief Crée un nœud pour une expression booléenne.
 */
ast * CreerNoeudBOOL(int operateur, ast* p1, ast* p2);

/**
 * @brief Crée un nœud pour une liste d'instructions.
 */
ast * CreerNoeudLIST_INSTR(ast* p1, ast* p2);

/**
 * @brief Crée un nœud pour une liste d'identifiants.
 */
ast * CreerNoeudLIST_ID(char* id, ast* p1);

/**
 * @brief Crée un nœud pour une liste d'expressions.
 */
ast * CreerNoeudLIST_EXP(ast* exp, ast* list_exp);

/**
 * @brief Crée un nœud pour une déclaration globale.
 */
ast * CreerNoeudDECLA(ast* decla_var, ast* decla_fct);

/**
 * @brief Crée un nœud pour des paramètres de fonction.
 */
ast * CreerNoeudPARAM(ast* list_id);

/**
 * @brief Crée un nœud pour une déclaration de variable.
 */
ast * CreerNoeudDECLA_VAR(ast* list_id, ast * exp, ast* suivant);

/**
 * @brief Crée un nœud pour une déclaration de fonction.
 */
ast * CreerNoeudDECLA_FONCTION(ast* fonction, ast* suivant);

/**
 * @brief Crée un nœud pour une instruction d'affectation.
 */
ast * CreerNoeudAFFECT(char* id, ast* p1);

/**
 * @brief Crée un nœud pour une boucle tant que (TQ).
 */
ast * CreerNoeudTQ(ast* p1, ast* p2);

/**
 * @brief Crée un nœud pour une boucle faire tant que (FAIRE TQ).
 */
ast * CreerNoeudFAIRE_TQ(ast* p1, ast* p2);

/**
 * @brief Crée un nœud pour une boucle pour (POUR).
 */
ast * CreerNoeudPOUR(char * id, ast * debut, ast* fin, ast * list_inst);

/**
 * @brief Crée un nœud pour une structure conditionnelle (SI).
 */
ast * CreerNoeudSI(ast* exp, ast* list_instr1, ast* list_instr2);

/**
 * @brief Crée un nœud pour une fonction.
 */
ast * CreerNoeudFONCTION(char * id, ast* param, ast* decla, ast * L_instr);

/**
 * @brief Crée un nœud pour l'appel d'une fonction.
 */
ast * CreerNoeudAPPELLE_FONCTION(char * id, ast* args);

/**
 * @brief Crée un nœud pour une instruction de renvoi.
 */
ast * CreerNoeudRENVOIE(ast* p1);

/**
 * @brief Libère la mémoire allouée pour un nœud de l'AST et ses descendants.
 */
void FreeAst(ast * p);

/**
 * @brief Affiche une représentation de l'AST.
 */
void PrintAst(ast * p);

/**
 * @brief Affiche une erreur associée à l'AST et quitte le programme.
 */
void ErrorAst(const char * errmsg);

#endif
