#ifndef SISTEMA_ARQUIVOS_H
#define SISTEMA_ARQUIVOS_H

#include <time.h>   // Biblioteca usada para trabalhar com datas e horários

#define MAX_NOME 50         // Tamanho máximo permitido para nomes de arquivos e diretórios
#define MAX_CONTEUDO 512    // Tamanho máximo do conteúdo digitado para um arquivo
#define TAM_DISCO 4096      // Tamanho total do disco simulado em bytes
#define TAM_BLOCO 64        // Tamanho de cada bloco do disco simulado
#define MAX_BLOCOS (TAM_DISCO / TAM_BLOCO)  // Quantidade total de blocos do disco simulado

// Enumeração que representa os tipos possíveis de arquivo no simulador
typedef enum {
    TIPO_NUMERICO = 1,  // Arquivo de dados numéricos
    TIPO_CARACTERE,     // Arquivo de texto/caracteres
    TIPO_BINARIO,       // Arquivo binário
    TIPO_PROGRAMA       // Arquivo executável/programa
} TipoArquivo;

// Estrutura que representa o File Control Block (FCB)
// O FCB armazena os metadados de cada arquivo do sistema simulado
typedef struct FCB {        // File Control Block
    int inode;              // identificador    
    char nome[MAX_NOME];    // nome
    int tamanho;            // tamanho 
    TipoArquivo tipo;       // tipo 
    time_t criacao;         // data criação
    time_t modificacao;     // data modificação
    time_t acesso;          // data acesso
    int permissoes;         // proteção
    int blocoInicial;       // Localização no disco
    int qtdBlocos;          // Quantidade de blocos
    struct FCB *prox;       // Ponteiro para o próximo arquivo da lista encadeada
} FCB;

// Estrutura que representa um diretório no sistema de arquivos simulado
// Os diretórios são organizados em uma árvore
typedef struct Diretorio {
    char nome[MAX_NOME];        // Nome do diretório    
    struct Diretorio *pai;      // Ponteiro para o diretório pai
    struct Diretorio *subdirs;  // Ponteiro para a lista de subdiretórios
    struct Diretorio *prox;     // Ponteiro para o próximo diretório no mesmo nível
    FCB *arquivos;              // Ponteiro para a lista de arquivos do diretório
} Diretorio;

// Inicializa o sistema de arquivos, criando o diretório raiz
// e preparando o disco simulado
void inicializarSistema();

// Exibe o menu principal e controla a interação com o usuário
void menuSistema();                         

// Exibe todos os metadados armazenados no FCB de um arquivo
void statSimulado(char *nome);

// Remove o conteúdo de um arquivo sem excluir o arquivo
void truncateSimulado(char *nome);

// Reposiciona a leitura para uma posição específica dentro do arquivo
void seekSimulado(char *nome, int posicao);

// Verifica se um nome de arquivo ou diretório é válido
int nomeValido(char *nome);

#endif