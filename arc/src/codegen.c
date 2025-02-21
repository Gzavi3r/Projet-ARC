/**
 * @file codegen.c
 * @brief Implémentation des fonctions de génération de code pour les différents nœuds d'un arbre syntaxique abstrait (AST).
 * 
 * Ce fichier contient une fonction principale `codegen` qui, en fonction du type du nœud de l'AST, appelle la fonction appropriée pour générer du code.
 * 
 * Les fonctions suivantes traitent les différents types d'expressions et instructions : programmes, déclarations de variables, affectations, boucles, conditionnelles, etc.
 */

#include "codegen.h"

int PILE = 1;
int NB_LIGNE = 0;
//rajouter out
#define NB_REGISTRE  9

// Prototypes des fonctions d'affichage statiques.
static void codegen_PROGRAMME(ast * p);
static void codegen_NB(ast * p);
static void codegen_OP_UNAIRE(ast * p);
static void codegen_OP(ast * p);
static void codegen_ECRIRE(ast * p);
static void codegen_LIRE(ast *p);
static void codegen_BOOL(ast * p);

static void codegen_NON(ast * p);
static void codegen_OU(ast * p);
static void codegen_ET(ast * p);
static void codegen_INF(ast * p);
static void codegen_SUP(ast * p);
static void codegen_EG(ast * p);
static void codegen_DIFF(ast * p);
static void codegen_INFEG(ast * p);
static void codegen_SUPEG(ast * p);

static void codegen_ID(ast * p);
static void codegen_DECLA(ast * p);
static void codegen_PARAM(ast * p);
static void codegen_DECLA_VAR(ast * p);
static void codegen_DECLA_FONCTION(ast * p);
static void codegen_LIST_ID(ast * p);
static void codegen_AFF(ast * p);
static void codegen_LIST_INSTR(ast * p);
static void codegen_TQ(ast * p);
static void codegen_FAIRE_TQ(ast * p);
static void codegen_POUR(ast * p);
static void codegen_SI(ast * p);
static void codegen_FONCTION(ast * p);
static void codegen_APPELLE_FONCTION(ast * p);
static void codegen_RENVOIE(ast * p);

/**
 * @brief Fonction principale pour générer du code en fonction du type du nœud AST.
 * 
 * Cette fonction vérifie le type du nœud et appelle la fonction spécifique correspondant à ce type.
 * 
 * @param p Pointeur vers l'AST.
 */
void codegen(ast * p){
    switch(p->type){
    case AST_PROGRAMME:
        codegen_PROGRAMME(p);
        break;     
    case AST_NB:
        codegen_NB(p);
        break;
    case AST_OP_UNAIRE:
        codegen_OP_UNAIRE(p);
        break;      
    case AST_OP:
        codegen_OP(p);
        break;    
    case AST_ECRIRE:
        codegen_ECRIRE(p);
        break;  
    case AST_LIRE:
        codegen_LIRE(p);
        break; 
    case AST_BOOL:
        codegen_BOOL(p);
        break;   
    case AST_DECLA:
        codegen_DECLA(p);
        break;   
    case AST_PARAM:
        codegen_PARAM(p);
        break;   
    case AST_DECLA_VAR:
        codegen_DECLA_VAR(p);
        break;  
    case AST_DECLA_FONCTION:
        codegen_DECLA_FONCTION(p);
        break;         
    case AST_ID:
        codegen_ID(p);
        break;    
    case AST_AFFECT:
        codegen_AFF(p);
        break;    
    case AST_LIST_INSTR:
        codegen_LIST_INSTR(p);
        break;
    case AST_LIST_ID:
        codegen_LIST_ID(p);
        break;
    case AST_TQ:
        codegen_TQ(p);
        break;   
    case AST_FAIRE_TQ:
        codegen_FAIRE_TQ(p);
        break;
    case AST_POUR:
        codegen_POUR(p);
        break;
    case AST_SI:
        codegen_SI(p);
        break;   
    case AST_FONCTION:
        codegen_FONCTION(p);
        break;
    case AST_APPELLE_FONCTION:
        codegen_APPELLE_FONCTION(p);
        break;
    case AST_RENVOIE:
        codegen_RENVOIE(p);
        break;
    default:
        printf("erreur dans codegen");
        exit(1);
        break;
    }
}

/**
 * @brief Génère l'initialisation de la pile et des registres associés.
 * 
 * Cette fonction génère le code pour initialiser la pile et s'assurer que les registres de la pile et l'adresse temporaire reçoivent les valeurs appropriées.
 * Elle charge la tête de la pile et la stocke dans le registre de la pile, puis charge une valeur dans le registre temporaire d'adresse.
 * 
 * @param tabsymb Table des symboles (non utilisée dans cette fonction, mais passée pour cohérence avec la signature).
 */
void codegen_INIT(ts tabsymb){
    fprintf(out,"LOAD #%d    ;On charge la tete de pile\n",NB_REGISTRE+1); //Charge le debut de la pile
    fprintf(out,"STORE %d   ;On la stocke dans le registre de la pile\n",REGISTRE_PILE);  //on charge dans la pile   //pareil
    fprintf(out,"LOAD #%d    ;On charge la tete de pile\n",0); 
    fprintf(out,"STORE %d   ;On la stocke dans le registre de la pile\n",REGISTRE_TMP_ADRESSE);  //on s'assure que REGISTRE_TMP_ADRESSE ai une valeur

    NB_LIGNE += 4;
}

/**
 * @brief Génère le code pour un programme.
 * @param p Pointeur vers le nœud AST du programme.
 */
void codegen_PROGRAMME(ast * p){
    if (p->suivant[0] != NULL){
        codegen(p->suivant[0]);
    }
    codegen(p->suivant[1]);
}

/**
 * @brief Génère le code pour un nombre (ast NOMBRE).
 * @param p Pointeur vers le nœud AST contenant le nombre.
 */
void codegen_NB(ast * p){
    fprintf(out,"LOAD #%d    ;On charge une valeur\n",p->valeur);
    NB_LIGNE += 1;
}

/**
 * @brief Génère le code pour une opération unaire.
 * @param p Pointeur vers le nœud AST représentant l'opération unaire.
 */
