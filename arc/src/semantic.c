/**
 * @file semantic.c
 * @brief Implémente les vérifications sémantiques et la gestion des erreurs pour les nœuds AST.
 * 
 * Ce fichier contient les fonctions nécessaires pour vérifier la validité sémantique du code source 
 * représenté sous forme d'arbre syntaxique abstrait (AST). Ces vérifications incluent la validation 
 * des types, l'initialisation des variables, la gestion des déclarations, et les contrôles spécifiques 
 * liés aux instructions comme les boucles, conditions, et appels de fonctions. Les fonctions gèrent 
 * également la propagation des longueurs de code généré pour chaque nœud.
 * 
 * Les fonctionnalités principales comprennent :
 * - Vérification des déclarations de variables et de fonctions.
 * - Validation de la cohérence des types pour les expressions.
 * - Gestion des retours de fonctions et des instructions conditionnelles.
 * - Génération des erreurs sémantiques et des avertissements.
 */

#include "semantic.h"
#include <string.h>
//pour une gestion plus facile dans cherche renvoie
#include <stdbool.h>

extern ts TABSYMB;
extern char CTXT[32];
//pour n'afficher qu'une seul fois le warning
int warning_lire = 1;
int warning_division = 1;

void semantic_PROGRAMME(ast* p);
void semantic_NB(ast* p);
void semantic_ID(ast* p);
void semantic_OP_UNAIRE(ast *p);
void semantic_OP(ast* p);
void semantic_ECRIRE(ast* p);
void semantic_LIRE(ast *p);
void semantic_BOOL(ast* p);
void semantic_AFFECT(ast* p);
void semantic_LIST_INSTR(ast* p);
void semantic_LIST_ID(ast* p);
void semantic_LIST_EXP(ast* p);
void semantic_DECLA(ast* p);
void semantic_PARAM(ast* p);
void semantic_DECLA_VAR(ast* p);
void semantic_DECLA_FONCTION(ast* p);
void semantic_TQ(ast* p);
void semantic_FAIRE_TQ(ast *p);
void semantic_POUR(ast *p);
void semantic_SI(ast* p);
void semantic_FONCTION(ast* p);
void semantic_APPELLE_FONCTION(ast* p);
void semantic_RENVOIE(ast* p);

void check_type_exp(ast *p);
bool verifier_renvoie(ast * p);


void semantic(ast * p){
    switch(p->type){   

    case AST_PROGRAMME:
        semantic_PROGRAMME(p);
        break;   
    case AST_NB:
        semantic_NB(p);    
        break;   
    case AST_ID:
        semantic_ID(p);   
        break;  
    case AST_ECRIRE:
        semantic_ECRIRE(p);   
        break;  
    case AST_LIRE:
        semantic_LIRE(p);   
        break; 
    case AST_OP_UNAIRE:
        semantic_OP_UNAIRE(p);
        break;  
    case AST_OP:
        semantic_OP(p);
        break;   
    case AST_BOOL:
        semantic_BOOL(p);
        break;   
    case AST_AFFECT:
        semantic_AFFECT(p);
        break; 
    case AST_LIST_INSTR:
        semantic_LIST_INSTR(p);
        break;
    case AST_LIST_ID:  
        semantic_LIST_ID(p);       
        break;
    case AST_LIST_EXP:  
        semantic_LIST_EXP(p);       
        break;
    case AST_DECLA:
        semantic_DECLA(p);
        break;
    case AST_PARAM:
        semantic_PARAM(p);
        break;
    case AST_DECLA_VAR:
        semantic_DECLA_VAR(p);
        break;
    case AST_DECLA_FONCTION:
        semantic_DECLA_FONCTION(p);
        break;
    case AST_TQ:
        semantic_TQ(p);
        break;    
    case AST_FAIRE_TQ:
        semantic_FAIRE_TQ(p);
        break;
    case AST_POUR:
        semantic_POUR(p);
        break;
    case AST_SI:
        semantic_SI(p);
        break;   
    case AST_FONCTION:
        semantic_FONCTION(p);
        break;
    case AST_APPELLE_FONCTION:
        semantic_APPELLE_FONCTION(p);
        break;
    case AST_RENVOIE:
        semantic_RENVOIE(p);
        break;
    default:
        printf("erreur case dans semantic\n");
        exit(1);
        break;
    }
}

