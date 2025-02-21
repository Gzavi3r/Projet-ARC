/**
 * @file ast.c
 * @brief Implémentation des fonctions pour manipuler les arbres syntaxiques abstraits (AST).
 */

#include "ast.h"

// Variable globale pour suivre la profondeur d'affichage lors du débogage.
int profondeur = 0;

// Prototypes des fonctions d'affichage statiques.
static void PrintPROGRAMME(ast *p, char *indent);
static void PrintNB(ast *p, char * indent);
static void PrintOP(ast *p, char *indent);
static void PrintECRIRE(ast *p, char *indent);
static void PrintLIRE(ast *p, char *indent);
static void PrintOP_UNAIRE(ast *p, char *indent);
static void PrintBOOL(ast *p, char *indent);
static void PrintDECLA(ast *p, char *indent);
static void PrintPARAM(ast *p, char *indent);
static void PrintDECLA_VAR(ast *p, char *indent);
static void PrintDECLA_FONCTION(ast *p, char *indent);
static void PrintLIST_ID(ast *p, char *indent);
static void PrintLIST_EXP(ast *p, char *indent);
static void PrintLIST_INSTR(ast *p, char *indent);
static void PrintID(ast *p, char *indent);
static void PrintAFFECT(ast *p, char *indent);
static void PrintTQ(ast *p, char *indent);
static void PrintFAIRE_TQ(ast *p, char *indent);
static void PrintSI(ast *p, char *indent);
static void PrintPOUR(ast *p, char *indent);
static void PrintFONCTION(ast *p, char *indent);
static void PrintAPPELLE_FONCTION(ast *p, char *indent);
static void PrintRENVOIE(ast *p, char *indent);


/**
 * @brief Crée un nœud représentant un programme.
 * @param declaration Arbre des déclarations.
 * @param prog_main Arbre du programme principal.
 * @return Pointeur vers le nœud AST créé.
 */
ast* CreerNoeudPROGRAMME(ast* declaration, ast* prog_main) {
    ast* p;
    INIT_NOEUD(p);
    p->type = AST_PROGRAMME; 
    strcpy(p->type_str,"PROGRAMME");
    p->suivant[0] = declaration;
    p->suivant[1] = prog_main;
    return p;
}

