# Tradutor Assembly-Binário

Tradutor de código assembly para binário feito com a intenção de auxiliar no desenvolvimento do sistema operacional para o processador do projeto. O sistema operacional foi escrito majoritáriamente em C- e teve algumas alterações feitas diretamente no código assembly compilado, sendo necessário um tradutor para converter esse conjunto de instruções em código binário.

**Exemplo de uso:**
```
$ python3 asc_translator.py
$ File name: SO.asc
$ Translating assembly code from source: SO.asc
$ Assembly code successfully translated!
```

**OBS.:** O código assembly a ser compilado precisa estar presente na pasta "Gerados".