/*
fonction qui cherche dans une liste d'instruction si il y'a un renvoie
Cette fonction permet surtout de verifier les SI SINON 
*/
/**
 * @brief Vérifie si une liste d'instructions contient un renvoi.
 *
 * Cette fonction explore récursivement un arbre syntaxique (AST) pour vérifier
 * si une instruction de type "RENVOIE" est présente, notamment dans les blocs conditionnels
 * ou les listes d'instructions.
 *
 * @param p Pointeur vers le nœud AST à analyser.
 * @return `true` si un renvoi est trouvé, `false` sinon.
 */
bool verifier_renvoie(ast* p) {
    if (p==NULL) return false;

    switch (p->type) {
        case AST_RENVOIE:
            return true; // Trouvé un RENVOIE
        case AST_LIST_INSTR: {
            ast* courant = p;
            while (courant) {
                if (verifier_renvoie(courant->suivant[0])) {
                    return true; // Retour trouvé dans une instruction
                }
                courant = courant->suivant[1]; // Prochaine instruction
            }
            return false;
        }
        case AST_SI: {
            // Vérifie que les deux branches (si et sinon) possèdent un RENVOIE
            bool renvoie_si = verifier_renvoie(p->suivant[1]);
            bool renvoie_sinon = verifier_renvoie(p->suivant[2]);
            return renvoie_si && renvoie_sinon; // Les deux branches doivent retourner
        }
        case AST_TQ:
        case AST_FAIRE_TQ:
        case AST_POUR:
            return false; // Les boucles n'assurent pas forcément un retour
        default: {
            // Vérifie les sous-arbres des autres nœuds
            if (verifier_renvoie(p->suivant[0])) return true;
            if (verifier_renvoie(p->suivant[1])) return true; 
            if (verifier_renvoie(p->suivant[2])) return true;
            return false;
        }
    }
}

//Fonction pour voir si les expression on une valeur
/**
 * @brief Vérifie que les expressions ont une valeur valide.
 *
 * Cette fonction s'assure que les fonctions appelées retournent une valeur 
 * et que les variables utilisées sont initialisées. Si des incohérences sont détectées,
 * une erreur est signalée.
 *
 * @param p Pointeur vers le nœud AST représentant l'expression à vérifier.
 */
void check_type_exp(ast *p){
    update_position(p->position);
    if (p->suivant[0] != NULL && p->suivant[0]->type == AST_APPELLE_FONCTION){
        int id_fnc = ts_recherche_fonction(TABSYMB,p->suivant[0]->nom_id);
        if (!TABSYMB[id_fnc].Possede_renvoie){
            erreur(TYPE_ERROR,"La fonction '%s' ne renvoie pas forcement une valeur\n",p->suivant[0]->nom_id);
        }
    }
    if (p->suivant[1] != NULL && p->suivant[1]->type == AST_APPELLE_FONCTION){
        int id_fnc = ts_recherche_fonction(TABSYMB,p->suivant[1]->nom_id);
        if (!TABSYMB[id_fnc].Possede_renvoie){
            erreur(TYPE_ERROR,"La fonction '%s' ne renvoie pas forcement une valeur\n",p->suivant[1]->nom_id);
        }
    }
    if (p->suivant[2] != NULL && p->suivant[2]->type == AST_APPELLE_FONCTION){
        int id_fnc = ts_recherche_fonction(TABSYMB,p->suivant[2]->nom_id);
        if (!TABSYMB[id_fnc].Possede_renvoie){
            erreur(TYPE_ERROR,"La fonction '%s' ne renvoie pas forcement une valeur\n",p->suivant[2]->nom_id);
        }
    }

    if (p->suivant[0] != NULL && p->suivant[0]->type == AST_ID){
        int id = ts_recherche_id(TABSYMB,CTXT,p->suivant[0]->nom_id);
        if (!TABSYMB[id].Est_init){
            erreur(TYPE_ERROR,"La variable '%s' n'a pas de valeur\n",p->suivant[0]->nom_id);
        }
    }
    if (p->suivant[1] != NULL && p->suivant[1]->type == AST_ID){
        int id = ts_recherche_id(TABSYMB,CTXT,p->suivant[1]->nom_id);
        if (!TABSYMB[id].Est_init){
            erreur(TYPE_ERROR,"La variable '%s' n'a pas de valeur\n",p->suivant[1]->nom_id);
        }
    }
    if (p->suivant[2] != NULL && p->suivant[2]->type == AST_ID){
        int id = ts_recherche_id(TABSYMB,CTXT,p->suivant[2]->nom_id);
        if (!TABSYMB[id].Est_init){
            erreur(TYPE_ERROR,"La variable '%s' n'a pas de valeur\n",p->suivant[2]->nom_id);
        }
    }


}

