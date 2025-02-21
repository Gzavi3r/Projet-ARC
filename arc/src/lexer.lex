%{
  #include <string.h>
  #include "parser.h"
  #include "erreur.h"

  extern void yyerror(char *);

  char errmsg[256]="";
  const char charerr[] = "\x1b[1m\x1b[31m[erreur lexicale]\x1b[0m caractère \x1b[41m%c\x1b[0m inattendu";
  
  /* MACRO défini 
   * Action executee avant chaque action semantique (meme vide)  
   * et rempli la variable Bison `yylloc` avec la position du token
   */
#define YY_USER_ACTION                                             \
  yylloc.first_line = yylloc.last_line;                            \
  yylloc.first_column = yylloc.last_column-3;                        \
  if (yylloc.last_line == yylineno)                                \
    yylloc.last_column += yyleng;                                  \
  else {                                                           \
    yylloc.last_line = yylineno;                                   \
    yylloc.last_column = 1;					   \
  }


  
%}

%option nounput
%option noinput
%option yylineno

ID              [_a-zA-Z]+[_a-zA-Z0-9]*
NOMBRE          ([1-9][0-9]*|0)  
/*https://efxa.org/2014/05/10/ignoring-multiline-comments-with-flex-start-states/*/
%x COMMENT

%%
"/*" { BEGIN(COMMENT); }

  <COMMENT>"*/" { BEGIN(INITIAL); }

  <COMMENT>(.|\n) ;

  <COMMENT><<EOF>> { yyerror("Unexpected file end (unterminated comment).\n"); }

(\/\/|#).*      {/* Ignore */}


"VAR"           { return VAR; }
"ALGO"          { return ALGO; }
"MAIN"          { return MAIN; }
"DEBUT"         { return DEBUT; }
"FIN"           { return FIN; }
"RENVOIE"       { return RENVOIE;}
"ECRIRE"        { return ECRIRE; }
"LIRE"          { return LIRE; }

"TQ"            { return TQ; }
"FAIRE"         { return FAIRE; }
"FTQ"           { return FTQ; }

"POUR"          { return POUR; }
"ALLANT"        { return ALLANT; }
"DE"            { return DE; }
"A"             { return A; }
"FPOUR"         { return FPOUR; }

"SI"            { return SI; }
"ALORS"         { return ALORS; }
"SINON"         { return SINON; }
"FSI"           { return FSI; }

"<-"            { return AFFECT; }


"NON"           {return NON;}
"ET"            {return ET;}
"OU"            {return OU;}
"VRAI"          {return VRAI;}
"FAUX"          {return FAUX;}
"!="            {return DIFF;}
">="            {return SUPEG;}
"<="            {return INFEG;}


[-+*/%();,<>=]     { return yytext[0]; }


{ID}            { strcpy(yylval.id, yytext); return ID; }


{NOMBRE}        { yylval.nb = atoi(yytext); return NB; }


[ \t\n]+        {    for (int i = 0; yytext[i] != '\0'; i++) {
                        if (yytext[i] == '\t') {
                            yylloc.first_column += 4; // Comptez chaque tabulation comme 4 espaces
                            yylloc.last_column += 4;
                        } else {
                            yylloc.first_column++;
                            yylloc.last_column++;
                        }
                      }
                }


.   { 
    update_position(yylloc);
    erreur(LEXICAL_ERROR,"Caractère inattendu '%s' à la ligne %d\n", yytext, yylineno);
}


%%
 