void codegen_OP_UNAIRE(ast *p)
{

    switch (p->op)
    {
    case OP_NON:
        codegen_NON(p);
        break;
    case '-':
        //soit on fait *(-1) soit on fait 0-valeur
        codegen(p->suivant[0]);
        fprintf(out,"MUL #-1     ; On multiplie par -1 pour faire le - unaire\n");
        NB_LIGNE += 1;
        break;
    default:
        printf("Erreur dans codegen OP UNAIRE\n");
        exit(1);
        break;
    }
}

/**
 * @brief Génère le code pour une opération binaire (ex: +, -, *, /, %).
 * @param p Pointeur vers le nœud AST représentant l'opération binaire.
 */
void codegen_OP(ast * p){
    codegen(p->suivant[1]); // Génère le code pour le premier opérande
    EMPILER();
    NB_LIGNE += COUT_EMPILER;
    codegen(p->suivant[0]); // Génère le code pour le deuxième opérande
    fprintf(out, "DEC %d  ;On ajuste le sommet de la pile\n", REGISTRE_PILE); 

    // Génération de l'instruction d'opération
    switch(p->op){
        case '+':
            fprintf(out, "ADD ");
            break;
        case '-':
            fprintf(out, "SUB ");
            break;
        case '*':
            fprintf(out, "MUL ");
            break;
        case '/':
            fprintf(out, "DIV ");
            break;
        case '%':
            fprintf(out, "MOD ");
            break;
        default:
            printf("Erreur dans codegen OP\n");
            exit(1);
    }
    ADR_SOMMET_PILE(); // on applique l'operateur sur le sommet de la pile
    NB_LIGNE +=  2; // Une ligne pour l'opération
}

/**
 * @brief Génère le code pour une instruction d'écriture (ECRIRE).
 * @param p Pointeur vers le nœud AST contenant l'expression à écrire.
 */
void codegen_ECRIRE(ast * p){
    codegen(p->suivant[0]);
    fprintf(out, "WRITE    ;\n");
    NB_LIGNE +=1;

}

/**
 * @brief Génère le code pour une instruction de lecture (LIRE).
 * @param p Pointeur vers le nœud AST pour la lecture.
 */
void codegen_LIRE(ast *p){
    fprintf(out, "READ    ;\n");
    NB_LIGNE +=1;

}

/**
 * @brief Génère le code pour une expression booléenne (opérations logiques, comparaisons).
 * @param p Pointeur vers le nœud AST représentant l'expression booléenne.
 */
void codegen_BOOL(ast * p){
        /* On gère l'associativité */
    switch (p->op)
    {
    case OP_OU:
        codegen_OU(p);
        break;
    case OP_ET:
        codegen_ET(p);
        break;
    case '>':
        codegen_SUP(p);
        break;
    case '<':
        codegen_INF(p);
        break;
    case '=':
        codegen_EG(p);
        break;
    case OP_DIFF:
        codegen_DIFF(p);
        break;
    case OP_INFEG:
        codegen_INFEG(p);
        break;
    case OP_SUPEG:
        codegen_SUPEG(p);
        break;
    default:
        printf("Erreur dans codegen BOOL\n");
        exit(1);
    
    }
}

/**
 * @brief Génère le code pour une opération logique NON.
 * @param p Pointeur vers le nœud AST représentant l'expression.
 */
void codegen_NON(ast *p)
{
    //Si exp vaut != 0  on charge 0(FAUX) sinon exp vaut 0 donc on charge 1(VRAI)
    codegen(p->suivant[0]);
    fprintf(out,"JUMZ %d ;NON: si l'op vaut 0 on saute charger 1 \n",NB_LIGNE +3);
    NB_LIGNE += 1;
    fprintf(out,"LOAD #%d ;NON :on charge 0\n",0);
    NB_LIGNE += 1;
    fprintf(out,"JUMP %d ;NON: on saute a l'instr suivante \n",NB_LIGNE+2);
    NB_LIGNE += 1;
    fprintf(out,"LOAD #%d ;NON : l'op est vrai donc on charge 0\n",1);  
    NB_LIGNE += 1;     
}

/**
 * @brief Génère le code pour une opération logique OU.
 * @param p Pointeur vers le nœud AST représentant l'expression.
 */
void codegen_OU(ast *p)
{
    //Comme sur les compilateur actuelle : on evalue a gauche et si = 1 on arrete
    codegen(p->suivant[0]);
    //on ne peut pas utiliser de JUMG car dans le cas où la valeur est negative elle reste vrai
    fprintf(out,"JUMZ %d ;OU: si l'op gauche vaut 0 on check l'op droit \n",NB_LIGNE +2);
    NB_LIGNE += 1;

    int adresse1 = NB_LIGNE + p->suivant[1]->codelen + 2;
    fprintf(out,"JUMP %d ;OU: si l'op gauche vaut 1 on a fini \n",adresse1);
    NB_LIGNE += 1;

    codegen(p->suivant[1]);
    fprintf(out,"JUMZ %d ;OU: si l'op droit vaut aussi 0 alors FAUX \n",NB_LIGNE +3);
    NB_LIGNE += 1;

    //Dans le cas VRAI on charge 1
    fprintf(out,"LOAD #%d ;OU : EXP VRAI on charge 1\n",1);
    NB_LIGNE += 1;
    fprintf(out,"JUMP %d ;OU: on saute a l'instr suivante \n",NB_LIGNE+2);
    NB_LIGNE += 1;

    //Dans le cas FAUX on charge 0
    fprintf(out,"LOAD #%d ;OU : EXP FAUX on charge 0\n",0);  
    NB_LIGNE += 1;     
}


/**
 * @brief Génère le code pour une opération logique ET.
 * @param p Pointeur vers le nœud AST représentant l'expression.
 */