/**
 * @brief Analyse sémantiquement un programme complet.
 *
 * Cette fonction traite les déclarations globales et les instructions principales d'un programme,
 * en mettant à jour les longueurs de code généré correspondantes.
 *
 * @param p Pointeur vers le nœud AST représentant le programme.
 */
void semantic_PROGRAMME(ast *p)
{
    if (p->suivant[0] != NULL) //si il y'a des declaration
    {
        semantic(p->suivant[0]);
        p->codelen += p->suivant[0]->codelen;
    }
    semantic(p->suivant[1]);
    p->codelen += p->suivant[1]->codelen;
    p->codelen += 4; //taille du codegen INIT
}


/**
 * @brief Vérifie sémantiquement un entier.
 *
 * Cette fonction prépare un entier pour le code généré, en attribuant une longueur de code fixe.
 *
 * @param p Pointeur vers le nœud AST représentant l'entier.
 */
void semantic_NB(ast *p)
{
    /* 1 LOAD seulement */
    p->codelen = 1;
}

/**
 * @brief Analyse sémantiquement une variable identifiée.
 *
 * Cette fonction met à jour les informations sur l'utilisation de la variable et ajuste
 * la longueur du code généré selon son contexte (global ou local).
 *
 * @param p Pointeur vers le nœud AST représentant la variable.
 */
void semantic_ID(ast *p){
    int i = ts_recherche_id(TABSYMB,CTXT,p->nom_id);
    TABSYMB[i].Est_utiliser = 1;
    if (strcmp(TABSYMB[i].context,"GLOBAL") !=0){
        p->codelen = 4;
    }
    else{
        p->codelen = 1;
    }
}   

/**
 * @brief Analyse sémantiquement une instruction d'écriture.
 *
 * Cette fonction vérifie le type de l'expression à écrire, traite les sous-arbres correspondants,
 * et met à jour la longueur du code généré.
 *
 * @param p Pointeur vers le nœud AST représentant l'instruction d'écriture.
 */
void semantic_ECRIRE(ast *p){
    check_type_exp(p);
    semantic(p->suivant[0]);
    p->codelen += p->suivant[0]->codelen;
    p->codelen += 1;                

}   

/**
 * @brief Analyse sémantiquement une instruction de lecture.
 *
 * Cette fonction avertit l'utilisateur si la lecture pourrait échouer faute de valeurs suffisantes
 * et ajuste la longueur du code généré.
 *
 * @param p Pointeur vers le nœud AST représentant l'instruction de lecture.
 */
void semantic_LIRE(ast *p){
    if (warning_lire){
       warning("Assurez-vous que le programme ait assez de valeurs à lire");
       warning_lire = 0;
    }
    p->codelen += 1;                

}   

