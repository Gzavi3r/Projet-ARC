/**
 * @file parser.y
 * @brief Analyseur syntaxique et sémantique pour le langage.
 * 
 * Ce fichier contient la définition de l'analyseur syntaxique et sémantique du programme, utilisant Flex et Bison pour analyser un fichier source. L'analyseur génère un arbre syntaxique abstrait (AST) à partir du code source et effectue la vérification sémantique avant de produire le code intermédiaire à l'aide du générateur de code.
 * 
 * Le programme commence par initialiser les tables de symboles et les autres structures nécessaires avant de procéder à l'analyse lexicale et syntaxique. Ensuite, il effectue les vérifications sémantiques sur l'AST et génère le code final. Le tout est écrit dans un fichier de sortie spécifié par l'utilisateur.
 * 
 * Les fonctions suivantes sont définies dans ce fichier :
 * - `main` : Point d'entrée du programme, qui charge le fichier source et lance l'analyse.
 * - `print_file_error` : Affiche un message d'erreur en cas de problème avec l'ouverture du fichier source.
 * - `yyerror` : Gère les erreurs de syntaxe détectées par Bison.
 * 
 * La grammaire utilisée dans ce fichier définit la structure du programme, des déclarations de variables, des fonctions, des boucles, des conditions, etc. Elle crée un arbre syntaxique abstrait (AST) qui sera utilisé pour l'analyse sémantique et la génération de code.
 */
%{
  #include <stdio.h>
  #include <ctype.h>
  #include <string.h>
  
  #include "ast.h"
  #include "ts.h" 
  #include "codegen.h"
  #include "semantic.h"
  #include "erreur.h"


  extern int yydebug; // Déclaration de la variable globale
  extern int yylex();
  static void print_file_error(char * s, char *errmsg);
  ast* affecter_valeur(ast* p1,ast* p2);

  struct ast * ARBRE_ABSTRAIT = NULL;
  ts TABSYMB;
  char CTXT[32] = "GLOBAL";

  void yyerror(const char * s);

  char srcname[64];
  char exename[64] = "a.out";
  FILE * out;
%}

%debug
%union {
  int nb;                // Pour les nombres
  char id[32];           // Pour les identifiants
  struct ast* arbre;     // Pour l'arbre syntaxique abstrait
}
%define parse.error detailed
%locations


%token <nb> NB                                  // Nombre entier
%token <id> ID                                  // Identifiant
%token ALGO MAIN '(' ')' DEBUT FIN ',' ';' RENVOIE  // Mots-clés
%token TQ FTQ FAIRE                         // Boucle TantQue
%token POUR ALLANT DE A FPOUR                   // Boucle Pour
%token SI ALORS SINON FSI
%token AFFECT "<-"                              // Symbole d'affectation
%token SUPEG ">="
%token INFEG "<="
%token DIFF "!="
%token VRAI FAUX
%token ECRIRE LIRE


%precedence FAIRE TQ

%token VAR                                      // Déclaration de variables

%right "<-"                              // Précédence pour l'affectation

%left OU
%left ET
%left NON
%left '<' '>' '=' "<=" ">=" "!="

%left '+' '-'                            // Précédence pour addition/soustraction
%left '*' '/' '%'                        // Précédence pour multiplication/division/modulo

%type <arbre> PROGRAMME                  // Programme complet
%type <arbre> PROG_MAIN                  // Partie principale (main)
%type <arbre> LIST_DECLA                 // Un Noeud contenant la Liste des variables et la liste des fonctions
%type <arbre> LIST_DECLA_VAR             // Liste des déclarations de variables
%type <arbre> LIST_DECLA_FONCTION        // Liste des déclarations de fonctions
%type <arbre> FONCTION                   // Déclaration d'une fonction
%type <arbre> LIST_PARAM                 // Liste des paramètres d'une fonction
%type <arbre> LIST_ID                    // Liste d'identifiants
%type <arbre> APPELLE_FONCTION
%type <arbre> LIST_EXPR
%type <arbre> LIST_INSTR                // Liste des instructions
%type <arbre> INSTR                      // Instruction
%type <arbre> EXP                        // Expression
%type <arbre> BOUCLE_TQ
%type <arbre> BOUCLE_FAIRE_TQ
%type <arbre> FIN_FAIRE_TQ
%type <arbre> BOUCLE_POUR
%type <arbre> CONDITION_SI

// Axiome
%start PROGRAMME                         

%%

/*
Gramaire d'une fonction
ALGO ID *(LIST_ID**)
  DECLA VAR***
  DEBUT
    LIST_INST****
  FIN

* Ajouter l'id de la fonctiona la TSYMB en contexte global puis changer le contexte en utilisant l'id {strcpy(CTXT,$2)}

** ajouter l'id du parametre de la fonction

***Declaration normal avec le contexte modifier

**** on recherche les id dans le local puis dans le global

Grammaire d'un programme
DECLA_VAR_GLOBAL

DECLA FONCTION

DECLA MAIN

On recherche une variable en local puis en global 
*/

