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
FlagType CreateFiles = FALSE; // Criar arquivos de compilação
FlagType Error = FALSE;
FlagType SO;

/**
 * @brief Verifica se o compilador está analisando um código do sistema operacional.
 * 
 * @param argc: Parâmetro argc da chamada do programa.
 * @param argv: Parâmetro argv da chamada do programa.
 * 
 * @return 1 caso seja uma compilação do sistema operacional, 0 caso contrário.
 */
int isSystemCompilation(int argc, char *argv[]) {

    if (strcmp(argv[2], "-so") == 0) {
        return 1;
    }

    return 0;
}

/**
 * @brief Verifica se os argumentos passados na chamada do programa estão de acordo com o padrão
 * desejado.
 * 
 * @param argc: Parâmetro argc da chamada do programa.
 * @param argv: Parâmetro argv da cgamada do programa.
 */
void validateCallInputs(int argc, char *argv[]) {
    if (argc < 3) {
        printError("Arquivo não especificado.\nUso: ./compilador <nome do arquivo> <flag>", Beginning, 0);
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
 * @brief Cria a string que contém o caminho para o arquivo fonte à ser compilado.
 * 
 * @param path: String que recebe o caminho do arquivo
 * @param pgm: String recebe o nome do arquivo
 * @param argv: Parâmetro argv[1] da chamada do programa
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
        printError("Arquivo não encontrado.", Beginning, 0);
        exit(-1);
    }
}

/**
 * @brief Executa as etapas de análise léxica e sintática da compilação.
 */
void doParse() {
    syntaxTree = parse();
    if (Error == TRUE) {
        printError("Impossível concluir a compilação!\n", CodeGen, 0);
        exit(-1);
    }

    printSucess("Análise léxica concluída.", HeaderOnly);

    if (TraceParse) {
        printf(N_AZ "Árvore Sintática:\n" RESET);
        printTree(syntaxTree);
    }
    printSucess("Árvore sintática construída.", HeaderOnly);
}

/**
 * @brief Executa a etapa análise semântica da compilação.
 */
void doAnalize() {
    buildSymtab(syntaxTree);

    if (Error) {
        printError("Impossível concluir a compilação!\n", CodeGen, 0);
        exit(-1);
    }
}

/**
 * @brief Executa a etapa de síntese da compilação e gera os arquivos com o resultado de todas as etapas.
 */
void doCodeGen() {
    codeGen(syntaxTree);
    printSucess("Código intermediário criado.", HeaderOnly);
    generateAssembly(getIntermediate());
    printSucess("Código assembly criado.", HeaderOnly);

    if (!PrintCode) {
        listing = NULL;
    }

    generateBinary();
    printSucess("Código binário criado.", HeaderOnly);
    listing = stdout;

    if (CreateFiles) {
        makeFiles();
    } else {
        FILE *binary, *assembly, *temp;
        temp = listing;
        assembly = fopen(assCode, "w");
        listing = assembly;
        printAssembly();
        binary = fopen(binCode, "w");
        listing = binary;
        generateBinary();
        listing = temp;
        fclose(binary);
        printWarning("Não foram criados arquivos dos processos intermediários.", 0);
    }
}

int main(int argc, char *argv[]) {
    char pgm[120];
    char path[120];

    validateCallInputs(argc, argv);
    formatFilePath(path, pgm, argv[1]);
    listing = stdout; 
    printf(N_BRC "\nCOMPILAÇÃO DO ARQUIVO C-\n" RESET "Fonte: " VERD "./%s\n" RESET, path);
    nameFiles(pgm);

#if NO_PARSE
    while (getToken() != ENDFILE);
    printSucess("Análise léxica concluída.", HeaderOnly);
    printWarning("Não foram feitas as demais etapas da compilação.", 0)


#else
    doParse();

#if !NO_ANALYZE
    doAnalize();
    printSucess("Etapa de análise concluída.", HeaderOnly);

#if !NO_CODE
    doCodeGen();
    printSucess("Compilação concluida com sucesso!\n", FullMessage);

#endif
#endif
#endif

    fclose(source);
    return 0;
}