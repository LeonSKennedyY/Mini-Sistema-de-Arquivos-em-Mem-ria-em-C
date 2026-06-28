# Mini-Sistema de Arquivos em Memória em C

Universidade do Vale do Itajaí    
Nomes:   
    Gabriel Tenfen    
    Leonardo Alberto da Silva    
    Mariana Leal    
Disciplina: Sistemas Operacionais    
Atividade M3    
TRABALHO AVALIATIVO:     
Implementação e Análise de um Mini-Sistema de Arquivos em Memória em C    


## Descrição

Este projeto implementa um mini-sistema de arquivos em memória utilizando a linguagem C. O simulador representa diretórios, arquivos, permissões, FCBs, inodes simulados e alocação de blocos de forma interna, sem usar diretamente o sistema de arquivos real do Linux para armazenar os arquivos simulados.


## Metodologia e Ferramentas

O código foi desenvolvido em C, usando o Windows Subsystem for Linux (WSL) no Visual Studio Code.

ferramentas de compilação:    
    Compilador GCC (GNU Compiler Collection) para compilação do código-fonte.    
    Visual Studio Code (VS Code) como ambiente de desenvolvimento principal.    
    Extensão WSL do VS Code para integração direta com o ambiente Linux.    
    Terminal Linux do Ubuntu/WSL para compilação e execução do simulador.    

## Como compilar / Executar

No terminal do WSL, dentro da pasta do projeto:

Compilar    
    gcc main.c sistemaArquivos.c -o simulador

Executar    
    ./simulador


## Funcionalidades

O sistema possui um menu interativo com operações semelhantes aos comandos Linux:

|ls:    | lista arquivos e diretórios simulados.                                    |    
|mkdir: | cria diretórios.                                                          |    
|cd:    | navega entre diretórios.                                                  |    
|pwd:   | mostra o diretório atual.                                                 |    
|touch: | cria arquivos.                                                            |    
|echo:  | escreve conteúdo em um arquivo.                                           |    
|cat:   | lê o conteúdo de um arquivo.                                              |    
|chmod: | altera permissões RWX.                                                    |    
|rm:    | remove arquivos.                                                          |    
|cp:    | copia arquivos.                                                           |    
|mv:    | move ou renomeia arquivos.                                                |    
|mostrar blocos: | exibe o estado do disco simulado.                                |    
|stat:      | exibe todos os atributos armazenados no FCB do arquivo.               |    
|truncate:  | remove todo o conteúdo de um arquivo sem excluí-lo.                   |    
|seek:      | reposiciona a leitura para uma posição específica dentro do arquivo.  |    

    Comparação com comandos Linux reais
|Simulador	    Linux real
| -------------| -------------------------- |
|    ls	       |  ls -l                     |
|    mkdir	   |  mkdir nome                |
|    cd	       |  cd nome                   |
|    touch	   |  touch arquivo             |
|    echo	   |  echo "texto" > arquivo    |
|    cat	   |      cat arquivo           |
|    chmod	   |  chmod 644 arquivo         |
|    rm	       |  rm arquivo                |
|    cp	       |  cp origem destino         |
|    mv	       |  mv origem destino         |
|mostrar blocos|	Não possui equivalente direto (função didática do simulador)    |
|    stat	   | stat arquivo               |
|    truncate  |	truncate -s 0 arquivo   |
|    seek	   |   lseek() (chamada de sistema) ou reposicionamento usando fseek() em programas C   |


## Conceitos implementados

Arquivos e atributos
    Cada arquivo possui atributos semelhantes aos de um sistema real, como nome, tamanho, tipo, datas, permissões e inode.

File Control Block e inode
    O FCB armazena os metadados do arquivo. O campo inode é um identificador único gerado automaticamente para cada arquivo criado.

Diretórios em árvore
    Os diretórios são representados por uma estrutura em árvore. Cada diretório pode ter subdiretórios e arquivos, permitindo organização hierárquica semelhante ao Linux.

Permissões RWX
    O sistema implementa permissões no formato proprietário, grupo e outros. As permissões são armazenadas em formato numérico, como 644 ou 755, e verificadas antes de operações de leitura, escrita e execução.

Alocação de blocos
    O sistema possui um disco simulado representado por um vetor de caracteres. Os arquivos são divididos em blocos de tamanho fixo, e o FCB guarda o bloco inicial e a quantidade de blocos utilizados.

