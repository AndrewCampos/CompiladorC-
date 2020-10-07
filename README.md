ESPECIFICAÇÕES PARA FUNCIONAMENTO DO COMPILADOR:

**Comandos para dar permissão aos scripts:

$ chmod +x run.sh (compilar o compilador)

$ chmod +x clean.sh (limpar arquivos de montagem)

**Executar compilador:

$ ./run.sh

$ ./compilador [diretorio do arquivo a ser testado]

$ ./clean.sh

ALTERAÇÕES QUE PRECISAM SER FEITAS:

1. Gerar código assembly

1. Pensar na alocação de variáveis (principalmente vetor) quando são parâmetros

1. Pensar na manipulação de vetores com variavel como indice

1. Gerar código binário

1. Rever necessidade do registrador $io

1. Fazer uso das instruções com imediato

1. Colcoar flag para loads desnecessarios

OBSERVAÇÕES:

- int i[10]; return i; funciona e não deveria

- erro de numero de parametros que nao batem nao é tratado