/**
 * @brief Analyse sémantiquement une opération unaire.
 *
 * Cette fonction traite les opérateurs unaires (comme la négation logique ou l'inversion arithmétique),
 * ajuste la longueur du code généré, et vérifie les sous-arbres.
 *
 * @param p Pointeur vers le nœud AST représentant l'opération unaire.
 */
void semantic_OP_UNAIRE(ast *p)
{
    semantic(p->suivant[0]);
    p->codelen = p->suivant[0]->codelen;

    switch (p->op)
    {
    case OP_NON:
        p->codelen += 4;
        break;
    case '-':
        p->codelen += 1;
        break;
    default: 
        printf("Erreur: semantic_OP_UNAIRE operateur non reconnu\n");
        exit(1);
        break;
    }
}

/*
Pour les operation binaire non booleen(+,-,/,*,%)
*/
/**
 * @brief Analyse sémantiquement une opération binaire.
 *
 * Cette fonction gère les opérations arithmétiques et vérifie des avertissements tels que
 * la division par zéro. Elle met également à jour la longueur du code généré.
 *
 * @param p Pointeur vers le nœud AST représentant l'opération binaire.
 */
void semantic_OP(ast* p){
    check_type_exp(p);
    semantic(p->suivant[0]);
    if ((p->op == '/' || p->op == '%') && warning_division){
        update_position(p->position);
        warning("Attention à la division par 0, elle n'est pas vérifiée");
        warning_division = 0;
    }
    semantic(p->suivant[1]);
    p->codelen = p->suivant[0]->codelen + p->suivant[1]->codelen +2+ COUT_EMPILER;

}

/**
 * @brief Analyse sémantiquement une expression booléenne.
 *
 * Cette fonction traite les opérateurs logiques et de comparaison, vérifie les expressions
 * contenues, et met à jour la longueur du code généré.
 *
 * @param p Pointeur vers le nœud AST représentant l'expression booléenne.
 */
void semantic_BOOL(ast* p){
    //on verifie que les fonction appellé renvoie bien une valeur  
    check_type_exp(p);
    semantic(p->suivant[0]);
    semantic(p->suivant[1]);
    p->codelen = p->suivant[0]->codelen + p->suivant[1]->codelen;
    
    int op = p->op;
    //TODO Faire un switch
    if (op == OP_OU) p->codelen += 6;
    else if (op == OP_ET) p->codelen += 5;
    else if (op == '<' || op == '>' || op == '=' || op == OP_DIFF ) p->codelen += 6 + COUT_EMPILER;
    else if (op == OP_SUPEG || op == OP_INFEG) p->codelen += 7+COUT_EMPILER;
}

/**
 * @brief Analyse sémantiquement une instruction d'affectation.
 *
 * Cette fonction vérifie le type de l'expression assignée, initialise la variable affectée,
 * et met à jour la longueur du code généré.
 *
 * @param p Pointeur vers le nœud AST représentant l'instruction d'affectation.
 */
void semantic_AFFECT(ast* p){
    
    check_type_exp(p);
    semantic(p->suivant[1]);


    int id = ts_recherche_id(TABSYMB,CTXT,p->nom_id); 
    TABSYMB[id].Est_utiliser = 1;
    
    TABSYMB[id].Est_init = 1;
    if (strcmp(TABSYMB[id].context,"GLOBAL") !=0){
        p->codelen = p->suivant[1]->codelen + 6;
    }
    else{
        p->codelen = p->suivant[1]->codelen + 1;
    }
    
}

/**
 * @brief Analyse sémantiquement une liste d'instructions.
 *
 * Cette fonction traite chaque instruction de la liste, vérifie si un renvoi est présent,
 * et met à jour la longueur du code généré.
 *
 * @param p Pointeur vers le nœud AST représentant la liste d'instructions.
 */
void semantic_LIST_INSTR(ast* p){
    semantic(p->suivant[0]);
    p->codelen = p->suivant[0]->codelen ;
    //TODO Cheker si c'est un RENVOIE (plus rien ne se passe apres.)
    if (p->suivant[1] != NULL){
        semantic(p->suivant[1]);
        p->codelen += p->suivant[1]->codelen;
    }
    
}