## Estruturas de dados utilizadas

O sistema utiliza structs para representar diretórios e arquivos.

A struct Diretorio representa a árvore de diretórios. Cada diretório possui nome, ponteiro para o diretório pai, lista de subdiretórios e lista de arquivos.

A struct FCB representa o File Control Block de cada arquivo. Ela armazena nome, tamanho, tipo, datas de criação, modificação e acesso, permissões, inode simulado e informações dos blocos ocupados no disco simulado.

## Objetivos

Arquivo como tipo abstrato com atributos.
    | typedef struct FCB
    | Atende: Nome, Identificador (inode), Tipo, Tamanho, Proteção, Datas

Estrutura de diretórios (Estrutura hierárquica).
    | typedef struct Diretorio
    | operações: mkdir, cd, pwd, ls

Operações básicas com arquivos
    | Operação	    |Implementada   |
    | ------------- | -------       |
    |Criar	        | touch         |
    |Escrever	    | echo          |
    |Ler 	        | cat           |
    |Excluir	    | rm            |
    |Copiar	        | cp            |
    |Mover	        | mv            |
    |Reposicionar   | seek          |
    |Truncar        | truncate      |

Controle de acesso -- gerenciamento de permissões.  
    possui:     
        chmodSimulado()
        temPermissao()
    verifica:   
        cat, echo, rm, cp, mv

Mapeamento lógico → físico. 
    char disco[TAM_DISCO];  
        blocoInicial
        qtdBlocos
    Arquivo lógico → Blocos físicos simulados.

Reposicionar.
    criar, escrever, ler, reposicionar, excluir e truncar.  
        void seekSimulado(char *nome, int posicao)
    reposicionamento do ponteiro de leitura, equivalente ao uso de: fseek() ou lseek(). 

Truncar.    
    void truncateSimulado(char *nome)   
    Isso corresponde exatamente ao comportamento conceitual de: truncate -s 0 arquivo.txt   

## 3.Objetivos Específicos

3.1. Modelagem da Estrutura de Diretórios:

A estrutura de diretórios está modelada assim:
    typedef struct Diretorio
Representa uma árvore de diretórios, porque cada diretório possui:
    pai: aponta para o diretório anterior/superior;
    subdirs: aponta para seus subdiretórios;
    prox: permite encadear vários diretórios no mesmo nível;
    arquivos: lista os arquivos dentro daquele diretório.
Também possui criação de diretórios: void mkdirSimulado(char *nome)
Navegação entre diretórios: void cdSimulado(char *nome)

| Requisito                                 | Atende? |
| ----------------------------------------- | ------- |
| Estrutura de dados em árvore              |   Sim   |
| Uso de ponteiros para pai e subdiretórios |   Sim   |
| Criação de diretórios com `mkdir`         |   Sim   |
| Navegação com `cd`                        |   Sim   |
| Retorno ao diretório pai com `cd ..`      |   Sim   |


3.2. Representação e Gerenciamento de Arquivos e Metadados:

Criado a estrutura FCB dentro de sistemaArquivos.h
Também implementado as operações basicas, listadas em Objetivos
Ponto importante, as operações são feitas na estrutura simulada, usando FCB, Diretorio, disco[] e blocosUsados[], não no sistema de arquivos real do Linux.


3.3. Controle de Acesso e Permissões:

|Requisito	                                    | Atende?
| ----------------------------------------------| -------
|Permissões RWX	                                | Sim
|Owner, group e public	                        | Sim, pela variável usuarioAtual
|Bitmask / operadores bit a bit	                | Sim, em temPermissao()
|chmod numérico	                                | Sim, em chmodSimulado()
|Exibir permissões	                            | Sim, em imprimirPermissoes() e lsSimulado()
|Verificar permissão ao ler	                    | Sim, em catSimulado()
|Verificar permissão ao escrever	            | Sim, em echoSimulado()
|Verificar permissão ao remover/mover/copiar	| Sim
|Mensagem de permissão negada	                | Sim


3.4. Simulação de Alocação de Blocos

código possui o disco simulado:     
    char disco[TAM_DISCO];  
    int blocosUsados[MAX_BLOCOS];   
Isso representa:        
    disco[]: área de armazenamento simulada;    
    blocosUsados[]: controle dos blocos livres e ocupados.  

Também possui alocação contínua simplificada:   
    int alocarBlocos(int tamanho)

