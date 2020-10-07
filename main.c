#include "globals.h"

/* Flag com valor TRUE impede a análise sintática */
#define NO_PARSE FALSE
/* Flag com valor TRUE impede a análise semântica */
#define NO_ANALYZE FALSE
/* Flag com valor TRUE impede a geração de código intermediário */
#define NO_CODE FALSE

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
int lineno = 1;
FILE * source;
FILE * listing;
FILE * code;

int TraceScan = FALSE; // Imprimir tokens
int TraceParse = TRUE; // Imprimir árvore sintática
int TraceAnalyze = TRUE; // Imprimir tabela de simbolos
int TraceCode = FALSE; // Imprimir nós da geração de código
int CreateFiles = FALSE; // Criar arquivos de compilação
int Error = FALSE; // Flag que marca a existência de erros

int main( int argc, char * argv[] ) {
  char pgm[120]; /* nome do arquivo do código fonte */
  char path[120];
  if (argc != 2) {
    fprintf(stderr,N_VERM"Arquivo não especificado.\n Uso: %s <nome do arquivo>\n"RESET,argv[0]);
    exit(1);
  }
  strcpy(path,"codigos/");
  strcpy(pgm,argv[1]) ;
  if (strchr (pgm, '.') == NULL)
     strcat(pgm,".cm");
  strcat(path,pgm);
  source = fopen(path,"r");
  if (source==NULL) {
    fprintf(stderr,N_VERM"Arquivo %s não encontrado.\n"RESET,path);
    exit(1);
  }
  listing = stdout; /* send listing to screen */
  fprintf(listing,N_BRC"\nCOMPILAÇÃO DO ARQUIVO C-\n"RESET);
  fprintf(listing,"Fonte: "VERD"./%s\n"RESET,path);
  nomeiaArquivos(pgm);

#if NO_PARSE
  while (getToken()!=ENDFILE);
#else
  syntaxTree = parse();
  if(Error == TRUE) exit(-1);
  if (TraceParse) {
    fprintf(listing,N_AZ"Árvore Sintática:\n"RESET);
    printTree(syntaxTree);
  }

#if !NO_ANALYZE
  if (TraceAnalyze) fprintf(listing,AZ"Construindo Tabela de Simbolos...\n"RESET);
  buildSymtab(syntaxTree);
  if (TraceAnalyze) fprintf(listing,N_VERD"\nAnálise Concluida!\n"RESET); 

#if !NO_CODE
  if (!Error){
  char * codefile;
    int fnlen = strcspn(pgm,".");
    codefile = (char *) calloc(12+fnlen+5, sizeof(char));
    strcpy(codefile,"binarios/");
    strncat(codefile,pgm,fnlen);
    strcat(codefile,".inst");
    code = fopen(codefile,"w");
    if (code == NULL) {
      printf(N_VERM"Não foi possível abrir o arquivo '%s'!\n"RESET,codefile);
      exit(1);
    }
    if(TraceCode) fprintf(listing,AZ"Criando código intermediário...\n"RESET);
    codeGen(syntaxTree);  //GERADOR DE COD. INTERMED.
    fclose(code);
    generateAssembly(getIntermediate());  // GERADOR DE COD. ASSEMBLY
    generateBinary();  // GERADOR DE COD. BINÁRIO
    if(CreateFiles) makeFiles();
  } 

#endif
#endif
#endif
  fclose(source);
  return 0;
}