/**
 * @brief Analyse sémantiquement une liste d'identifiants.
 *
 * Cette fonction ajoute chaque identifiant à la table des symboles et initialise ses informations
 * d'utilisation et d'initialisation.
 *
 * @param p Pointeur vers le nœud AST représentant la liste d'identifiants.
 */
void semantic_LIST_ID(ast* p){
    int indice = ts_ajouter_id(TABSYMB,CTXT,p->nom_id);
    TABSYMB[indice].Est_init = 0;
    TABSYMB[indice].Est_utiliser = 0;
    if (p->suivant[0] != NULL){
        semantic(p->suivant[0]);
        p->codelen += p->suivant[0]->codelen;
    }
    
}

/**
 * @brief Analyse sémantiquement une liste d'expressions.
 *
 * Cette fonction traite chaque expression de la liste, vérifie leur validité, et met à jour
 * la longueur du code généré.
 *
 * @param p Pointeur vers le nœud AST représentant la liste d'expressions.
 */
void semantic_LIST_EXP(ast* p){
    check_type_exp(p);
    semantic(p->suivant[0]);
    p->codelen = p->suivant[0]->codelen ;
    if (p->suivant[1] != NULL){
        semantic(p->suivant[1]);
        p->codelen += p->suivant[1]->codelen;
    }   
}


/*
A gauche(suivant[0]) les declaration de variable global
A droite(suivant[1]) les declarations de fonction (qui sont obligatoirement en GLOBAL)
*/
/**
 * @brief Analyse sémantiquement une déclaration.
 *
 * Cette fonction gère les déclarations de variables et de fonctions globales,
 * tout en ajustant la longueur du code généré.
 *
 * @param p Pointeur vers le nœud AST représentant la déclaration.
 */
void semantic_DECLA(ast* p){
    if (p->suivant[0] != NULL){
        semantic(p->suivant[0]);
        p->codelen = p->suivant[0]->codelen+3;
    }
    if (p->suivant[1] != NULL){
        semantic(p->suivant[1]);
        p->codelen += p->suivant[1]->codelen;
    }
}

/**
 * @brief Gère les paramètres d'une fonction et met à jour la table des symboles.
 *
 * Cette fonction parcourt la liste des identifiants des paramètres, les ajoute à la table
 * des symboles avec les attributs appropriés et met à jour la longueur du code généré.
 *
 * @param p Pointeur vers le nœud AST représentant les paramètres.
 */
void semantic_PARAM(ast* p){
    ast * list_id = p->suivant[0];
    while (list_id != NULL){
        int indice = ts_ajouter_id(TABSYMB,CTXT,list_id->nom_id);
        TABSYMB[indice].Est_init = 1;
        TABSYMB[indice].Est_utiliser = 0;
        p->codelen += list_id->codelen;
        list_id = list_id->suivant[0];
    }
}

/**
 * @brief Traite une déclaration de variable et génère le code associé.
 *
 * Cette fonction gère les déclarations de variables, les ajoute à la table des symboles,
 * initialise leurs attributs, et calcule la longueur du code en fonction de leur contexte.
 *
 * @param p Pointeur vers le nœud AST représentant la déclaration de variable.
 */
void semantic_DECLA_VAR(ast* p){
    if (p->suivant[2] != NULL){
        semantic(p->suivant[2]);
        p->codelen += p->suivant[2]->codelen;
        ast* list_id = p->suivant[0];
        while (list_id != NULL){
            int indice = ts_ajouter_id(TABSYMB,CTXT,list_id->nom_id);            
            TABSYMB[indice].Est_init = 1;
            TABSYMB[indice].Est_utiliser = 0;
            
            if (strcmp(TABSYMB[indice].context,"GLOBAL") !=0){
                p->codelen += 6;
            }
            else{
                p->codelen += 1;
            }

            
            list_id = list_id->suivant[0];

        }
    }else{
        semantic(p->suivant[0]);
        p->codelen = p->suivant[0]->codelen;
    }
    if (p->suivant[1] != NULL){
        semantic(p->suivant[1]);
        p->codelen += p->suivant[1]->codelen;
    }
}