void codegen_ET(ast *p)
{
    //Comme sur les compilateur actuelle : on evalue a gauche et si = 0 on arrete
    codegen(p->suivant[0]);
    int adresse1 = NB_LIGNE + p->suivant[1]->codelen + 4;
    fprintf(out,"JUMZ %d ;ET: si l'op gauche vaut 0 on saute \n",adresse1);
    NB_LIGNE += 1;
    

    codegen(p->suivant[1]);
    fprintf(out,"JUMZ %d ;ET: si l'op droit vaut 0 on saute \n",NB_LIGNE+3);
    NB_LIGNE +=1;

    fprintf(out,"LOAD #%d ;ET: si les deux sont bon on charge 1 \n",1);
    NB_LIGNE +=1;
    fprintf(out,"JUMP %d ;ET: on saute le sinon \n",NB_LIGNE+2);
    NB_LIGNE +=1;

    /* Cas où les 2 sont faux: on charge 0 */
    fprintf(out,"LOAD #%d ;ET: sinon on charge 0 \n",0);
    NB_LIGNE +=1;
}

/*
Pour verifier si x<y on va comparer en utilisant la soustraction:

    -Si x−y < 0 , alors x<y.
    -Sinon, x >= y.
*/
/**
 * @brief Génère le code pour l'opération "inférieur à" (x < y).
 * 
 * Cette fonction génère le code nécessaire pour comparer deux valeurs (x et y) en vérifiant si x est inférieur à y. Si x-y est négatif, le résultat est vrai.
 * 
 * @param p Pointeur vers le nœud AST représentant l'expression x < y.
 */
void codegen_INF(ast *p)
{
    //meme code que pour l'operation -
    codegen(p->suivant[1]); // Génère le code pour le premier opérande
    EMPILER();
    codegen(p->suivant[0]); // Génère le code pour le deuxième opérande

    fprintf(out, "DEC %d  ;On ajuste le sommet de la pile\n", REGISTRE_PILE); 
    fprintf(out, "SUB ");
    ADR_SOMMET_PILE();
    NB_LIGNE += COUT_EMPILER + 2; // Une ligne pour l'opération
    //La valeur de x-y est dans l'acc apres l'operation

    //check si x-y < 0
    fprintf(out, "JUML %d  ;INF: Si x-y<0 alors saute a VRAI\n", NB_LIGNE+3); 
    NB_LIGNE += 1;
    fprintf(out, "LOAD #%d  ;INF: sinon alors FAUX\n", 0); 
    NB_LIGNE += 1;
    fprintf(out,"JUMP %d ;INF: on saute le VRAI \n",NB_LIGNE+2);
    NB_LIGNE += 1;
    fprintf(out, "LOAD #%d  ;INF: on charge VRAI\n", 1); 
    NB_LIGNE += 1;
}


/*
Pour verifier si x>y on va comparer en utilisant la soustraction(même principe que pour le inferieur):

    -Si x−y > 0 , alors x>y.
    -Sinon, x <= y.
*/
/**
 * @brief Génère le code pour l'opération "supérieur à" (x > y).
 * 
 * Cette fonction génère le code nécessaire pour comparer deux valeurs (x et y) en vérifiant si x est supérieur à y. Si x-y est positif, le résultat est vrai.
 * 
 * @param p Pointeur vers le nœud AST représentant l'expression x > y.
 */
void codegen_SUP(ast *p)
{
    
    //meme code que pour l'operation -
    codegen(p->suivant[1]); // Génère le code pour le premier opérande
    EMPILER();
    codegen(p->suivant[0]); // Génère le code pour le deuxième opérande

    fprintf(out, "DEC %d  ;On ajuste le sommet de la pile\n", REGISTRE_PILE); 
    fprintf(out, "SUB ");
    ADR_SOMMET_PILE();
    NB_LIGNE += COUT_EMPILER + 2; // Une ligne pour l'opération
    //La valeur de x-y est dans l'acc apres l'operation

    //check si x-y > 0
    fprintf(out, "JUMG %d  ;SUP: Si x-y>0 alors saute a VRAI\n", NB_LIGNE+3); 
    NB_LIGNE += 1;
    fprintf(out, "LOAD #%d  ;SUP: sinon alors FAUX\n", 0); 
    NB_LIGNE += 1;
    fprintf(out,"JUMP %d ;SUP: on saute le VRAI \n",NB_LIGNE+2);
    NB_LIGNE += 1;
    fprintf(out, "LOAD #%d  ;SUP: on charge VRAI\n", 1); 
    NB_LIGNE += 1;

}

/*
Pour verifier si x=y on va comparer en utilisant la soustraction(toujours le meme principe):

    -Si x−y = 0 , alors x=y.
    -Sinon, x != y.
*/
/**
 * @brief Génère le code pour l'opération "égal à" (x == y).
 * 
 * Cette fonction génère le code nécessaire pour comparer deux valeurs (x et y) en vérifiant si elles sont égales. Si x-y est égal à 0, le résultat est vrai.
 * 
 * @param p Pointeur vers le nœud AST représentant l'expression x == y.
 */
void codegen_EG(ast *p)
{
    //meme code que pour l'operation -
    codegen(p->suivant[1]); // Génère le code pour le premier opérande
    EMPILER();
    codegen(p->suivant[0]); // Génère le code pour le deuxième opérande

    fprintf(out, "DEC %d  ;EG:On ajuste le sommet de la pile\n", REGISTRE_PILE); 
    fprintf(out, "SUB ");
    ADR_SOMMET_PILE();
    NB_LIGNE += COUT_EMPILER + 2; // Une ligne pour l'opération
    //La valeur de x-y est dans l'acc apres l'operation

    //check si x-y = 0
    fprintf(out, "JUMZ %d  ;EG: Si x-y=0 alors saute a VRAI\n", NB_LIGNE+3); 
    NB_LIGNE += 1;
    fprintf(out, "LOAD #%d  ;EG: sinon alors FAUX\n", 0); 
    NB_LIGNE += 1;
    fprintf(out,"JUMP %d ;EG: on saute le VRAI \n",NB_LIGNE+2);
    NB_LIGNE += 1;
    fprintf(out, "LOAD #%d  ;EG: on charge VRAI\n", 1); 
    NB_LIGNE += 1;
}


