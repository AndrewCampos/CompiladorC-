#include "globals.h"
#include "util.h"
#include "cgen.h"
#include "symtab.h"
#include "assembly.h"
#include "binary.h"

#define INDENT indentno += 2
#define UNINDENT indentno -= 2

static int indentno = 0; // A variável indentno é usada pelo printTree para armazenar o número atual de espaços para identação.


void printToken(TokenType token, const char *tokenString) {
    switch (token) {
    case IF:
        fprintf(listing, "%s\n", tokenString);
        break;
    case RET:
        fprintf(listing, "%s\n", tokenString);
        break;
    case ELSE:
        fprintf(listing, "%s\n", tokenString);
        break;
    case INT:
        fprintf(listing, "%s\n", tokenString);
        break;
    case WHI:
        fprintf(listing, "%s\n", tokenString);
        break;
    case VOID:
        fprintf(listing, "%s\n", tokenString);
        break;
    case IGL:
        fprintf(listing, "==\n");
        break;
    case ATR:
        fprintf(listing, "=\n");
        break;
    case DIF:
        fprintf(listing, "!=\n");
        break;
    case MENO:
        fprintf(listing, "<\n");
        break;
    case MAIO:
        fprintf(listing, ">\n");
        break;
    case MEIG:
        fprintf(listing, "<=\n");
        break;
    case MAIG:
        fprintf(listing, ">=\n");
        break;
    case APR:
        fprintf(listing, "(\n");
        break;
    case FPR:
        fprintf(listing, ")\n");
        break;
    case ACO:
        fprintf(listing, "[\n");
        break;
    case FCO:
        fprintf(listing, "]\n");
        break;
    case ACH:
        fprintf(listing, "{\n");
        break;
    case FCH:
        fprintf(listing, "}\n");
        break;
    case PEV:
        fprintf(listing, ";\n");
        break;
    case VIRG:
        fprintf(listing, ",\n");
        break;
    case SOM:
        fprintf(listing, "+\n");
        break;
    case SUB:
        fprintf(listing, "-\n");
        break;
    case MUL:
        fprintf(listing, "*\n");
        break;
    case DIV:
        fprintf(listing, "/\n");
        break;
    case FIM:
        fprintf(listing, "EOF\n");
        break;
    case NUM:
        fprintf(listing, "NUM, val = %s\n", tokenString);
        break;
    case ID:
        fprintf(listing, "ID, nome = %s\n", tokenString);
        break;
    case ERR:
        fprintf(listing,
                "%s\n", tokenString);
        break;
    default:
        fprintf(listing, "Token Desconhecido: %d\n", token);
    }
}

TreeNode *newStmtNode(StmtKind kind) {
    TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
    int i;
    if (t == NULL) {
        fprintf(listing, "Out of memory error at line %d\n", lineno);
    } else {
        for (i = 0; i < MAXCHILDREN; i++)
            t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = StmtK;
        t->kind.stmt = kind;
        t->lineno = lineno;
    }
    return t;
}

TreeNode *newExpNode(ExpKind kind) {
    TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
    int i;
    if (t == NULL) {
        fprintf(listing, "Out of memory error at line %d\n", lineno);
    } else {
        for (i = 0; i < MAXCHILDREN; i++)
            t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = ExpK;
        t->kind.exp = kind;
        t->lineno = lineno;
        t->type = Void;
    }
    return t;
}

char *copyString(char *string) {
    char *copy;

    if (string == NULL) {
        return NULL;
    }
    
    copy = malloc(strlen(string) + 1);

    if (copy == NULL) {
        printError("Erro na alocação da string.", CodeGen, lineno);
    } else {
        strcpy(copy, string);
    }

    return copy;
}

char *composeString(char *string1, char *string2) {
    char *newString = malloc(strlen(string1) + 1);
    sprintf(newString, string1, string2);
    return newString;
}

/**
 * Imprime espaços  no arquivo apontado por "listing" para realizar a identação do código.
 */
static void printSpaces(void) {
    int i;
    for (i = 0; i < indentno; i++) {
        fprintf(listing, " ");
    }
}

