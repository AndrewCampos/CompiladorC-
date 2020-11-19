# Compilador para a Linguagem C-

Este reposítório se trata de um projeto da disciplina Laboratório de Compiladores da UNIFESP SJC no ano de 2020, onde a proposta foi desenvolver a um compilador para a linguagem C- proposta no livro Compiladores: Princípios e Práticas de Kenneth Louden. O compilador, além de fazer toda análise do código C-, gera um código binário baseado em um [processador de 32 bits](https://github.com/AndrewCampos/Processador-AOC), feito anteriormente durante o curso, e arquivos de texto com todos os processos de compilação sendo eles a **árvore sintática**, a **tabela de símbolos**, o **código intermediário** e o **código assembly**.

## 

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

### OBSERVAÇÕES:

- int i[10]; return i; funciona e não deveria

- erro de numero de parametros que nao batem nao é tratado

- registradores de parametro para branches estão invertidos

- load antes de chamadas recursivas estão dando erro