/*
Pour verifier si x!=y on va comparer en utilisant la soustraction(et oui):

    -Si x−y != 0 , alors x!=y.
    -Sinon, x = y.
*/
/**
 * @brief Génère le code pour l'opération "différent de" (x != y).
 * 
 * Cette fonction génère le code nécessaire pour comparer deux valeurs (x et y) en vérifiant si elles sont différentes. Si x-y n'est pas égal à 0, le résultat est vrai.
 * 
 * @param p Pointeur vers le nœud AST représentant l'expression x != y.
 */
void codegen_DIFF(ast *p)
{
    //meme code que pour l'operation -
    codegen(p->suivant[1]); // Génère le code pour le premier opérande
    EMPILER();
    codegen(p->suivant[0]); // Génère le code pour le deuxième opérande

    fprintf(out, "DEC %d  ;On ajuste le sommet de la pile\n", REGISTRE_PILE); 
    fprintf(out, "SUB ");
    ADR_SOMMET_PILE();
    NB_LIGNE += COUT_EMPILER + 2; // Une ligne pour l'opération
    //La valeur de x-y est dans l'acc apres l'operation

    //check si x-y != 0
    fprintf(out, "JUMZ %d  ;DIFF: Si x-y=0 alors saute a FAUX\n", NB_LIGNE+3); 
    NB_LIGNE += 1;
    fprintf(out, "LOAD #%d  ;DIFF: sinon alors VRAI\n", 1); 
    NB_LIGNE += 1;
    fprintf(out,"JUMP %d ;DIFF: on saute le FAUX \n",NB_LIGNE+2);
    NB_LIGNE += 1;
    fprintf(out, "LOAD #%d  ;DIFF: on charge FAUX\n", 0); 
    NB_LIGNE += 1;

}

/*
Pour verifier si x>=y on va comparer en utilisant la soustraction(toujours):

    -Si x−y >= 0 , alors x>=y.
    -Sinon, x < y.
    Pour cela on va verifier deux choses :
        si x-y > 0
        si Faux il faut aussi verifier x-y = 0
*/
/**
 * @brief Génère le code pour l'opération "supérieur ou égal à" (x >= y).
 * 
 * Cette fonction génère le code nécessaire pour comparer deux valeurs (x et y) en vérifiant si x est supérieur ou égal à y. Cela implique deux vérifications : si x-y est positif ou égal à 0.
 * 
 * @param p Pointeur vers le nœud AST représentant l'expression x >= y.
 */
void codegen_SUPEG(ast *p)
{
    //meme code que pour l'operation -
    codegen(p->suivant[1]); // Génère le code pour le premier opérande
    EMPILER();
    codegen(p->suivant[0]); // Génère le code pour le deuxième opérande

    fprintf(out, "DEC %d  ;On ajuste le sommet de la pile\n", REGISTRE_PILE); 
    fprintf(out, "SUB ");
    ADR_SOMMET_PILE();
    NB_LIGNE += COUT_EMPILER + 2; // Une ligne pour l'opération
    //La valeur de x-y est dans l'acc apres l'operation

    //check si x-y > 0
    fprintf(out, "JUMG %d  ;SUPEG: Si x-y>0 alors saute a VRAI\n", NB_LIGNE+4); 
    NB_LIGNE += 1;
    //check si x-y = 0
    fprintf(out, "JUMZ %d  ;SUPEG: Si x-y=0 alors saute a VRAI\n", NB_LIGNE+3); 
    NB_LIGNE += 1;
    fprintf(out, "LOAD #%d  ;SUPEG: sinon alors FAUX\n", 0); 
    NB_LIGNE += 1;
    fprintf(out,"JUMP %d ;SUPEG: on saute le VRAI \n",NB_LIGNE+2);
    NB_LIGNE += 1;
    fprintf(out, "LOAD #%d  ;SUPEG: on charge VRAI\n", 1); 
    NB_LIGNE += 1;
}

/* 
    Fonction générant le code pour l'opération INFEG (inférieur ou égal)
    Cette fonction réalise une comparaison entre deux expressions (x - y) et vérifie si x - y est inférieur ou égal à 0. 
    Elle génère le code machine pour cette opération avec les tests appropriés.
*/
/**
 * @brief Génère le code pour l'opération "inférieur ou égal à" (x <= y).
 * 
 * Cette fonction génère le code nécessaire pour comparer deux valeurs (x et y) en vérifiant si x est inférieur ou égal à y. Cela implique deux vérifications : si x-y est négatif ou égal à 0.
 * 
 * @param p Pointeur vers le nœud AST représentant l'expression x <= y.
 */
void codegen_INFEG(ast *p)
{
    //meme code que pour l'operation -
    codegen(p->suivant[1]); // Génère le code pour le premier opérande
    EMPILER();
    codegen(p->suivant[0]); // Génère le code pour le deuxième opérande

    // Ajuste le sommet de la pile
    fprintf(out, "DEC %d  ;On ajuste le sommet de la pile\n", REGISTRE_PILE); 
    fprintf(out, "SUB ");
    ADR_SOMMET_PILE();
    NB_LIGNE += COUT_EMPILER + 2; // Une ligne pour l'opération
    //La valeur de x-y est dans l'acc apres l'operation

    //check si x-y < 0
    fprintf(out, "JUML %d  ;INFEG: Si x-y>0 alors saute a VRAI\n", NB_LIGNE+4); 
    NB_LIGNE += 1;
    //check si x-y = 0
    fprintf(out, "JUMZ %d  ;INFEG: Si x-y=0 alors saute a VRAI\n", NB_LIGNE+3); 
    NB_LIGNE += 1;
    fprintf(out, "LOAD #%d  ;INFEG: sinon alors FAUX\n", 0); 
    NB_LIGNE += 1;
    fprintf(out,"JUMP %d ;INFEG: on saute le VRAI \n",NB_LIGNE+2);
    NB_LIGNE += 1;
    fprintf(out, "LOAD #%d  ;INFEG: on charge VRAI\n", 1); 
    NB_LIGNE += 1;
}

/**
 * @brief Génère le code pour l'identifiant (variable).
 * 
 * Cette fonction génère le code nécessaire pour charger une variable dans un registre, en prenant en compte si la variable est locale ou globale.
 * 
 * @param p Pointeur vers le nœud AST représentant l'identifiant (nom de la variable).
 */