void printTree(TreeNode *tree) {
    int i;
    INDENT;
    while (tree != NULL) {
        printSpaces();
        if (tree->nodekind == StmtK) {
            switch (tree->kind.stmt) {
            case IfK:
                fprintf(listing, "If\n");
                break;
            case WhileK:
                fprintf(listing, "While\n");
                break;
            case AssignK:
                fprintf(listing, "Assign: \n");
                break;
            case ReturnINT:
                fprintf(listing, "Return\n");
                break;
            case ReturnVOID:
                fprintf(listing, "Return\n");
                break;
            default:
                fprintf(listing, "Unknown StmtNode kind\n");
                break;
            }
        } else if (tree->nodekind == ExpK) {
            switch (tree->kind.exp) {
            case OpK:
                fprintf(listing, "Op: ");
                printToken(tree->attr.op, "\0");
                break;
            case ConstK:
                fprintf(listing, "Const: %d\n", tree->attr.val);
                break;
            case IdK:
                fprintf(listing, "Id: %s\n", tree->attr.name);
                break;
            case VarDeclK:
                fprintf(listing, "Var: %s\n", tree->attr.name);
                break;
            case FunDeclK:
                fprintf(listing, "Func: %s\n", tree->attr.name);
                break;
            case AtivK:
                fprintf(listing, "Chamada da Função: %s\n", tree->attr.name);
                break;
            case TypeK:
                fprintf(listing, "Tipo: %s\n", tree->attr.name);
                break;
            case VarParamK:
                fprintf(listing, "Parametro: %s\n", tree->attr.name);
                break;
            case VetParamK:
                fprintf(listing, "Parametro: %s\n", tree->attr.name);
                break;
            case VetorK:
                fprintf(listing, "Vetor: %s", tree->attr.name);
                break;
            default:
                fprintf(listing, "Unknown ExpNode kind: %d\n", tree->nodekind);
                break;
            }
        } else {
            fprintf(listing, "Unknown Node kind\n");
        }
        
        for (i = 0; i < MAXCHILDREN; i++) {
            printTree(tree->child[i]);
        }
        tree = tree->sibling;
    }
    UNINDENT;
}

void nameFiles(char *nome) {
    int fnlen = strcspn(nome, ".");
    // Aloca espaços para nomes
    ArvSint = (char *)calloc(8 + fnlen + 5, sizeof(char));
    TabSimb = (char *)calloc(8 + fnlen + 4, sizeof(char));
    interCode = (char *)calloc(8 + fnlen + 4, sizeof(char));
    assCode = (char *)calloc(8 + fnlen + 5, sizeof(char));
    binCode = (char *)calloc(8 + fnlen + 5, sizeof(char));
    // Insere pasta '/gerados' ao caminho
    strcpy(ArvSint, "gerados/");
    strcpy(TabSimb, "gerados/");
    strcpy(interCode, "gerados/");
    strcpy(assCode, "gerados/");
    strcpy(binCode, "gerados/");
    // Insere nome do arquivo ao caminho
    strncat(ArvSint, nome, fnlen);
    strncat(TabSimb, nome, fnlen);
    strncat(interCode, nome, fnlen);
    strncat(assCode, nome, fnlen);
    strncat(binCode, nome, fnlen);
    // Insere extensão do arquivo
    strcat(ArvSint, ".tree");
    strcat(TabSimb, ".tab");
    strcat(interCode, ".itm");
    strcat(assCode, ".asb");
    strcat(binCode, ".bin");
}

void makeFiles() {
    int i;
    FILE *arvore, *tabela, *intermed, *temp, *assembly, *binary;
    arvore = fopen(ArvSint, "w");
    tabela = fopen(TabSimb, "w");
    intermed = fopen(interCode, "w");
    assembly = fopen(assCode, "w");
    binary = fopen(binCode, "w");
    temp = listing;
    listing = arvore;
    printTree(syntaxTree);
    listing = tabela;
    printSymTab(listing);
    listing = intermed;
    printCode(getIntermediate());
    listing = assembly;
    printAssembly();
    listing = binary;
    generateBinary();
    listing = temp;
    fclose(arvore);
    fclose(tabela);
    fclose(intermed);
    fclose(assembly);
    fclose(binary);
}

void printError(char *message, CompilationStep step, int line) {
    char *stepString[] = {"Léxico", "Sintático", "Semântico"};

    if (step < 3) {
        printf(N_VERM "[ERRO]    " RESET "Erro %s: %s (%d)\n", stepString[step], message, line);
    } else  if (step == Beginning) {
        printf(N_VERM "%s\n" RESET, message);
    } else {
        printf(N_VERM "[ERRO]    " RESET  "%s\n", message);
    }
}

void printWarning(char *message, int line) {
    if (line > 0) {
        printf(N_AMAR "[AVISO]   " RESET "%s (%d)\n", message, line);
    } else {
        printf(N_AMAR "[AVISO]   " RESET "%s\n", message);
    }
}

void printSucess(char *message, PrintFlag flag) {
    switch (flag) {
    case FullMessage:
        printf(N_VERD "%s\n" RESET, message);
        break;
    
    case HeaderOnly:
        printf(N_VERD "[SUCESSO] " RESET "%s\n", message);
        break;

    default:
        break;
    }
}

int isReservedFunction(TreeNode *t) {
    return (strcmp(t->attr.name, "input") == 0)
        || (strcmp(t->attr.name, "output") == 0)
        || (strcmp(t->attr.name, "load") == 0)
        || (strcmp(t->attr.name, "store") == 0)
        || (strcmp(t->attr.name, "loadStack") == 0)
        || (strcmp(t->attr.name, "saveStack") == 0)
        || (strcmp(t->attr.name, "nop") == 0);
}