void semantic_DECLA_FONCTION(ast* p){
    semantic(p->suivant[0]);
    p->codelen = p->suivant[0]->codelen;
   
    if (p->suivant[1] != NULL){
        semantic(p->suivant[1]);
        p->codelen += p->suivant[1]->codelen;
    }
}

void semantic_TQ(ast* p){
    semantic(p->suivant[0]);
    semantic(p->suivant[1]);
    p->codelen = p->suivant[0]->codelen + p->suivant[1]->codelen +2;
}

void semantic_FAIRE_TQ(ast *p)
{
    semantic(p->suivant[0]);
    semantic(p->suivant[1]);

    p->codelen = p->suivant[0]->codelen + p->suivant[1]->codelen + 2;
}

void semantic_POUR(ast *p)
{
    
    semantic(p->suivant[0]);
    semantic(p->suivant[1]);
    semantic(p->suivant[2]);

    int id = ts_recherche_id(TABSYMB, CTXT, p->nom_id);
    TABSYMB[id].Est_utiliser = 1;

    if (strcmp(TABSYMB[id].context,"GLOBAL") != 0){
        p->codelen += p->suivant[0]->codelen + p->suivant[1]->codelen + p->suivant[2]->codelen + 6;
    }
    else{
        p->codelen += p->suivant[0]->codelen + p->suivant[1]->codelen + p->suivant[2]->codelen + 3;
    }
}

void semantic_SI(ast* p){
    semantic(p->suivant[0]);
    p->codelen += p->suivant[0]->codelen;

    p->codelen += 1;


    semantic(p->suivant[1]);
    p->codelen += p->suivant[1]->codelen;


    if (p->suivant[2] != NULL){
        semantic(p->suivant[2]);
        p->codelen += 1;
        p->codelen += p->suivant[2]->codelen;
        
    }
}

void semantic_FONCTION(ast* p){
    update_position(p->position);
    int indice = ts_ajouter_fonction(TABSYMB,p->nom_id);
    TABSYMB[indice].type = 1;
    TABSYMB[indice].Est_init = 1;
    TABSYMB[indice].nb_param = p->nb_param;
    //on change de contexte tant qu'on est dans la fonction
    strcpy(CTXT,p->nom_id);
    if (strcmp(p->nom_id, "MAIN") != 0) {
        p->codelen += 1;
    }
    else{
        p->codelen+=2;
    }


    if (p->suivant[0] != NULL){
        semantic(p->suivant[0]); //on checke les parametre
        p->codelen += p->suivant[0]->codelen;
    }
    if (p->suivant[1] != NULL){
        p->codelen += 3;
        semantic(p->suivant[1]); //on checke les declaration
        p->codelen += p->suivant[1]->codelen;
        
    }
    //on a besoin a l'avance de s'avoir si la fonction a un renvoie
    if (verifier_renvoie(p->suivant[2])){
        TABSYMB[indice].Possede_renvoie = 1;
    }
    semantic(p->suivant[2]); //on checke les instruction
    p->codelen += p->suivant[2]->codelen;
    
    if (!TABSYMB[indice].Possede_renvoie && strcmp(p->nom_id, "MAIN") != 0){
       p->codelen += 6;
    }


    if (strcmp(p->nom_id, "MAIN") == 0) {
        p->codelen += 1;
    }

    strcpy(CTXT,"GLOBAL");
}