void codegen_ID(ast * p){
    int i;
    i = ts_recherche_id(TABSYMB,CTXT,p->nom_id);
    //Si la varibale est local alors elle n'est pas stockée dans la memoire statique 
    if (strcmp(TABSYMB[i].context,"GLOBAL") !=0){
        fprintf(out,"LOAD %d ;On charge la tete de pile avant appelle de fonction \n",REGISTRE_PILE_APPELLE);
        fprintf(out,"ADD #%d ;On ajoute sa position dans la memoire local\n",TABSYMB[i].adresse);
        fprintf(out,"STORE %d ;On stocke dans le registre tmp\n",REGISTRE_TMP);
        fprintf(out,"LOAD @%d ;On charge la variable LOCAL %s\n",REGISTRE_TMP,p->nom_id);
        NB_LIGNE += 4;
    }
    else{
        fprintf(out,"LOAD %d ;On charge la variable GLOBAL%s\n",i+NB_REGISTRE+1,p->nom_id);
        NB_LIGNE += 1;
    }
    
}

/**
 * @brief Génère le code pour l'affectation d'une valeur à une variable.
 * 
 * Cette fonction génère le code nécessaire pour affecter une valeur à une variable, qu'elle soit locale ou globale.
 * 
 * @param p Pointeur vers le nœud AST représentant l'affectation.
 */
void codegen_AFF(ast * p){
    int i = 0;
    
    codegen(p->suivant[1]);
    
    i = ts_recherche_id(TABSYMB,CTXT,p->nom_id);
    
    //Si l'id est LOCAL(la variable d'une fonction)
    if (strcmp(TABSYMB[i].context,"GLOBAL") !=0){
        //on stocke la valeur a affecter le temps de calculer l'adresse de l'id
        fprintf(out,"STORE %d    ;On enregistre dans le registre tmp valeur\n",REGISTRE_TMP_VALEUR);
        
        //on calcule l'adresse de l'id dans la pile 
        fprintf(out,"LOAD %d ;On charge la tete de pile avant appelle de fonction \n",REGISTRE_PILE_APPELLE);
        fprintf(out,"ADD #%d ;On ajoute la position de l'id dans la memoire local\n",TABSYMB[i].adresse);
        fprintf(out,"STORE %d ;On stocke dans le registre tmp\n",REGISTRE_TMP);

        //on recharge la valeur et on l'affecte a l'id
        fprintf(out,"LOAD %d    ;On charge la valeur que l'on avait stockée \n",REGISTRE_TMP_VALEUR);
        fprintf(out,"STORE @%d    ;On donne a %s la valeur de l'accumulateur\n",REGISTRE_TMP,p->nom_id);
        NB_LIGNE += 6;
    }
    else{
        fprintf(out,"STORE %d    ;On donne a %s la valeur de l'accumulateur\n",i+NB_REGISTRE+1,p->nom_id);
        NB_LIGNE += 1;
    }
}


/*
Ce noeud sert a lier tout les noeuds de déclaration:
    decla var
    decla fonction
*/
/**
 * @brief Génère le code pour la déclaration de variables et de fonctions.
 * 
 * Cette fonction lie les nœuds de déclaration de variables et de fonctions. Elle gère les déclarations globales et locales.
 * 
 * @param p Pointeur vers le nœud AST représentant les déclarations.
 */
void codegen_DECLA(ast * p){

    if (p->suivant[0] != NULL){
        int nb_decla_var = p->nb_declaration;
        fprintf(out,"LOAD #%d     ;On charge le nombre de declaration global\n",nb_decla_var);
        fprintf(out,"ADD %d    ;On bouge la pile d'autant de declaration\n",REGISTRE_PILE);
        fprintf(out,"STORE %d    ;On enregistre\n",REGISTRE_PILE);
        NB_LIGNE += 3;
        codegen(p->suivant[0]);
    }
    if (p->suivant[1] != NULL){
        codegen(p->suivant[1]);
    }
}

/**
 * @brief Génère le code pour les paramètres de fonction.
 * 
 * Cette fonction génère le code nécessaire pour traiter les paramètres d'une fonction. Elle appelle récursivement la génération de code pour chaque paramètre.
 * 
 * @param p Pointeur vers le nœud AST représentant les paramètres d'une fonction.
 */
void codegen_PARAM(ast * p){

    if (p->suivant[0] != NULL){
        codegen(p->suivant[0]);
    }
}


/**
 * @brief Génère le code pour la déclaration de variables avec affectation.
 * 
 * Cette fonction génère le code pour déclarer et, si nécessaire, affecter une valeur à des variables locales ou globales. Les variables locales sont stockées dans la pile, tandis que les globales sont directement affectées.
 * 
 * @param p Pointeur vers le nœud AST représentant la déclaration des variables.
 */
void codegen_DECLA_VAR(ast * p){
    //Si il y'a une expression a affecter, on l'affect a toutes les variable dans la liste
    if (p->suivant[2] != NULL){
        codegen(p->suivant[2]);
        ast* list_id = p->suivant[0];
        while (list_id != NULL){
            int id = ts_recherche_id(TABSYMB, CTXT, list_id->nom_id);
            
            if (strcmp(TABSYMB[id].context,"GLOBAL") !=0){
                //on stocke la valeur a affecter le temps de calculer l'adresse de l'id
                fprintf(out,"STORE %d    ;On enregistre dans le registre tmp valeur\n",REGISTRE_TMP_VALEUR);
                
                //on calcule l'adresse de l'id dans la pile 
                fprintf(out,"LOAD %d ;On charge la tete de pile avant appelle de fonction \n",REGISTRE_PILE_APPELLE);
                fprintf(out,"ADD #%d ;On ajoute la position de l'id dans la memoire local\n",TABSYMB[id].adresse);
                fprintf(out,"STORE %d ;On stocke dans le registre tmp\n",REGISTRE_TMP);

                //on recharge la valeur et on l'affecte a l'id
                fprintf(out,"LOAD %d    ;On charge la valeur que l'on avait stockée \n",REGISTRE_TMP_VALEUR);
                fprintf(out,"STORE @%d    ;On donne a %s la valeur de l'accumulateur\n",REGISTRE_TMP,p->nom_id);
                NB_LIGNE += 6;
            }
            else{
                fprintf(out,"STORE %d    ;On donne a %s la valeur de l'accumulateur\n",id+NB_REGISTRE+1,p->nom_id);
                NB_LIGNE += 1;
            }

            
            list_id = list_id->suivant[0];

        }
    }else{
        codegen(p->suivant[0]);
    }

    if (p->suivant[1] != NULL){
        codegen(p->suivant[1]);
    }
}

