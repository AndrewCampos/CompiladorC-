###ESPECIFICAÇÕES PARA FUNCIONAMENTO DO COMPILADOR:

**Comandos para dar permissão aos scripts:

$ chmod +x run.sh (compilar o compilador)

$ chmod +x clean.sh (limpar arquivos de montagem)

**Executar compilador:

$ ./run.sh

$ ./compilador [diretorio do arquivo a ser testado]

$ ./clean.sh

OBSERVAÇÕES:

- int i[10]; return i; funciona e não deveria

- erro de numero de parametros que nao batem nao é tratado

- registradores de parametro para branches estão invertidos

- load antes de chamadas recursivas estão dando erro