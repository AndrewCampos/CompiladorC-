#include "globals.h"

#define NO_PARSE FALSE // Flag com valor TRUE impede a análise sintática
#define NO_ANALYZE FALSE // Flag com valor TRUE impede a análise semântica
#define NO_CODE FALSE // Flag com valor TRUE impede a geração de código intermediário

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
int init_code;
FILE *source;
FILE *listing;

FlagType TraceScan = FALSE;
FlagType TraceParse = FALSE;
FlagType TraceAnalyze = FALSE;
FlagType TraceCode = FALSE;
FlagType PrintCode = FALSE;
FlagType CreateFiles = FALSE;  // Criar arquivos de compilação
FlagType Error = FALSE;
FlagType SO;

/**
 * Verifica se o compilador está analisando um código do sistema operacional.
 * 
 * Parâmetros:
 * - argc: Parâmetro argc da chamada do programa.
 * - argv: Parâmetro argv da chamada do programa.
 * 
 * Retorna:
 * 1 caso seja uma compilação do sistema operacional, 0 caso contrário.
 */
int isSystemCompilation(int argc, char *argv[]) {

    if (strcmp(argv[2], "-so") == 0) {
        return 1;
    }

    return 0;
}

/**
 * Verifica se os argumentos passados na chamada do programa estão de acordo com o padrão
 * desejado.
 * 
 * Parâmetros:
 * - argc: Parâmetro argc da chamada do programa.
 * - argv: Parâmetro argv da cgamada do programa.
 */
void validateCallInputs(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, N_VERM "Arquivo não especificado.\nUso: %s <nome do arquivo> <flag>\n" RESET, argv[0]);
        exit(-1);
    }

    if (isSystemCompilation(argc, argv)) {
        init_code = 0;
        SO = TRUE;
    } else {
        init_code = atoi(argv[2]);
        SO = FALSE;
    }
}

/**
 * Cria a string que contém o caminho para o arquivo fonte à ser compilado.
 * 
 * Parâmetros:
 * - path: String que recebe o caminho do arquivo
 * - pgm: String recebe o nome do arquivo
 * - argv: Parâmetro argv[1] da chamada do programa
 */
void formatFilePath(char *path, char *pgm, char *argv) {
    strcpy(path, "codigos/");
    strcpy(pgm, argv);
    
    if (strchr(pgm, '.') == NULL) {
        strcat(pgm, ".cm");
    }

    strcat(path, pgm);
    source = fopen(path, "r");

    if (source == NULL) {
        fprintf(stderr, N_VERM "Arquivo %s não encontrado.\n" RESET, path);
        exit(-1);
    }
}

/**
 * Executa as etapas de análise léxica e sintática da compilação.
 */
void doParse() {
    syntaxTree = parse();
    if (Error == TRUE) {
        printf(N_VERM "Impossível concluir a compilação!\n\n");
        exit(-1);
    }
    if (TraceParse) {
        fprintf(listing, N_AZ "Árvore Sintática:\n" RESET);
        printTree(syntaxTree);
    }
}

/**
 * Executa a etapa análise semântica da compilação.
 */
void doAnalize() {
    if (TraceAnalyze) {
        fprintf(listing, AZ "Construindo Tabela de Simbolos...\n" RESET);
    }

    buildSymtab(syntaxTree);

    if (TraceAnalyze) {
        fprintf(listing, N_VERD "\nAnálise Concluida!\n" RESET);
    }

    if (Error) {
        printf(N_VERM "Impossível concluir a compilação!\n\n");
        exit(-1);
    }
}

/**
 * Executa a etapa de síntese da compilação e gera os arquivos com o resultado de todas as etapas.
 */
void doCodeGen() {
    if (TraceCode) {
        fprintf(listing, AZ "Criando código intermediário...\n" RESET);
    }

    codeGen(syntaxTree);
    generateAssembly(getIntermediate());

    if (!PrintCode) {
        listing = NULL;
    }

    generateBinary();
    listing = stdout;
    fprintf(listing, N_VERD "Compilação concluida com sucesso!\n\n" RESET);

    if (CreateFiles) {
        criaArquivos();
    } else {
        FILE *binary, *temp;
        temp = listing;
        binary = fopen(binCode, "w");
        listing = binary;
        generateBinary();
        listing = temp;
        fclose(binary);
    }
}

int main(int argc, char *argv[]) {
    char pgm[120];
    char path[120];

    validateCallInputs(argc, argv);
    formatFilePath(path, pgm, argv[1]);
    listing = stdout; 
    fprintf(listing, N_BRC "\nCOMPILAÇÃO DO ARQUIVO C-\n" RESET);
    fprintf(listing, "Fonte: " VERD "./%s\n" RESET, path);
    nomeiaArquivos(pgm);

#if NO_PARSE
    while (getToken() != ENDFILE);

#else
    doParse();

#if !NO_ANALYZE
    doAnalize();

#if !NO_CODE
    doCodeGen();

#endif
#endif
#endif

    fclose(source);
    return 0;
}