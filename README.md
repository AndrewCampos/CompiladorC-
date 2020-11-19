# Compilador para a Linguagem C-

Este reposítório se trata de um projeto da disciplina Laboratório de Compiladores da UNIFESP SJC no ano de 2020, onde a proposta foi desenvolver a um compilador para a linguagem C- proposta no livro Compiladores: Princípios e Práticas de Kenneth Louden. O compilador, além de fazer toda análise do código C-, gera um código binário baseado em um [processador de 32 bits](https://github.com/AndrewCampos/Processador-AOC), feito anteriormente durante o curso, e arquivos de texto com todos os processos de compilação sendo eles a **árvore sintática**, a **tabela de símbolos**, o **código intermediário** e o **código assembly**.

Para informações mais detalhadas sobre a implementação do compilador confira o relaório presente na pasta **Relatório**.

## Sobre o Projeto

O projeto do compilador foi baseado nas premissas de Kenneth Louden em Compiladores: Princípios e Práticas e foi majoritáriamente feito na linguagem C, utilizando algumas ferramentas para auxiliar na criação de algumas partes.

A primeira ferramenta foi o compilador **Flex**, que foi utilizado para facilitar na criação de um _Scaner_ sem ter a necessidade de programar linha por linha do mesmo, e sim inserir Expressões Regulares para a aceitação de cada token.

A segunda, foi o compilador **YACC Bison**, que foi utilizado em conjunto de uma Gramática Livre de Contexto para gerar um _Parser_. Essa ferramenta tem uma vantagem que é o fato de ser própria para a utilização em conjunto com o Flex, fazendo a criação de uma rotina de análise se tornar mais simples.

Ambas ferramentas depois de compiladas geram códigos C que podem ser facilmente integrados com as outas partes do projeto que foram desenvolvidas na mesma linguagem.

## Divisão do Projeto

O projeto foi divido macroscopicamente em duas partes: a Análise e a Síntese, sendo cada uma delas subdivididas em mais 3 partes.

### Análise

A fase de análise é a responsável por percorrer todo o código e verificar sua integridade e a correspondência com a linguagem proposta, no caso C-. Ela foi dividida em análise léxica**¹** (_scaner.l_), análise sintática**²** (_parser.y_) e análise semântica**³** (_analyze.c_ e _symtab.c_). A função de _1_ é percorrer todo o código fonte analisando se cada palavra escrita se encaixa no escopo da linguagem C-, gerando _tokens_ que são passados para _2_ que os verifica em conjunto e analisa se a estrutura do código está correta, gerando uma árvore sintática. Essa última por fim é repassada para _3_ que procura por erros de contexto como variáveis não declaradas ou atribuições de tipos conflitantes e caso nenhum seja encontrado uma tabela pe gerada constando todos os itens presentes no código, bem como a posição de memória a ser alocada (respeitando as restrições do processador) e outras informações presentes na árvore sintática. 

**Exemplo de Tabela de Símbolos**
```
                               Tabela de Simbolos:
---------------------------------------------------------------------------------
Nome           Escopo  Tipo ID  Tipo Retorno  Tipo Param  Mem. Loc.  Num da linha
-------------  ------  -------  ------------  ----------  ---------  ------------
main           global  fun      VOID          VOID           -        10; 
input          main    call     VOID          null           -        13;  14; 
return         gcd     ret      INT           null           -         6; 
return         gcd     ret      INT           null           -         3; 
output         main    call     null          null           -        15; 
u              gcd     var      INT           null           1         1;   3;   6;   6; 
v              gcd     var      INT           null           2         1;   2;   6;   6;   6; 
x              main    var      INT           null           1        11;  13;  15; 
y              main    var      INT           null           2        12;  14;  15; 
gcd            gcd     call     INT           null           -         6;  15; 
gcd            global  fun      INT           INT            -         1; 
```

### Síntese

A fase de síntese é executada apenas se a fase de análise foi executada completamente sem encontrar nenhum erro. Ela é responsável por gerar os códigos à partir do código fonte e é dividido em: Código Intermediário**¹** (_cgen.c_), Código Assembly**²** (_assembly.c_) e Código Binário**³** (_binary.c_). _1_ é um código gerado pelo percorrimento da árvore sintática formado por quadruplas de três endereços, ou seja, todo o código é trazido para um nível mais baixo contendo uma _label_ que referencia qual operação será feita e até três operandos informando onde o dado final será guardado e de onde os dados são retirados para que a operação possa ser feita. Em seguida cada quádrupla é convertida em um ou mais comandos assembly para gerar _2_ e o mesmo pode ser convertido diretamente para _3_ por estar no nível mais próximo possivel de comandos binários para uma linguagem.

### Conexão

Além dos arquivos citados acima, também existem mais arquivos para que a conexão entre eles possa ser feita. Intuitivamente exisstem arquivos de cabeçalho _.h_ que contém estruturas utilizadas em seus respectivos códigos, variáveis globais e funções que podem ser acessadas por outros arquivos. Também existem o cabeçalho _globals.h_ que se trata do cabeçalho global com estruturas que e variáveis que são compartilhados por todo o projeto. Por fim existem os arquivos de utilidades, sendo eles o arquivo _main.c_ que é o que contém a rotina de união do compilador e o arquivo _util.c_ que se trata de rotinas úteis que podem ser usadas por todos os módulos.

### ESPECIFICAÇÕES PARA FUNCIONAMENTO DO COMPILADOR:

**Comandos para dar permissão aos scripts:
```
$ chmod +x run.sh (compilar o compilador)

$ chmod +x clean.sh (limpar arquivos de montagem)
```

**Executar compilador:
```
$ ./run.sh

$ ./compilador [diretorio do arquivo a ser testado]

$ ./clean.sh
```

### OBSERVAÇÕES

Apesar já estar com sua funcionalidade completa ainda existem alguns ponto que precisam ser refinados, sendo eles:
- Uso de vetores sem índice não é tratado como erro
Ex:
```
C
int i[10]; 
return i;
```
- Funções com número de parametros errados não retornam erro
Ex:
```
C
void func(int a){ ... }

func(a,b);
```
- Instruções de _branch_ são geradas com os registradores de parâmentro invertidos
- Ao conter variáveis e chamadas de funções em uma atribuição a ordem dos _loads_ poem impedir recursão