/**
 * @brief Génère le code pour la déclaration de fonction.
 * 
 * Cette fonction génère le code pour la déclaration d'une fonction. Elle traite les paramètres et le corps de la fonction en appelant les fonctions appropriées.
 * 
 * @param p Pointeur vers le nœud AST représentant la déclaration de la fonction.
 */
void codegen_DECLA_FONCTION(ast * p){

    codegen(p->suivant[0]);
    if (p->suivant[1] != NULL){
        codegen(p->suivant[1]);
    }
}

/**
 * @brief Génère le code pour une liste d'instructions.
 * 
 * Cette fonction génère le code pour les instructions présentes dans une liste. Elle traite récursivement chaque instruction.
 * 
 * @param p Pointeur vers le nœud AST représentant la liste d'instructions.
 */
void codegen_LIST_INSTR(ast * p){
    codegen(p->suivant[0]);
    if (p->suivant[1] != NULL){
        codegen(p->suivant[1]);
    }
}

/**
 * @brief Génère le code pour une liste d'identifiants.
 * 
 * Cette fonction génère le code pour chaque identifiant dans une liste. Elle appelle récursivement la génération de code pour chaque identifiant.
 * 
 * @param p Pointeur vers le nœud AST représentant la liste d'identifiants.
 */
void codegen_LIST_ID(ast * p){
    if (p->suivant[0] != NULL){
        codegen(p->suivant[0]);
    }
}

/**
 * @brief Génère le code pour une boucle "Tant que" (TQ).
 * 
 * Cette fonction génère le code pour une boucle "Tant que" en générant le code pour la condition, puis en revenant au début de la boucle si la condition est vraie.
 * 
 * @param p Pointeur vers le nœud AST représentant l'expression de la boucle "Tant que".
 */
void codegen_TQ(ast * p){
    int adresse1 = NB_LIGNE;
    codegen(p->suivant[0]); 
    int adresse2 = NB_LIGNE + p->suivant[1]->codelen + 2; 
    fprintf(out, "JUMZ %d    ; TQ : saut si la condition est fausse\n", adresse2);
    NB_LIGNE++; 

    codegen(p->suivant[1]); 

    fprintf(out, "JUMP %d    ; TQ : retour au début de la boucle\n", adresse1);
    NB_LIGNE++; 
}

/**
 * @brief Génère le code pour une boucle "Faire tant que" (FAIRE_TQ).
 * 
 * Cette fonction génère le code pour une boucle "Faire tant que", où le corps de la boucle est exécuté avant la vérification de la condition.
 * 
 * @param p Pointeur vers le nœud AST représentant la boucle "Faire tant que".
 */
void codegen_FAIRE_TQ(ast *p)
{
    int adresse1 = NB_LIGNE;
    codegen(p->suivant[0]);
    codegen(p->suivant[1]);

    fprintf(out, "JUMZ %d    ; FAIRE TQ : On saute hors de la boucle si 0\n", NB_LIGNE+1);
    NB_LIGNE++; 
    fprintf(out, "JUMP %d    ; FAIRE TQ : retour au début de la boucle\n", adresse1);
    NB_LIGNE++; 
}

/**
 * @brief Génère le code pour une boucle "Pour" (POUR).
 * 
 * Cette fonction génère le code pour une boucle "Pour", comprenant l'initialisation, la vérification de la condition, l'exécution du corps, et l'incrémentation de la variable de boucle.
 * 
 * @param p Pointeur vers le nœud AST représentant la boucle "Pour".
 */
void codegen_POUR(ast *p)
{
    int id = ts_recherche_id(TABSYMB, CTXT, p->nom_id);

    /* Initialisation de la variable */
    codegen(p->suivant[0]);

    /* Vérification de la condition */
    int adresse1 = NB_LIGNE;
    codegen(p->suivant[1]);

    int adresse2 = NB_LIGNE + p->suivant[2]->codelen;
    if (strcmp(TABSYMB[id].context,"GLOBAL") != 0){
        adresse2 += 6;
    }
    else{
        adresse2 += 3;
    }
    fprintf(out, "JUMZ %d    ; POUR : On saute hors de la boucle pour \n", adresse2);
    NB_LIGNE += 1;
    codegen(p->suivant[2]);

    //code pour faire id++ 

    //Si la varibale est local alors elle n'est pas stockée dans la memoire statique 
    if (strcmp(TABSYMB[id].context,"GLOBAL") !=0){
        fprintf(out,"LOAD %d ;POUR :On charge la tete de pile avant appelle de fonction \n",REGISTRE_PILE_APPELLE);
        fprintf(out,"ADD #%d ;POUR :On ajoute sa position dans la memoire local\n",TABSYMB[id].adresse);
        fprintf(out,"STORE %d ;POUR :On stocke dans le registre tmp\n",REGISTRE_TMP);
        fprintf(out,"INC @%d ;POUR :On incremente la variable LOCAL %s\n",REGISTRE_TMP,p->nom_id);
        NB_LIGNE += 4;
    }
    else{
        fprintf(out,"INC %d ;POUR :On incremente la variable GLOBAL%s\n",id+NB_REGISTRE+1,p->nom_id);
        NB_LIGNE += 1;
    }

    fprintf(out, "JUMP %d    ; POUR : retour au début de la boucle\n", adresse1);
    NB_LIGNE += 1; 
}