/**
 * @brief Crée une feuille représentant un entier.
 * @param nb Valeur entière à stocker dans la feuille.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerFeuilleNB(int nb){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_NB;
  strcpy(p->type_str,"NB");
  p->valeur = nb;
  return p;
}

/**
 * @brief Crée une feuille représentant un identifiant.
 * @param id Nom de l'identifiant.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerFeuilleID(char* id){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_ID;
  strcpy(p->type_str,"ID");
  strcpy(p->nom_id,id);
  return p;
}

/**
 * @brief Crée un nœud pour une instruction d'écriture.
 * @param exp Expression à écrire.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudECRIRE(ast * exp){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_ECRIRE;
  strcpy(p->type_str,"ECRIRE");
  p->suivant[0] = exp;
  return p;
}

/**
 * @brief Crée un nœud pour une instruction de lecture.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudLIRE(){
  ast * p;
  INIT_NOEUD(p);
  p->type = AST_LIRE;
  strcpy(p->type_str,"LIRE");
  return p;
}

/**
 * @brief Crée un nœud pour une liste d'instructions.
 * @param p1 Première instruction.
 * @param p2 Liste d'instructions suivantes.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudLIST_INSTR( ast* p1, ast* p2){
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_LIST_INSTR;
  strcpy(p->type_str,"LIST_INSTR");
  p->suivant[0] = p1;
  p->suivant[1] = p2;
  return p;
}

/**
 * @brief Crée un nœud pour des déclarations de variables et de fonctions.
 * @param decla_var Déclaration des variables.
 * @param decla_fct Déclaration des fonctions.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudDECLA( ast* decla_var, ast* decla_fct){
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_DECLA;
  strcpy(p->type_str,"DECLA");
  p->suivant[0] = decla_var;
  p->suivant[1] = decla_fct;
  
  ast* list_id;
  while (decla_var != NULL){
    list_id = decla_var->suivant[0];
    while (list_id != NULL){   
      p->nb_declaration += 1;
      list_id = list_id->suivant[0];
    } 
    decla_var = decla_var->suivant[1];
  }
  return p;
}

/**
 * @brief Crée un nœud pour les paramètres d'une fonction.
 * @param list_id Liste des identifiants des paramètres.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudPARAM( ast* list_id){
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_PARAM;
  strcpy(p->type_str,"PARAM");
  p->suivant[0] = list_id;
  return p;
}

/**
 * @brief Crée un nœud pour une déclaration de variable.
 * @param list_id Liste des identifiants.
 * @param exp Expression associée à la déclaration (facultative).
 * @param suivant Nœud suivant dans la liste des déclarations.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudDECLA_VAR( ast* list_id,ast * exp, ast* suivant){
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_DECLA_VAR;
  strcpy(p->type_str,"DECLA_VAR");
  p->suivant[0] = list_id;
  p->suivant[1] = suivant;
  p->suivant[2] = exp;
  return p;
}

/**
 * @brief Crée un nœud pour une déclaration de fonction.
 * @param fonction Fonction à déclarer.
 * @param suivant Nœud suivant dans la liste des déclarations.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudDECLA_FONCTION( ast* fonction, ast* suivant){
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_DECLA_FONCTION;
  strcpy(p->type_str,"DECLA_FONCTION");
  p->suivant[0] = fonction;
  p->suivant[1] = suivant;
  return p;
}



/*
Permet de créer une structure de type:
    List_Id
    |     |
    |     |
    v     v
   Id    List_Id
*/
/**
 * @brief Crée un nœud pour une liste d'identifiants.
 * @param id Identifiant actuel.
 * @param p1 Liste des identifiants suivants.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudLIST_ID(char* id ,ast* p1){
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_LIST_ID;
  strcpy(p->type_str,"LIST_ID");
  strcpy(p->nom_id ,id);
  p->suivant[0] = p1;
  return p;
}

/**
 * @brief Crée un nœud pour une liste d'expressions.
 * @param exp Expression actuelle.
 * @param list_exp Liste des expressions suivantes.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudLIST_EXP(ast* exp , ast* list_exp){
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_LIST_EXP;
  strcpy(p->type_str,"LIST_EXP");
  p->suivant[0] = exp;
  p->suivant[1] = list_exp;
  return p;
}

/**
 * @brief Crée un nœud pour une opération binaire.
 * @param operateur Opérateur de l'opération.
 * @param p1 Opérande gauche.
 * @param p2 Opérande droite.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudOP(int operateur, ast* p1, ast* p2){
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_OP;
  strcpy(p->type_str,"OP");
  p->op = operateur;
  p->suivant[0] = p1;
  p->suivant[1] = p2;
  return p;
}

/**
 * @brief Crée un nœud pour une opération unaire.
 * @param op Opérateur unaire.
 * @param p1 Opérande.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudOP_UNAIRE(int op, ast *p1)
{
    ast* p;
    INIT_NOEUD(p);
    p->type = AST_OP_UNAIRE;
    strcpy(p->type_str,"OP_UNAIRE");
    p->op = op;
    p->suivant[0] = p1;

    return p;
}

//même code que CreerNoeudOP mais réécris pour une meilleur lisibilité dans le codegen
/**
 * @brief Crée un nœud pour une opération booléenne.
 * @param operateur Opérateur booléen.
 * @param p1 Opérande gauche.
 * @param p2 Opérande droite.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudBOOL(int operateur, ast* p1, ast* p2){
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_BOOL;
  strcpy(p->type_str,"BOOL");
  p->op = operateur;
  p->suivant[0] = p1;
  p->suivant[1] = p2;
  return p;
}

/**
 * @brief Crée un nœud pour une affectation.
 * @param id Nom de l'identifiant auquel la valeur sera affectée.
 * @param p1 Expression ou valeur à affecter.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudAFFECT( char* id, ast* p1){
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_AFFECT;
  strcpy(p->type_str,"<-");
  strcpy(p->nom_id,id); 
  p->suivant[1] = p1;
  return p;
}

/**
 * @brief Crée un nœud pour une boucle Tant Que.
 * @param p1 Condition de la boucle.
 * @param p2 Liste d'instructions à exécuter.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudTQ(ast* p1, ast* p2){
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_TQ;
  strcpy(p->type_str,"TQ");
  p->suivant[0] = p1;
  p->suivant[1] = p2;
  return p;
}

/**
 * @brief Crée un nœud pour une boucle Faire Tant Que.
 * @param p1 Liste d'instructions à exécuter au moins une fois.
 * @param p2 Condition de la boucle.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudFAIRE_TQ(ast* p1, ast* p2){
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_FAIRE_TQ;
  strcpy(p->type_str,"FAIRE_TQ");
  p->suivant[0] = p1;
  p->suivant[1] = p2;
  return p;
}

/**
 * @brief Crée un nœud pour une boucle Pour.
 * @param id Nom de l'indice de boucle.
 * @param debut Valeur initiale de l'indice.
 * @param fin Valeur finale de l'indice.
 * @param list_inst Liste d'instructions à exécuter.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudPOUR(char * id,ast * debut, ast* fin, ast * list_inst){
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_POUR;
  strcpy(p->type_str,"POUR");
  strcpy(p->nom_id,id); 
  p->suivant[0] = CreerNoeudAFFECT(id,debut);
  p->suivant[1] = CreerNoeudBOOL('<',CreerFeuilleID(id),fin);
  p->suivant[2] = list_inst;
  return p;
}

/**
 * @brief Crée un nœud pour une structure conditionnelle Si.
 * @param exp Expression conditionnelle.
 * @param list_instr1 Liste d'instructions si la condition est vraie.
 * @param list_instr2 Liste d'instructions si la condition est fausse (peut être NULL).
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudSI(ast* exp, ast* list_instr1, ast* list_instr2){
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_SI;
  strcpy(p->type_str,"SI");
  p->suivant[0] = exp;
  p->suivant[1] = list_instr1;
  p->suivant[2] = list_instr2;
  return p;
}

/**
 * @brief Crée un nœud pour une fonction.
 * @param id Nom de la fonction.
 * @param param Paramètres de la fonction (peut être NULL).
 * @param decla Déclarations locales dans la fonction (peut être NULL).
 * @param L_instr Liste d'instructions dans le corps de la fonction.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudFONCTION(char * id, ast* param, ast* decla,ast * L_instr){
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_FONCTION;
  strcpy(p->type_str,"FONCTION");
  strcpy(p->nom_id,id);
  p->suivant[0] = param;
  p->suivant[1] = decla;
  p->suivant[2] = L_instr;
  //on compte le nombre de parametre que possède la fonction
  if (param != NULL) param = param->suivant[0];
  while (param != NULL){
    p->nb_param += 1;
    param = param->suivant[0];

  }
  ast* list_id;
  while (decla != NULL){
    list_id = decla->suivant[0];
    while (list_id != NULL){   
      p->nb_declaration += 1;
      list_id = list_id->suivant[0];
    } 
    decla = decla->suivant[1];
  }
  return p;
}


/**
 * @brief Crée un nœud pour un appel de fonction.
 * @param id Nom de la fonction appelée.
 * @param args Arguments passés à la fonction (peut être NULL).
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudAPPELLE_FONCTION(char * id, ast* args){
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_APPELLE_FONCTION;
  strcpy(p->type_str,"APPELLE_FONCTION");
  strcpy(p->nom_id,id); 
  p->suivant[0] = args;
  return p;
}

/**
 * @brief Crée un nœud pour une instruction de retour de valeur.
 * @param p1 Expression ou valeur à retourner.
 * @return Pointeur vers le nœud AST créé.
 */