void semantic_APPELLE_FONCTION(ast* p){
    /*
    TODO on autorise a appeller le main (risque de boucle infini)
    if (strcmp(p->nom_id,"MAIN")){
        
    }
    */
    update_position(p->position);
    int indice_fnc = ts_recherche_fonction(TABSYMB, p->nom_id);
    TABSYMB[indice_fnc].Est_utiliser = 1;
    p->codelen += 3+3*COUT_EMPILER;
    p->codelen += 2;
    //on compte le nombre d'argument donné
    int nb_params = 0;
    ast* argument = p->suivant[0];
    while (argument != NULL){
        nb_params++;           
        p->codelen +=  COUT_EMPILER;      
        argument = argument->suivant[1];
    }
    if (p->suivant[0] != NULL){
        semantic(p->suivant[0]);
        p->codelen += p->suivant[0]->codelen;
    }
    // On vérifie que le nombre d'argument est le bon
    if (nb_params != TABSYMB[indice_fnc].nb_param) {
        update_position(p->suivant[0]->position);
        erreur(ARITY_ERROR,"Nombre de paramètre incorecte: la fonction '%s' attend %d paramètres mais %d on été passés\n",p->nom_id,TABSYMB[indice_fnc].nb_param,nb_params);
    }
    if (TABSYMB[indice_fnc].Possede_renvoie){
        p->codelen += 2+COUT_DEPILER;

    
    }
    p->codelen += 5+2*COUT_DEPILER;
    
    
}


void semantic_RENVOIE(ast* p){
    //TODO choisir l'erreur a afficher
    /*
    if (strcmp(CTXT, "MAIN")==0){
        printf("Vous avez un RENVOIE en dehors d'une fonction");
        exit(1);
    }
    */
    if (strcmp(CTXT, "MAIN") == 0)
    {    
        erreur(SYNTAX_ERROR,"Il faut mettre un ECRIRE à la place du RENVOIE dans le MAIN");

    }

    semantic(p->suivant[0]);
    p->codelen += p->suivant[0]->codelen;
    
    
    p->codelen += 8+COUT_EMPILER;
    
}


/*
Fonction pour verifier si une variable est inutilisée, 
doit ce faire apres la semantic pour etre sur d'avoir bien lu toutes les variables
Pourrais servir d'autre verification voire a faire de l'optimisation 
*/
void semantic_utilisation(ast *p){
    if (p == NULL) return;
    int id;
    switch (p->type)
    {
    case AST_ID:
        id = ts_recherche_id(TABSYMB, CTXT, p->nom_id);
        update_position(p->position);
        if (!TABSYMB[id].Est_utiliser && !TABSYMB[id].Est_check){
            
            warning("La variable '%s' dans '%s' n'est pas utilisée", p->nom_id,CTXT);
        }
        TABSYMB[id].Est_check = 1;
        break;
    case AST_LIST_ID:
        id = ts_recherche_id(TABSYMB, CTXT, p->nom_id);
        update_position(p->position);
        if (!TABSYMB[id].Est_utiliser && !TABSYMB[id].Est_check){
            
            warning("La variable '%s' dans '%s' n'est pas utilisée", p->nom_id,CTXT);
        }
        TABSYMB[id].Est_check = 1;
        
        semantic_utilisation(p->suivant[0]);
        break;
    case AST_FONCTION:
        id = ts_recherche_fonction(TABSYMB, p->nom_id);
        
        update_position(p->position);
        if ((!TABSYMB[id].Est_utiliser) && (!TABSYMB[id].Est_check) && strcmp(p->nom_id,"MAIN") != 0){
            warning("la fonction '%s' n'est pas utilisée", p->nom_id);
        }
        TABSYMB[id].Est_check = 1;
        strcpy(CTXT, p->nom_id);

        semantic_utilisation(p->suivant[0]);
        semantic_utilisation(p->suivant[1]) ; 
        semantic_utilisation(p->suivant[2]);
        strcpy(CTXT, "GLOBAL");

        break;
    default: 
        // Vérifie les sous-arbres des autres nœuds
        semantic_utilisation(p->suivant[0]);
        semantic_utilisation(p->suivant[1]) ; 
        semantic_utilisation(p->suivant[2]);
        break;
    }
}