PROGRAMME: LIST_DECLA PROG_MAIN {$$ = CreerNoeudPROGRAMME($1 , $2); ARBRE_ABSTRAIT = $$;}


LIST_DECLA: %empty   {$$ = NULL;}
          | LIST_DECLA_VAR LIST_DECLA_FONCTION {$$ = CreerNoeudDECLA($1,$2);}
          ;

LIST_DECLA_VAR: VAR LIST_ID ';' LIST_DECLA_VAR { $$ = CreerNoeudDECLA_VAR($2,NULL, $4); }
              | %empty                         { $$ = NULL; }
              | VAR LIST_ID "<-" EXP ';' LIST_DECLA_VAR  { $$ = CreerNoeudDECLA_VAR($2,$4, $6); }
              ;

LIST_ID: ID                          { $$ = CreerNoeudLIST_ID($1, NULL); }
       | ID  ',' LIST_ID              { $$ = CreerNoeudLIST_ID($1, $3); }
       ;


LIST_DECLA_FONCTION: FONCTION LIST_DECLA_FONCTION   {$$ = CreerNoeudDECLA_FONCTION($1,$2);}
                   |%empty {$$ = NULL;}
                   ;

PROG_MAIN: MAIN '(' ')' 
           LIST_DECLA_VAR 
           DEBUT 
           LIST_INSTR 
           FIN  {$$ = CreerNoeudFONCTION("MAIN",NULL,$4,$6);}



FONCTION : ALGO ID  '(' LIST_PARAM ')' 
           LIST_DECLA_VAR
           DEBUT
           LIST_INSTR
           FIN         {$$ = CreerNoeudFONCTION($2,$4,$6,$8);}

LIST_PARAM: %empty       {$$ = NULL;}
          | LIST_ID      {$$ = CreerNoeudPARAM($1);}
          ;


LIST_INSTR: INSTR                   {$$ = CreerNoeudLIST_INSTR($1, NULL);}
           | INSTR  LIST_INSTR      {$$ = CreerNoeudLIST_INSTR($1, $2);}
           ;


INSTR: EXP  ';'                {$$ = $1;}
     | BOUCLE_TQ               {$$ = $1;}  
     | CONDITION_SI            {$$ = $1;}
     | BOUCLE_FAIRE_TQ         {$$ = $1;}
     | BOUCLE_POUR             {$$ = $1;}
     | RENVOIE EXP  ';'        {$$ = CreerNoeudRENVOIE($2);}
     | ECRIRE '(' EXP ')' ';'  {$$ = CreerNoeudECRIRE($3);}
     ;


EXP  : NB                    { $$ = CreerFeuilleNB($1) ;} 
     | ID                    { $$ = CreerFeuilleID($1) ;}
     | ID "<-" EXP           {$$ = CreerNoeudAFFECT($1, $3);}
     | NON EXP               {$$ = CreerNoeudOP_UNAIRE(OP_NON, $2);}
     | '-' EXP               {$$ = CreerNoeudOP_UNAIRE('-', $2);}
     | EXP '+' EXP           { $$ = CreerNoeudOP('+',$1,$3) ;}
     | EXP '-' EXP           { $$ = CreerNoeudOP('-',$1,$3) ;} 
     | EXP '*' EXP           { $$ = CreerNoeudOP('*',$1,$3) ;}
     | EXP '/' EXP           { $$ = CreerNoeudOP('/',$1,$3) ;}
     | EXP '%' EXP           { $$ = CreerNoeudOP('%',$1,$3) ;}
     | EXP '<' EXP           {$$ = CreerNoeudBOOL('<', $1, $3);}
     | EXP '>' EXP           {$$ = CreerNoeudBOOL('>', $1, $3);}
     | EXP '=' EXP           {$$ = CreerNoeudBOOL('=', $1, $3);}
     | EXP "<=" EXP          {$$ = CreerNoeudBOOL(OP_INFEG, $1, $3);}
     | EXP ">=" EXP          {$$ = CreerNoeudBOOL(OP_SUPEG, $1, $3);}
     | EXP "!=" EXP          {$$ = CreerNoeudBOOL(OP_DIFF, $1, $3);}
     | EXP OU EXP            {$$ = CreerNoeudBOOL(OP_OU, $1, $3);}
     | EXP ET EXP            {$$ = CreerNoeudBOOL(OP_ET, $1, $3);}
     | VRAI                  {$$ = CreerFeuilleNB(1);}
     | FAUX                  {$$ = CreerFeuilleNB(0);}
     | '(' EXP ')'           {$$ = $2 ;}
     | APPELLE_FONCTION      {$$ = $1;}
     | LIRE '('')'        {$$ = CreerNoeudLIRE();}
     ;