ast * CreerNoeudRENVOIE(ast* p1){
  ast* p;
  INIT_NOEUD(p);
  p->type = AST_RENVOIE;
  strcpy(p->type_str,"RENVOIE");
  p->suivant[0] = p1;
  return p;
}


/**
 * @brief Libère la mémoire allouée pour un AST.
 * @param p Pointeur vers le nœud à libérer.
 */
void FreeAst(ast * p){
  if (p->suivant[0] != NULL) FreeAst(p->suivant[0]);
  if (p->suivant[1] != NULL) FreeAst(p->suivant[1]);
  if (p->suivant[2] != NULL) FreeAst(p->suivant[2]);
  free(p);
}

/**
 * @brief Affiche un AST pour le débogage.
 * @param p Pointeur vers le nœud AST à afficher.
 */
void PrintAst(ast * p){
  if (p == NULL) return;
  char indent[32] ="";
  int i = 0;
  for(i=0;i<profondeur;i++){
    indent[i] = '\t';
  }
  indent[i] = '\0';
  printf("%s" TXT_BOLD TXT_GREEN "Taille du Bloc: "TXT_NULL "%d\n",indent, p->codelen);
  switch(p->type){
  case AST_PROGRAMME:
    PrintPROGRAMME(p,indent);
    break;
  case AST_NB:
    PrintNB(p,indent);
    break;
  case AST_ID:
    PrintID(p,indent);
    break;
  case AST_ECRIRE:
    PrintECRIRE(p,indent);
    break;
  case AST_LIRE:
    PrintLIRE(p,indent);
    break;
  case AST_DECLA:
    PrintDECLA(p,indent);
    break;   
  case AST_PARAM:
    PrintPARAM(p,indent);
    break;
  case AST_DECLA_VAR:
    PrintDECLA_VAR(p,indent);
    break;
  case AST_DECLA_FONCTION:
    PrintDECLA_FONCTION(p,indent);
    break;
  case AST_OP_UNAIRE:
    PrintOP_UNAIRE(p,indent);
    break;
  case AST_OP:
    PrintOP(p,indent);
    break;
  case AST_BOOL:
    PrintBOOL(p,indent);
    break;
  case AST_LIST_ID:
    PrintLIST_ID(p,indent);
    break;
  case AST_LIST_EXP:
    PrintLIST_EXP(p,indent);
    break;
  case AST_LIST_INSTR:
    PrintLIST_INSTR(p,indent);
    break;
  case AST_AFFECT:
    PrintAFFECT(p,indent);
    break;
  case AST_TQ:
    PrintTQ(p,indent);
    break;
  case AST_FAIRE_TQ:
    PrintFAIRE_TQ(p,indent);
    break;
  case AST_POUR:
    PrintPOUR(p,indent);
    break;
  case AST_SI:
    PrintSI(p,indent);
    break;
  case AST_FONCTION:
    PrintFONCTION(p,indent);
    break;
  case AST_APPELLE_FONCTION:
    PrintAPPELLE_FONCTION(p,indent);
    break;
  case AST_RENVOIE:
    PrintRENVOIE(p,indent);
    break;
  default:
    fprintf(stderr,"[Erreur] type <%d>: %s non reconnu\n",p->type,p->type_str);
    break;
  }
}

