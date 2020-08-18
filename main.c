#include "globals.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE FALSE

/* set NO_CODE to TRUE to get a compiler that does not generate code */
#define NO_CODE TRUE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#if !NO_ANALYZE
#include "analyze.h"
#if !NO_CODE
#include "cgen.h"
#include "assembly.h"
#include "binary.h"
#endif
#endif
#endif

extern int check_return;
/* allocate global variables */
int lineno = 1;
FILE * source;
FILE * listing;
FILE * code;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = FALSE;
int Error = FALSE;

int main( int argc, char * argv[] ) {
  TreeNode * syntaxTree;
  char pgm[120]; /* nome do arquivo do código fonte */
  char path[120];
  if (argc != 2) {
    fprintf(stderr,VERMELHO"Arquivo não especificado.\n Uso: %s <nome do arquivo>\n"BRANCO,argv[0]);
    exit(1);
  }
  strcpy(path,"codigos/");
  strcpy(pgm,argv[1]) ;
  if (strchr (pgm, '.') == NULL)
     strcat(pgm,".cm");
  strcat(path,pgm);
  source = fopen(path,"r");
  if (source==NULL) {
    fprintf(stderr,VERMELHO"Arquivo %s não encontrado.\n"BRANCO,path);
    exit(1);
  }
  listing = stdout; /* send listing to screen */
  fprintf(listing,VERDE"\nCOMPILAÇÃO DO ARQUIVO C-: %s\n"BRANCO,pgm);


#if NO_PARSE
  while (getToken()!=ENDFILE);
#else
  syntaxTree = parse();
  if (TraceParse) {
    if(Error == TRUE)
      exit(-1);
    fprintf(listing,"\nÁrvore Sintática:\n");
    printTree(syntaxTree);


#if !NO_ANALYZE7 
  if (TraceAnalyze) fprintf(listing,AZUL"\nConstruindo Tabela de Simbolos...\n"BRANCO);
    buildSymtab(syntaxTree);
    if (TraceAnalyze) fprintf(listing,AZUL"\nChecando Tipos...\n"BRANCO);
    check_return = TRUE;
    typeCheck(syntaxTree);
   if (TraceAnalyze) fprintf(listing,VERDE"Compilação Concluida!\n"BRANCO); 

#if !NO_CODE
   if (!Error){
   char * codefile;
    int fnlen = strcspn(pgm,".");
    codefile = (char *) calloc(12+fnlen+4, sizeof(char));
    strcpy(codefile,"binarios/");
    strncat(codefile,pgm,fnlen);
    strcat(codefile,".inst");
    code = fopen(codefile,"w");
    if (code == NULL) {
      printf(VERMELHO"Unable to open %s\n"BRANCO,codefile);
      exit(1);
    }
    fprintf(listing,AZUL"Creating Intermediate Code...\n"BRANCO);
    codeGen(syntaxTree,codefile);                             //GERADOR DE COD. INTERMED.
    fprintf(listing,VERDE"Indermediate Code Created\n"BRANCO);
    fclose(code);
   } 
#endif
#endif
#endif
  fclose(source);
  return 0;
}
}