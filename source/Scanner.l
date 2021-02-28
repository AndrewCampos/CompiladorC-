%{
#include "globals.h"
#include "util.h"
#include "scan.h"
/* Lexema do identificador ou palavra reservada */
char tokenString[MAXTOKENLEN+1];
char id[MAXTOKENLEN+1];
char func[MAXTOKENLEN+1];
extern int yylex(void);
void novalinha();

int flag_coment = 0;
int num_lines = 0;
int token = 0;
%}

DIGITO [0-9]
LETRA [a-zA-Z]
OUTRO [^0-9a-zA-Z;/=\-"+""*""("")""{""[""}"">""<""!"",""\n""\t""\]"]
NEWLINE [\n]
TAB [\t]


%%
"/*"                    {flag_coment =1; while (token != -1 && token != FIM) token = yylex();}
"*/"                    {flag_coment =0; return -1;}
{DIGITO}+		            {return NUM;}
"int"                   {return INT;}
"void"                  {return VOID;}
"return"                {return RET;}
"if"                    {return IF;}
"else"                  {return ELSE;}
"while"                 {return WHI;}
{LETRA}+	              { return ID;}
({LETRA}|{DIGITO})+     {if(!flag_coment) return ERR;}
"+"				              {return SOM;}
"-"				              {return SUB;}
"*"				              {return MUL;}
"/"				              {return DIV;}
">="                    {return MAIG;}
"<="                    {return MEIG;}
"=="                    {return IGL;}
"="				              {return ATR;}
">"                     {return MAIO;}
"<"                     {return MENO;}
"!="                    {return DIF;}
","                     {return VIRG;}
";"				              {return PEV;}
"("				              {return APR;}
")"				              {return FPR;}
"{"                     {return ACH;}
"}"                     {return FCH;}
"["                     {return ACO;}
"]"                     {return FCO;}
{TAB}                   {}
{NEWLINE}               {novalinha();}
[ ]*                    {}
{OUTRO}+		            {if(!flag_coment) return ERR;}
%%

void novalinha(){
  lineno ++;
}

TokenType getToken(void)
{ static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { firstTime = FALSE;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  if(currentToken == ID) strncpy(id,yytext,MAXTOKENLEN);
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}