/**
 * @brief Gère une erreur spécifique à l'AST.
 * @param errmsg Message d'erreur.
 */
void ErrorAst(const char * errmsg){
  fprintf(stderr,"[AST error] %s\n",errmsg);
  exit(1);
}

// === Fonctions d'affichage statiques ===

/**
 * @brief Affiche un nœud de type PROGRAMME.
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintPROGRAMME(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Programme :  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD TXT_BLUE "Declaration :  " TXT_NULL "%p\n",indent, p->suivant[0]);
  printf("%s" TXT_BOLD TXT_BLUE "Main:  " TXT_NULL "%p\n",indent, p->suivant[1]);
  profondeur++;
  PrintAst(p->suivant[0]);
  PrintAst(p->suivant[1]);
  profondeur--;
}

/**
 * @brief Affiche un nœud de type NB (entier).
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintNB(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Feuille:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "Valeur: " TXT_NULL "%d\n",indent, p->valeur);
}

/**
 * @brief Affiche un nœud de type ID (identifiant).
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintID(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Feuille:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "Nom: " TXT_NULL "%s\n",indent, p->nom_id);
}

/**
 * @brief Affiche un nœud de type LIRE.
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintECRIRE(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD TXT_BLUE "Expression :  " TXT_NULL "%p\n",indent, p->suivant[0]);
  profondeur++;
  PrintAst(p->suivant[0]);
  profondeur--;
}

/**
 * @brief Affiche un nœud de type LIRE.
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintLIRE(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
}

/**
 * @brief Affiche un nœud de type DECLA (Déclaration).
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintDECLA(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD TXT_BLUE "Declaration de VAR:  " TXT_NULL "%p\n",indent, p->suivant[0]);
  printf("%s" TXT_BOLD TXT_BLUE "Declaration de fonction:  " TXT_NULL "%p\n",indent, p->suivant[1]);
  profondeur++;
  PrintAst(p->suivant[0]);
  PrintAst(p->suivant[1]);
  profondeur--;
}

/**
 * @brief Affiche un nœud de type PARAM (Paramètre).
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintPARAM(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD TXT_BLUE "Parametre:  " TXT_NULL "%p\n",indent, p->suivant[0]);
  profondeur++;
  PrintAst(p->suivant[0]);
  profondeur--;
}

/**
 * @brief Affiche un nœud de type DECLA_VAR (Déclaration de variables).
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintDECLA_VAR(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD TXT_BLUE "Liste d'ID :  " TXT_NULL "%p\n",indent, p->suivant[0]);
  printf("%s" TXT_BOLD TXT_BLUE "Suivant :  " TXT_NULL "%p\n",indent, p->suivant[1]);
  if (p->suivant[2] != NULL){
    printf("%s" TXT_BOLD TXT_BLUE "Expression affectée :  " TXT_NULL "%p\n",indent, p->suivant[2]);
  }
  profondeur++;
  PrintAst(p->suivant[0]);
  PrintAst(p->suivant[1]);
  PrintAst(p->suivant[2]);
  profondeur--;
}

/**
 * @brief Affiche un nœud de type DECLA_FONCTION (Déclaration de fonction).
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintDECLA_FONCTION(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD TXT_BLUE "Fonction:  " TXT_NULL "%p\n",indent, p->suivant[0]);
  printf("%s" TXT_BOLD TXT_BLUE "Suivant:  " TXT_NULL "%p\n",indent, p->suivant[1]);
  profondeur++;
  PrintAst(p->suivant[0]);
  PrintAst(p->suivant[1]);
  profondeur--;
}

/**
 * @brief Affiche un nœud de type LIST_ID (Liste d'identifiants).
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintLIST_ID(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD TXT_BLUE "ID :  " TXT_NULL "%s\n",indent, p->nom_id);
  printf("%s" TXT_BOLD TXT_BLUE "List ID suivante:  " TXT_NULL "%p\n",indent, p->suivant[0]);
  profondeur++;
  PrintAst(p->suivant[0]);
  profondeur--;
}

/**
 * @brief Affiche un nœud de type LIST_EXP (Liste d'expressions).
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintLIST_EXP(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD TXT_BLUE "Expression:  " TXT_NULL "%p\n",indent, p->suivant[0]);
  printf("%s" TXT_BOLD TXT_BLUE "EXP suivante':  " TXT_NULL "%p\n",indent, p->suivant[1]);
  profondeur++;
  PrintAst(p->suivant[0]);
  PrintAst(p->suivant[1]);
  profondeur--;
}

/**
 * @brief Affiche un nœud de type OP (opération binaire).
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintOP(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "Operateur: " TXT_NULL "%c\n",indent, p->op);
  printf("%s" TXT_BOLD TXT_BLUE "Opérande Gauche:  " TXT_NULL "%p\n",indent, p->suivant[0]);
  printf("%s" TXT_BOLD TXT_BLUE "Opérande Droite':  " TXT_NULL "%p\n",indent, p->suivant[1]);
  profondeur++;
  PrintAst(p->suivant[0]);
  PrintAst(p->suivant[1]);
  profondeur--;
}

/**
 * @brief Affiche un nœud de type OP_UNAIRE (Opérateur unaire).
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintOP_UNAIRE(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "Operateur: " TXT_NULL "%c\n",indent, p->op);
  printf("%s" TXT_BOLD TXT_BLUE "Opérande :  " TXT_NULL "%p\n",indent, p->suivant[0]);
  profondeur++;
  PrintAst(p->suivant[0]);
  profondeur--;
}

/**
 * @brief Affiche un nœud de type BOOL.
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintBOOL(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "Operateur: " TXT_NULL "%c\n",indent, p->op);
  printf("%s" TXT_BOLD TXT_BLUE "Opérande Gauche:  " TXT_NULL "%p\n",indent, p->suivant[0]);
  printf("%s" TXT_BOLD TXT_BLUE "Opérande Droite':  " TXT_NULL "%p\n",indent, p->suivant[1]);
  profondeur++;
  PrintAst(p->suivant[0]);
  PrintAst(p->suivant[1]);
  profondeur--;
}

/**
 * @brief Affiche un nœud de type LIST_INSTR (liste d'instructions).
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintLIST_INSTR(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  profondeur++;
  PrintAst(p->suivant[0]);
  printf("\n");
  PrintAst(p->suivant[1]);
  profondeur--;
}

/**
 * @brief Affiche un nœud de type AFFECT (affectation).
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintAFFECT(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD TXT_BLUE "Nom: " TXT_NULL "%s\n",indent, p->nom_id);
  printf("%s" TXT_BOLD TXT_BLUE "Expression affectée:  " TXT_NULL "%p\n",indent, p->suivant[1]);
  profondeur++;
  PrintAst(p->suivant[1]);
  profondeur--;
}

/**
 * @brief Affiche un nœud de type TQ (tant que).
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintTQ(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD TXT_BLUE "Condition :  " TXT_NULL "%p\n",indent, p->suivant[0]);
  printf("%s" TXT_BOLD TXT_BLUE "Liste Instruction:  " TXT_NULL "%p\n",indent, p->suivant[1]);
  profondeur++;
  PrintAst(p->suivant[0]);
  PrintAst(p->suivant[1]);
  profondeur--;
}

/**
 * @brief Affiche un nœud de type FAIRE_TQ (faire tant que).
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintFAIRE_TQ(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD TXT_BLUE "Liste Instruction :  " TXT_NULL "%p\n",indent, p->suivant[0]);
  printf("%s" TXT_BOLD TXT_BLUE "Condition :  " TXT_NULL "%p\n",indent, p->suivant[1]);
  profondeur++;
  PrintAst(p->suivant[0]);
  PrintAst(p->suivant[1]);
  profondeur--;
}

/**
 * @brief Affiche un nœud de type POUR (boucle pour).
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintPOUR(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "ID :   " TXT_NULL "%s\n",indent, p->nom_id);
  printf("%s" TXT_BOLD TXT_BLUE "Allant de :  " TXT_NULL "%p\n",indent, p->suivant[0]);
  printf("%s" TXT_BOLD TXT_BLUE "a :  " TXT_NULL "%p\n",indent, p->suivant[1]);
  printf("%s" TXT_BOLD TXT_BLUE "Faire:  " TXT_NULL "%p\n",indent, p->suivant[2]);

  profondeur++;
  PrintAst(p->suivant[0]);
  PrintAst(p->suivant[1]);
  PrintAst(p->suivant[2]);
  profondeur--;
}

/**
 * @brief Affiche un nœud de type SI (instruction conditionnelle).
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintSI(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD TXT_BLUE "Condition :  " TXT_NULL "%p\n",indent, p->suivant[0]);
  printf("%s" TXT_BOLD TXT_BLUE "ALORS:  " TXT_NULL "%p\n",indent, p->suivant[1]);
  printf("%s" TXT_BOLD TXT_BLUE "SINON:  " TXT_NULL "%p\n",indent, p->suivant[2]);

  profondeur++;
  PrintAst(p->suivant[0]);
  PrintAst(p->suivant[1]);
  PrintAst(p->suivant[2]);
  profondeur--;
}

/**
 * @brief Affiche un nœud de type FONCTION (fonction).
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintFONCTION(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "Nom de la fonction: " TXT_NULL "%s\n",indent, p->nom_id);
  printf("%s" TXT_BOLD "nombre de parametre: " TXT_NULL "%d\n",indent, p->nb_param);
  printf("%s" TXT_BOLD "nombre de declaration: " TXT_NULL "%d\n",indent, p->nb_declaration);
  printf("%s" TXT_BOLD TXT_BLUE "Liste des parametres :  " TXT_NULL "%p\n",indent, p->suivant[0]);
  printf("%s" TXT_BOLD TXT_BLUE "Liste des declarations:  " TXT_NULL "%p\n",indent, p->suivant[1]);
  printf("%s" TXT_BOLD TXT_BLUE "Liste Instruction:  " TXT_NULL "%p\n",indent, p->suivant[2]);
  profondeur++;
  PrintAst(p->suivant[0]);
  PrintAst(p->suivant[1]);
  PrintAst(p->suivant[2]);
  profondeur--;
}

/**
 * @brief Affiche un nœud de type APPELLE_FONCTION (appel de fonction).
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintAPPELLE_FONCTION(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD "fonction appelee:" TXT_NULL "%s\n",indent, p->nom_id);
  printf("%s" TXT_BOLD TXT_BLUE "Liste des argument :  " TXT_NULL "%p\n",indent, p->suivant[0]);
  profondeur++;
  PrintAst(p->suivant[0]);
  profondeur--;
}

/**
 * @brief Affiche un nœud de type RENVOIE (instruction de retour).
 * @param p Pointeur vers le nœud AST.
 * @param indent Indentation pour un affichage hiérarchique.
 */
static void PrintRENVOIE(ast *p, char *indent){
  printf("%s" TXT_BOLD TXT_BLUE "Noeud:  " TXT_NULL "%p\n",indent, p);
  printf("%s" TXT_BOLD "Type:   " TXT_NULL "%s\n",indent, p->type_str);
  printf("%s" TXT_BOLD TXT_BLUE "Renvoie :  " TXT_NULL "%p\n",indent, p->suivant[0]);
  profondeur++;
  PrintAst(p->suivant[0]);
  profondeur--;
}