/**
 * @brief Génère le code pour une instruction conditionnelle (if) avec éventuellement un bloc else.
 * 
 * Cette fonction génère le code nécessaire pour implémenter une instruction conditionnelle. Elle évalue d'abord l'expression conditionnelle et, si la condition est fausse (valeur 0), elle saute par-dessus le bloc associé à la condition vraie. Si un bloc `else` est présent, il sera exécuté après le bloc associé à la condition vraie.
 * 
 * @param p Pointeur vers le nœud AST représentant l'instruction conditionnelle `if` (et éventuellement `else`).
 */
void codegen_SI(ast * p){
    codegen(p->suivant[0]);//on genere le code de l'expression

    int adresse1 = NB_LIGNE + p->suivant[1]->codelen + 1;//+1 pour le JUMZ(car il n'est pas encore ecrit) 
    if (p->suivant[2] != NULL){
        adresse1++;//pour le jump
    }
    //si l'expression est vrai(!=0) alors on lit les instruction sinon on saute par dessus 
    fprintf(out, "JUMZ %d    ;On verifie si l'expression est fausse(=0)et on saute par dessus si oui \n", adresse1);
    NB_LIGNE += 1;
    codegen(p->suivant[1]);    
    //si il y'a un sinon
    if (p->suivant[2] != NULL){
        int adresse2 = NB_LIGNE + 1;
        adresse2 += p->suivant[2]->codelen;
        fprintf(out, "JUMP %d    ;On saute par dessus le sinon \n", adresse2);
        NB_LIGNE += 1;
        codegen(p->suivant[2]);
    }
    
}

/**
 * @brief Génère le code pour une fonction, y compris la gestion de la pile locale et du retour.
 * 
 * Cette fonction génère le code pour une fonction, en tenant compte des paramètres, des déclarations et du corps de la fonction. Elle s'assure également de la gestion correcte de la pile, notamment pour les fonctions avec ou sans valeur de retour. Si la fonction est `MAIN`, des instructions spécifiques pour l'initialisation du programme sont générées.
 * 
 * @param p Pointeur vers le nœud AST représentant la fonction.
 */
void codegen_FONCTION(ast * p){  
    int i = ts_recherche_fonction(TABSYMB, p->nom_id);

    strcpy(CTXT, p->nom_id);
    
    if (strcmp(p->nom_id, "MAIN") != 0) {
        fprintf(out, "JUMP %d    ; Sauter par-dessus la fonction %s\n", NB_LIGNE + p->codelen, p->nom_id);
        NB_LIGNE++;
    }
    else{
        //si on est dans le MAIN , on initialise la tete de pile local (le main marche comme une fonction)
        fprintf(out, "LOAD %d    ;MAIN: on charge la tete de pile \n", REGISTRE_PILE);
        fprintf(out, "STORE %d    ;MAIN: on stocke dans la pile local \n", REGISTRE_PILE_APPELLE);
        NB_LIGNE+=2;
    }
    TABSYMB[i].debut = NB_LIGNE; // Sauvegarde le point d'entrée de la fonction apres le jump pour eviter de revenir directement au MAIN et()        
    
    if (p->suivant[0] != NULL) {
        //on a déja bougé la tete de pile pour les argument dans l'appel de fonction
        codegen(p->suivant[0]); // Génération des paramètres
        
    } 
    
    
    if (p->suivant[1] != NULL) {
        //on fait sauter la tete de pile de nb_declaration pour pouvoir stocker chaque declaration 
        fprintf(out,"LOAD #%d     ;On charge le nombre de declaration\n",p->nb_declaration);
        fprintf(out,"ADD %d    ;On bouge la pile d'autant de declaration\n",REGISTRE_PILE);
        fprintf(out,"STORE %d    ;On enregistre\n",REGISTRE_PILE);
        NB_LIGNE += 3;
        codegen(p->suivant[1]); // Génération des declarations
    } 
    
    codegen(p->suivant[2]); // Génération du corps de la fonction

    //Si la fonction possede un renvoie c'est le codegen de revoie qui ecrit ce code
    if (!TABSYMB[i].Possede_renvoie && strcmp(p->nom_id, "MAIN") != 0){
        //a la fin d'une procedure , on ne renvoie rien
        fprintf(out,"LOAD %d     ;Charger la tete de pile local\n",REGISTRE_PILE_APPELLE);
        fprintf(out,"STORE %d     ;on la stocke dans le pile\n",REGISTRE_PILE);
        fprintf(out,"DEC %d     ;\n",REGISTRE_PILE);
        NB_LIGNE += 3;

        //on retrouve les données que l'on avait stocker durant l'appelle de fonction
        fprintf(out,"LOAD @%d   ;On charge l'adresse de retour qui est au sommet de la pile\n", REGISTRE_PILE);
        fprintf(out,"STORE %d     ;\n",REGISTRE_RETOUR);
        NB_LIGNE += 2;


        fprintf(out,"JUMP @%d   ;\n", REGISTRE_RETOUR);
        NB_LIGNE += 1;
    }
    if (strcmp(p->nom_id, "MAIN") == 0) {
        fprintf(out, "STOP\n"); // Arrêt du programme principal
        NB_LIGNE++; 
    }

    strcpy(CTXT, "GLOBAL");

    
}

/**
 * @brief Gère l'appel à une fonction en générant le code nécessaire pour empiler les paramètres et gérer la pile.
 * 
 * Cette fonction génère le code pour appeler une fonction. Elle empile les paramètres de la fonction, enregistre les informations nécessaires dans les registres, puis effectue le saut vers la fonction. Elle gère également les retours de fonctions avec ou sans valeur de retour.
 * 
 * @param p Pointeur vers le nœud AST représentant l'appel à la fonction.
 */
