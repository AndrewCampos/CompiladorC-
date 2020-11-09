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

FlagType TraceScan = FALSE; // Imprimir tokens
FlagType TraceParse = FALSE; // Imprimir árvore sintática
FlagType TraceAnalyze = FALSE; // Imprimir tabela de simbolos
FlagType TraceCode = FALSE; // Imprimir nós da geração de código
FlagType PrintCode = FALSE; // Imprimir os códigos gerados
FlagType CreateFiles = FALSE; // Criar arquivos de compilação
FlagType Error = FALSE; // Flag que marca a existência de erros

int main( int argc, char * argv[] ) {
  char pgm[120]; /* nome do arquivo do código fonte */
  char path[120];
  if (argc != 2) {
    fprintf(stderr,N_VERM"Arquivo não especificado.\n Uso: %s <nome do arquivo>\n"RESET,argv[0]);
    exit(-1);
  }
  strcpy(path,"codigos/");
  strcpy(pgm,argv[1]) ;
  if (strchr (pgm, '.') == NULL)
     strcat(pgm,".cm");
  strcat(path,pgm);
  source = fopen(path,"r");
  if (source==NULL) {
    fprintf(stderr,N_VERM"Arquivo %s não encontrado.\n"RESET,path);
    exit(-1);
  }
  listing = stdout; /* send listing to screen */
  fprintf(listing,N_BRC"\nCOMPILAÇÃO DO ARQUIVO C-\n"RESET);
  fprintf(listing,"Fonte: "VERD"./%s\n"RESET,path);
  nomeiaArquivos(pgm);

#if NO_PARSE
  while (getToken()!=ENDFILE);
#else
  syntaxTree = parse();
  if(Error == TRUE){
    printf(N_VERM "Impossível concluir a compilação!\n\n");
    exit(-1);
    }
  if (TraceParse) {
    fprintf(listing,N_AZ"Árvore Sintática:\n"RESET);
    printTree(syntaxTree);
  }

#if !NO_ANALYZE
  if (TraceAnalyze) fprintf(listing,AZ"Construindo Tabela de Simbolos...\n"RESET);
  buildSymtab(syntaxTree);
  if (TraceAnalyze) fprintf(listing,N_VERD"\nAnálise Concluida!\n"RESET); 
  if(Error){
    printf(N_VERM "Impossível concluir a compilação!\n\n");
    exit(-1);
  }
#if !NO_CODE
  if(TraceCode) fprintf(listing,AZ"Criando código intermediário...\n"RESET);
  codeGen(syntaxTree);  //GERADOR DE COD. INTERMED.
  generateAssembly(getIntermediate());  // GERADOR DE COD. ASSEMBLY
  if(!PrintCode) listing = NULL;
  generateBinary();  // GERADOR DE COD. BINÁRIO
  listing = stdout;
  fprintf(listing, N_VERD "Compilação concluida com sucesso!\n\n" RESET);
  // Cria os arquivos de compilação
  if(CreateFiles) makeFiles();
  else{
    FILE *binary, *temp;
    temp = listing;
    binary = fopen(binCode,"w");
    listing = binary;
    generateBinary();
    listing = temp;
    fclose(binary);
  }


#endif
#endif
#endif
  fclose(source);
  return 0;
}