APPELLE_FONCTION : ID '(' LIST_EXPR ')'  {$$ = CreerNoeudAPPELLE_FONCTION($1, $3);}     

LIST_EXPR: %empty      {$$ = NULL;}
         | EXP                 { $$ = CreerNoeudLIST_EXP($1, NULL); }
         | EXP ',' LIST_EXPR   { $$ = CreerNoeudLIST_EXP($1, $3); }
         ;
        

BOUCLE_TQ: TQ EXP FAIRE LIST_INSTR FTQ  {$$ = CreerNoeudTQ($2,$4);}

//N'est pas reconnu par bison
BOUCLE_FAIRE_TQ: FAIRE LIST_INSTR FIN_FAIRE_TQ    {$$ = CreerNoeudFAIRE_TQ($2, $3);}
FIN_FAIRE_TQ: TQ EXP ';'                          {$$ = $2;}

//meme principe que la boucle for i in range(x,y) en python /!\ i a besoin d'exister
BOUCLE_POUR: POUR ID ALLANT DE EXP A EXP FAIRE LIST_INSTR FPOUR      {$$ = CreerNoeudPOUR($2, $5, $7, $9);}



CONDITION_SI: SI EXP ALORS  LIST_INSTR SINON  LIST_INSTR FSI    {$$ = CreerNoeudSI($2, $4, $6);}
         | SI EXP ALORS  LIST_INSTR FSI  {$$ = CreerNoeudSI($2, $4, NULL);}
         ;





%%

/**
 * @brief Fonction d'entrée principale pour le traitement du fichier source et la génération de l'AST.
 * 
 * La fonction `main` sert de point d'entrée du programme. Elle charge le fichier source passé en argument, effectue l'analyse lexicale et syntaxique à l'aide de Bison, puis exécute l'analyse sémantique et la génération de code. Elle gère également la gestion d'erreurs en cas de problème avec les fichiers source ou d'entrée de programme.
 * 
 * @param argc Nombre d'arguments passés en ligne de commande.
 * @param argv Tableau des arguments passés en ligne de commande, contenant le nom du fichier source et du fichier de sortie.
 * @return Retourne 0 si le programme s'exécute correctement, sinon retourne une valeur d'erreur.
 */
int main( int argc, char * argv[] ) {
  extern FILE *yyin;
  
  if (argc > 1){
    strcpy(srcname, argv[1]);
    if ( (yyin = fopen(srcname,"r"))==NULL ){
      char errmsg[256];
      sprintf(errmsg,"fichier \x1b[1m\x1b[33m' %s '\x1b[0m introuvable",srcname);
      print_file_error(argv[0],errmsg);
      exit(1);
    }
  }  else {
    print_file_error(argv[0],"aucun fichier en entree");
    exit(1);
  }
  if (argc == 3){
    strcpy(exename, argv[2]);
  }
  out = fopen(exename,"w");
  INIT_TS(TABSYMB);
  //yydebug = 1; // Activer le mode débogage
  yyparse(); //Analyse lexicale et syntaxique

  semantic(ARBRE_ABSTRAIT);
  semantic_utilisation(ARBRE_ABSTRAIT);
  codegen_INIT(TABSYMB);
  codegen(ARBRE_ABSTRAIT);
  PrintTS(TABSYMB);
  PrintAst(ARBRE_ABSTRAIT);
  
  FreeAst(ARBRE_ABSTRAIT);
  fclose(yyin);
}

/**
 * @brief Affiche un message d'erreur de fichier en cas d'échec de l'ouverture d'un fichier source.
 * 
 * Cette fonction est utilisée pour afficher un message d'erreur lorsqu'un fichier source ne peut pas être ouvert. Elle affiche un message détaillant le problème, incluant le nom du programme et le message d'erreur spécifique.
 * 
 * @param prog Nom du programme appelant.
 * @param errmsg Message d'erreur à afficher.
 */
static void print_file_error(char * prog, char *errmsg){
  fprintf(stderr,
	  "\x1b[1m%s:\x1b[0m \x1b[31m\x1b[1merreur fatale:\x1b[0m %s\nechec de la compilation\n",
	  prog, errmsg);
}

/**
 * @brief Affiche un message d'erreur détaillé et termine l'exécution du programme.
 * 
 * Cette fonction est appelée lorsqu'une erreur de syntaxe est rencontrée. Elle affiche un message d'erreur contenant la ligne, la colonne et le type d'erreur. Après l'affichage, le programme termine son exécution avec le code d'erreur correspondant.
 * 
 * @param s Message d'erreur à afficher, qui peut être formaté avec des spécificateurs de format.
 */
void yyerror(const char *s) {
    update_position(yylloc);
    erreur(SYNTAX_ERROR, "%s", s);
}