void codegen_APPELLE_FONCTION(ast * p){  
    int id_fnt = ts_recherche_fonction(TABSYMB,p->nom_id);
    int ligne_debut = TABSYMB[id_fnt].debut;

    //on empile l'adresse temporaire au cas ou il 'a un appelle de fonction impriquer
    fprintf(out,"LOAD %d     ;APPELLE_FONCTION:On empile pour pouvoir recharger la tete de pile\n",REGISTRE_TMP_ADRESSE);
    EMPILER();
    NB_LIGNE += 1+COUT_EMPILER;

    //on empile la tete de pile local avant qu'elle ne soit modifier
    fprintf(out,"LOAD %d     ;APPELLE_FONCTION:On empile pour pouvoir recharger l'adresse de pile de debut de fonction\n",REGISTRE_PILE_APPELLE);
    EMPILER();
    NB_LIGNE += 1+COUT_EMPILER;

    
    int valeur_retour = NB_LIGNE + p->codelen+1;//+1 pour le LOAD qui arrive
    if (TABSYMB[id_fnt].Possede_renvoie){
        valeur_retour = valeur_retour-(2+2*COUT_EMPILER+1 + 4+3*COUT_DEPILER);//Il faut que l'on saute apres le jump mais on ne peut pas juste avoir le codelen des argument donc on enleve les instruction avant le LOAD et celle apres le JUMP et +1 pour aller au dela du JUMP
    }
    else{
        valeur_retour = valeur_retour-(2+2*COUT_EMPILER+1 + 2+2*COUT_DEPILER);
    }
    //on empile la valeur de l'adresse de retour 
    fprintf(out,"LOAD #%d     ;APPELLE_FONCTION:On charge la valeur de retour\n",valeur_retour);
    EMPILER();
    NB_LIGNE += 1+COUT_EMPILER;         

    //on enregistre la pile dans REGISTRE_TMP_ADRESSE pour connaitre l'adresse de debut de la pile local mais garder PILE_APPELLE a la bonne adresse le temps de faire le codegen 
    fprintf(out,"LOAD %d     ;APPELLE_FONCTION:On charge la tete de pile\n",REGISTRE_PILE);
    fprintf(out,"STORE %d     ;APPELLE_FONCTION:On le stocke dans le registre d'adresse temporaire\n",REGISTRE_TMP_ADRESSE);
    NB_LIGNE += 2;      
    
    //On empile les parametre en tete de pile local
    ast* argument = p->suivant[0];
    while(argument!= NULL){
        /*
        on produit les expressions et on les empiles pour simuler le stockage des argument dans la pile local
        Ex: fonction(1+1,a*2);
            (a l'adresse de pill relle x)   pile local 0 = codegen(1+1) 
            (a l'adresse de pile relle x+1) pile local 1 = codegen(a*2) 
        Sa evite d'avoir a faire une affectation
        */
        codegen(argument->suivant[0]);
        EMPILER();
        NB_LIGNE += COUT_EMPILER;
        //on continu avec la prochaine list d'expression
        argument = argument->suivant[1];
    }

    //on enregistre la pile dans REGISTRE_TMP_ADRESSE pour connaitre l'adresse de debut de la pile local mais garder PILE_APPELLE a la bonne adresse le temps de faire le codegen 
    fprintf(out,"LOAD %d     ;APPELLE_FONCTION:On charge la tete de pile\n",REGISTRE_TMP_ADRESSE);
    fprintf(out,"STORE %d     ;APPELLE_FONCTION:On le stocke dans le registre d'adresse temporaire)\n",REGISTRE_PILE_APPELLE);
    NB_LIGNE += 2;
 
    fprintf(out, "JUMP %d    ;APPELLE_FONCTION:On saute vers la fonction\n",ligne_debut);//debut sinon on retombe sur le jump de debut de fonction
    NB_LIGNE += 1;

    //TODO optimiser dans le cas ou la fonction ne renvoie rien
    if (TABSYMB[id_fnt].Possede_renvoie){
        DEPILER();
        fprintf(out,"STORE %d     ;APPELLE_FONCTION post jump: On enregistre la valeur de renvoie)\n",REGISTRE_TMP_VALEUR);
        NB_LIGNE += COUT_DEPILER+1;
    }
    DEPILER();
    fprintf(out,"STORE %d     ;APPELLE_FONCTION post jump:\n",REGISTRE_PILE_APPELLE);
    NB_LIGNE += COUT_DEPILER+1;

    DEPILER();
    fprintf(out,"STORE %d     ;APPELLE_FONCTION post jump:\n",REGISTRE_TMP_ADRESSE);
    NB_LIGNE += COUT_DEPILER+1;

    if (TABSYMB[id_fnt].Possede_renvoie){
        fprintf(out,"LOAD %d     ;APPELLE_FONCTION post jump:\n",REGISTRE_TMP_VALEUR);
        NB_LIGNE += 1;
    }

}

/**
 * @brief Gère l'opération de retour d'une fonction en générant le code nécessaire pour renvoyer une valeur.
 * 
 * Cette fonction génère le code pour l'instruction de retour dans une fonction. 
 * Elle évalue d'abord l'expression à retourner, puis enregistre la valeur de retour dans un registre. 
 * Ensuite, elle met à jour la pile et les registres pour gérer correctement le retour à l'appelant, en veillant à ce que la valeur de retour soit correctement stockée et que l'exécution reprenne à l'adresse appropriée.
 * 
 * @param p Pointeur vers le nœud AST représentant l'expression de retour.
 */
void codegen_RENVOIE(ast *p) {
    
    codegen(p->suivant[0]); // Génère le code pour l'expression de retour

    fprintf(out,"STORE %d     ;RENVOIE:\n",REGISTRE_RENVOIE);
    NB_LIGNE += 1;
    
    fprintf(out,"DEC %d     ;RENVOIE\n",REGISTRE_PILE_APPELLE);
    //on decremente le debut de fonction local pour arriver la on on stocke nos informations
    NB_LIGNE+= 1;
    fprintf(out,"LOAD %d     ;RENVOIE:\n",REGISTRE_PILE_APPELLE);
    fprintf(out,"STORE %d     ;RENVOIE:\n",REGISTRE_PILE);
    NB_LIGNE += 2;

    fprintf(out,"LOAD @%d   ;RENVOIE:\n", REGISTRE_PILE);
    fprintf(out,"STORE %d     ;RENVOIE:\n",REGISTRE_RETOUR);
    NB_LIGNE += 2;

    fprintf(out,"LOAD %d   ;RENVOIE:\n", REGISTRE_RENVOIE);
    EMPILER();
    fprintf(out,"JUMP @%d   ;RENVOIE:\n", REGISTRE_RETOUR);
    NB_LIGNE += 2+COUT_EMPILER;
}