O FCB referencia os blocos por meio destes campos:  
    int blocoInicial;   
    int qtdBlocos;          
        blocoInicial: onde o arquivo começa no disco simulado;  
        qtdBlocos: quantos blocos o arquivo ocupa.  

## Tratamento de Erros:

|Requisito	              | Atende?
| ----------------------- | -------
|Arquivo não encontrado	  |  Sim
|Diretório não encontrado |	 Sim
|Arquivo já existente	  |  Sim
|Diretório já existente	  |  Sim
|Permissão negada	      |  Sim
|Disco cheio	          |  Sim
|Nome inválido	          |  Sim


## Exemplos

Criando um diretório

Simulador.
    Opcao: 2
    Nome do diretorio: documentos
Linux.
    mkdir documentos
    Navegando entre diretórios

Simulador.
    Opcao: 3
    Diretorio: documentos
Linux.
    cd documentos
    Criando um arquivo

Simulador.
    Opcao: 5
    Nome do arquivo: teste.txt
Linux.
    touch teste.txt
    Escrevendo conteúdo em um arquivo

Simulador.
    Opcao: 6
    Arquivo: teste.txt
    Conteudo: Ola Mundo
Linux.
    echo "Ola Mundo" > teste.txt
    Lendo o conteúdo de um arquivo

Simulador.
    Opcao: 7
    Arquivo: teste.txt
    Saída: Ola Mundo
Linux.
    cat teste.txt
    Alterando permissões

Simulador.
    Opcao: 8
    Arquivo: teste.txt
    Permissao: 644
Linux.
    chmod 644 teste.txt
    Exibindo os metadados do arquivo

Simulador.
    Opcao: 13
    Arquivo: teste.txt
    Saída (exemplo):
        Nome: teste.txt
        Inode: 1
        Tamanho: 9 bytes
        Permissões: rw-r--r--
        Bloco inicial: 0
        Quantidade de blocos: 1
Linux.
    stat teste.txt
    Truncando um arquivo

Simulador.
    Opcao: 14
    Arquivo: teste.txt
Linux.
    truncate -s 0 teste.txt
    Reposicionando a leitura

Simulador.
    Opcao: 15
    Arquivo: teste.txt
    Posicao: 4
    Saída: Mundo
Linux (conceito equivalente).
    fseek(arquivo, 4, SEEK_SET);
    ou
    lseek(fd, 4, SEEK_SET);


## Exemplos de uso das opções do menu

1 - ls
    Lista arquivos e diretórios do diretório atual.
        Opcao: 1

2 - mkdir
    Cria um diretório.
        Opcao: 2
        Nome do diretorio: documentos

3 - cd
    Entra em um diretório.
        Opcao: 3
        Diretorio: documentos

4 - pwd
    Mostra o caminho atual.
        Opcao: 4

5 - touch
    Cria um arquivo vazio.
        Opcao: 5
        Nome do arquivo: teste.txt

6 - echo > arquivo
    Escreve conteúdo em um arquivo.
        Opcao: 6
        Arquivo: teste.txt
        Conteudo: Ola mundo

7 - cat
    Lê o conteúdo do arquivo.
        Opcao: 7
        Arquivo: teste.txt

8 - chmod
    Altera as permissões do arquivo.
        Opcao: 8
        Arquivo: teste.txt
        Permissao numerica, exemplo 644 ou 755: 644

9 - rm
    Remove um arquivo.
        Opcao: 9
        Arquivo: teste.txt

10 - cp
    Copia um arquivo.
        Opcao: 10
        Origem: teste.txt
        Destino: copia.txt

11 - mv
    Renomeia um arquivo.
        Opcao: 11
        Origem: copia.txt
        Novo nome: final.txt

12 - mostrar blocos
    Mostra quais blocos estão livres ou ocupados.
        Opcao: 12

13 - stat
    Mostra os metadados do FCB.
        Opcao: 13
        Arquivo: teste.txt

14 - truncate
    Apaga o conteúdo do arquivo sem remover o arquivo.
        Opcao: 14
        Arquivo: teste.txt

15 - seek
    Mostra o conteúdo a partir de uma posição.
        Opcao: 15
        Arquivo: teste.txt
        Posicao: 4

16 - trocar usuario
    Troca o usuário atual para testar permissões.
        Opcao: 16
        Usuario atual:
        0 - owner
        1 - group
        2 - public
    Escolha 0 ou 1 ou 2, para selecionar usuario
