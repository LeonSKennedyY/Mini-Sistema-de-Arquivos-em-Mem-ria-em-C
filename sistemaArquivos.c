#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "sistemaArquivos.h"

char disco[TAM_DISCO];
int blocosUsados[MAX_BLOCOS];

Diretorio *raiz;
Diretorio *atual;
int proximoInode = 1;
int usuarioAtual = 0; // 0 = owner, 1 = group, 2 = public

// Verifica se o usuário atual possui permissão para realizar
// uma operação no arquivo: leitura (r), escrita (w) ou execução (x)
int temPermissao(FCB *arq, char op) {
    int deslocamento;

    if (usuarioAtual == 0) deslocamento = 6;
    else if (usuarioAtual == 1) deslocamento = 3;
    else deslocamento = 0;

    int mascara = 0;

    if (op == 'r') mascara = 4;
    if (op == 'w') mascara = 2;
    if (op == 'x') mascara = 1;

    return (arq->permissoes >> deslocamento) & mascara;
}

// Imprime as permissões do arquivo no formato rwxrwxrwx,
// semelhante ao exibido pelo comando ls -l do Linux.
void imprimirPermissoes(int p) {
    int valores[9] = {
        0400, 0200, 0100,
        0040, 0020, 0010,
        0004, 0002, 0001
    };

    char letras[9] = {'r','w','x','r','w','x','r','w','x'};

    for (int i = 0; i < 9; i++) {
        if (p & valores[i]) printf("%c", letras[i]);
        else printf("-");
    }
}

// Procura uma sequência contínua de blocos livres no disco simulado
// e marca esses blocos como ocupados
int alocarBlocos(int tamanho) {
    int qtd = (tamanho + TAM_BLOCO - 1) / TAM_BLOCO;

    for (int i = 0; i <= MAX_BLOCOS - qtd; i++) {
        int livre = 1;

        for (int j = 0; j < qtd; j++) {
            if (blocosUsados[i + j]) {
                livre = 0;
                break;
            }
        }

        if (livre) {
            for (int j = 0; j < qtd; j++) {
                blocosUsados[i + j] = 1;
            }
            return i;
        }
    }

    return -1;
}

// Libera os blocos ocupados por um arquivo no disco simulado
void liberarBlocos(int inicio, int qtd) {
    for (int i = 0; i < qtd; i++) {
        blocosUsados[inicio + i] = 0;
    }
}

// Procura um arquivo pelo nome dentro do diretório atual
FCB *buscarArquivo(char *nome) {
    FCB *aux = atual->arquivos;

    while (aux != NULL) {
        if (strcmp(aux->nome, nome) == 0) return aux;
        aux = aux->prox;
    }

    return NULL;
}

// Procura um subdiretório pelo nome dentro do diretório atual
Diretorio *buscarDiretorio(char *nome) {
    Diretorio *aux = atual->subdirs;

    while (aux != NULL) {
        if (strcmp(aux->nome, nome) == 0) return aux;
        aux = aux->prox;
    }

    return NULL;
}

// Inicializa o sistema de arquivos simulado, criando o diretório raiz
// e limpando o disco e o vetor de controle de blocos
void inicializarSistema() {
    raiz = malloc(sizeof(Diretorio));
    strcpy(raiz->nome, "/");
    raiz->pai = NULL;
    raiz->subdirs = NULL;
    raiz->prox = NULL;
    raiz->arquivos = NULL;

    atual = raiz;

    memset(disco, 0, sizeof(disco));
    memset(blocosUsados, 0, sizeof(blocosUsados));
}

// Verifica se o nome informado é válido, permitindo apenas letras,
// números, ponto, hífen e underline
int nomeValido(char *nome) {

    if (strlen(nome) == 0) {
        return 0;
    }

    for (int i = 0; nome[i] != '\0'; i++) {

        if (!(isalnum(nome[i]) ||
              nome[i] == '_' ||
              nome[i] == '-' ||
              nome[i] == '.')) {

            return 0;
        }
    }

    return 1;
}

// Cria um novo diretório dentro do diretório atual
void mkdirSimulado(char *nome) {
    if (buscarDiretorio(nome) != NULL) {
        printf("Diretorio ja existe.\n");
        return;
    }

    Diretorio *novo = malloc(sizeof(Diretorio));
    strcpy(novo->nome, nome);
    novo->pai = atual;
    novo->subdirs = NULL;
    novo->arquivos = NULL;

    novo->prox = atual->subdirs;
    atual->subdirs = novo;

    printf("Diretorio criado: %s\n", nome);
}

// Altera o diretório atual, permitindo entrar em subdiretórios
// ou voltar ao diretório pai usando ".."
void cdSimulado(char *nome) {
    if (strcmp(nome, "..") == 0) {
        if (atual->pai != NULL) atual = atual->pai;
        return;
    }

    Diretorio *dir = buscarDiretorio(nome);

    if (dir == NULL) {
        printf("Diretorio nao encontrado.\n");
        return;
    }

    atual = dir;
}

// Função recursiva auxiliar usada para imprimir o caminho completo
// do diretório atual
void pwdRec(Diretorio *dir) {
    if (dir == NULL) return;
    if (dir->pai != NULL) pwdRec(dir->pai);
    if (strcmp(dir->nome, "/") != 0) printf("/%s", dir->nome);
}

// Exibe o caminho completo do diretório atual
void pwdSimulado() {
    if (atual == raiz) printf("/");
    else pwdRec(atual);
    printf("\n");
}

// Lista os diretórios e arquivos existentes no diretório atual,
// exibindo permissões, inode, tamanho e blocos dos arquivos
void lsSimulado() {
    Diretorio *d = atual->subdirs;

    while (d != NULL) {
        printf("d--------- %s\n", d->nome);
        d = d->prox;
    }

    FCB *a = atual->arquivos;

    while (a != NULL) {
        printf("-");
        imprimirPermissoes(a->permissoes);
        printf(" inode:%d tam:%d blocos:%d inicio:%d %s\n",
               a->inode, a->tamanho, a->qtdBlocos, a->blocoInicial, a->nome);
        a = a->prox;
    }
}

// Cria um novo arquivo vazio no diretório atual,
// inicializando seu FCB com inode, datas, permissões e metadados
void touchSimulado(char *nome) {
    if (!nomeValido(nome)) {
        printf("Nome de arquivo invalido.\n");
        return;
    }

    if (buscarArquivo(nome) != NULL) {
        printf("Arquivo ja existe.\n");
        return;
    }

    FCB *novo = malloc(sizeof(FCB));
    strcpy(novo->nome, nome);

    novo->inode = proximoInode++;
    novo->tamanho = 0;
    novo->tipo = TIPO_CARACTERE;
    novo->criacao = time(NULL);
    novo->modificacao = time(NULL);
    novo->acesso = time(NULL);
    novo->permissoes = 0644;
    novo->blocoInicial = -1;
    novo->qtdBlocos = 0;

    novo->prox = atual->arquivos;
    atual->arquivos = novo;

    printf("Arquivo criado: %s\n", nome);
}

// Escreve conteúdo em um arquivo, verificando permissão de escrita
// e alocando blocos no disco simulado
void echoSimulado(char *nome, char *conteudo) {
    FCB *arq = buscarArquivo(nome);

    if (arq == NULL) {
        printf("Arquivo nao encontrado.\n");
        return;
    }

    if (!temPermissao(arq, 'w')) {
        printf("Permissao negada para escrita.\n");
        return;
    }

    if (arq->blocoInicial != -1) {
        liberarBlocos(arq->blocoInicial, arq->qtdBlocos);
    }

    int tamanho = strlen(conteudo);
    int bloco = alocarBlocos(tamanho);

    if (bloco == -1) {
        printf("Sem espaco no disco simulado.\n");
        return;
    }

    arq->blocoInicial = bloco;
    arq->qtdBlocos = (tamanho + TAM_BLOCO - 1) / TAM_BLOCO;
    arq->tamanho = tamanho;
    arq->modificacao = time(NULL);

    strcpy(&disco[bloco * TAM_BLOCO], conteudo);

    printf("Conteudo escrito no arquivo.\n");
}

// Lê e exibe o conteúdo de um arquivo, verificando permissão de leitura
void catSimulado(char *nome) {
    FCB *arq = buscarArquivo(nome);

    if (arq == NULL) {
        printf("Arquivo nao encontrado.\n");
        return;
    }

    if (!temPermissao(arq, 'r')) {
        printf("Permissao negada para leitura.\n");
        return;
    }

    arq->acesso = time(NULL);

    if (arq->blocoInicial == -1) {
        printf("(arquivo vazio)\n");
        return;
    }

    printf("%s\n", &disco[arq->blocoInicial * TAM_BLOCO]);
}

// Altera as permissões numéricas de um arquivo,
// simulando o comportamento do chmod do Linux
void chmodSimulado(char *nome, int permissao) {
    FCB *arq = buscarArquivo(nome);

    if (arq == NULL) {
        printf("Arquivo nao encontrado.\n");
        return;
    }

    arq->permissoes = permissao;
    printf("Permissao alterada.\n");
}

// Remove um arquivo do diretório atual, liberando seus blocos
// e removendo seu FCB da lista de arquivos
void rmSimulado(char *nome) {
    FCB *ant = NULL;
    FCB *aux = atual->arquivos;

    while (aux != NULL) {
        if (strcmp(aux->nome, nome) == 0) {

            if (!temPermissao(aux, 'w')) {
                printf("Permissao negada para remover.\n");
                return;
            }

            if (aux->blocoInicial != -1) {
                liberarBlocos(aux->blocoInicial, aux->qtdBlocos);
            }

            if (ant == NULL) atual->arquivos = aux->prox;
            else ant->prox = aux->prox;

            free(aux);
            printf("Arquivo removido.\n");
            return;
        }

        ant = aux;
        aux = aux->prox;
    }

    printf("Arquivo nao encontrado.\n");
}

// Copia um arquivo existente para um novo arquivo,
// duplicando seu conteúdo e permissões
void cpSimulado(char *origem, char *destino) {
    FCB *arq = buscarArquivo(origem);

    if (arq == NULL) {
        printf("Arquivo origem nao encontrado.\n");
        return;
    }

    if (!temPermissao(arq, 'r')) {
        printf("Permissao negada para copiar.\n");
        return;
    }

    touchSimulado(destino);

    FCB *novo = buscarArquivo(destino);

    if (arq->blocoInicial != -1) {
        echoSimulado(destino, &disco[arq->blocoInicial * TAM_BLOCO]);
    }

    novo->permissoes = arq->permissoes;

    printf("Arquivo copiado.\n");
}

// Renomeia um arquivo existente, verificando permissão de escrita
// e validade do novo nome
void mvSimulado(char *origem, char *destino) {
    FCB *arq = buscarArquivo(origem);

    if (arq == NULL) {
        printf("Arquivo nao encontrado.\n");
        return;
    }

    if (!temPermissao(arq, 'w')) {
        printf("Permissao negada para mover/renomear.\n");
        return;
    }

    if (!nomeValido(destino)) {
    printf("Novo nome invalido.\n");
    return;
    }

    strcpy(arq->nome, destino);
    arq->modificacao = time(NULL);

    printf("Arquivo renomeado/movido.\n");
}

// Exibe o estado dos blocos do disco simulado,
// mostrando quais estão livres e quais estão ocupados
void blocosSimulado() {
    printf("Estado dos blocos:\n");

    for (int i = 0; i < MAX_BLOCOS; i++) {
        printf("[%d:%s] ", i, blocosUsados[i] ? "X" : "Livre");

        if ((i + 1) % 8 == 0) printf("\n");
    }
}

// Converte o tipo numérico do arquivo para texto,
// facilitando a exibição no comando stat
const char *tipoParaTexto(TipoArquivo tipo) {
    switch (tipo) {
        case TIPO_NUMERICO: return "Numerico";
        case TIPO_CARACTERE: return "Caractere";
        case TIPO_BINARIO: return "Binario";
        case TIPO_PROGRAMA: return "Programa";
        default: return "Desconhecido";
    }
}

// Exibe todos os metadados armazenados no FCB do arquivo,
// como inode, tamanho, tipo, permissões, datas e blocos
void statSimulado(char *nome) {
    FCB *arq = buscarArquivo(nome);

    if (arq == NULL) {
        printf("Arquivo nao encontrado.\n");
        return;
    }

    printf("\n  FCB DO ARQUIVO\n");
    printf("Nome: %s\n", arq->nome);
    printf("Inode simulado: %d\n", arq->inode);
    printf("Tamanho: %d bytes\n", arq->tamanho);
    printf("Tipo: %s\n", tipoParaTexto(arq->tipo));

    printf("Permissoes: ");
    imprimirPermissoes(arq->permissoes);
    printf(" (%o)\n", arq->permissoes);

    printf("Data de criacao: %s", ctime(&arq->criacao));
    printf("Data de modificacao: %s", ctime(&arq->modificacao));
    printf("Data de acesso: %s", ctime(&arq->acesso));

    printf("Bloco inicial: %d\n", arq->blocoInicial);
    printf("Quantidade de blocos: %d\n", arq->qtdBlocos);
}
// Remove todo o conteúdo de um arquivo sem apagar o arquivo,
// liberando os blocos e definindo o tamanho como zero
void truncateSimulado(char *nome) {
    FCB *arq = buscarArquivo(nome);

    if (arq == NULL) {
        printf("Arquivo nao encontrado.\n");
        return;
    }

    if (!temPermissao(arq, 'w')) {
        printf("Permissao negada para truncar arquivo.\n");
        return;
    }

    if (arq->blocoInicial != -1) {
        liberarBlocos(arq->blocoInicial, arq->qtdBlocos);
        memset(&disco[arq->blocoInicial * TAM_BLOCO], 0, arq->qtdBlocos * TAM_BLOCO);
    }

    arq->tamanho = 0;
    arq->blocoInicial = -1;
    arq->qtdBlocos = 0;
    arq->modificacao = time(NULL);

    printf("Arquivo truncado. Conteudo apagado e tamanho definido como 0.\n");
}

// Reposiciona a leitura para uma posição específica do arquivo,
// exibindo o conteúdo a partir dessa posição
void seekSimulado(char *nome, int posicao) {
    FCB *arq = buscarArquivo(nome);

    if (arq == NULL) {
        printf("Arquivo nao encontrado.\n");
        return;
    }

    if (!temPermissao(arq, 'r')) {
        printf("Permissao negada para reposicionar/leitura.\n");
        return;
    }

    if (posicao < 0 || posicao >= arq->tamanho) {
        printf("Posicao invalida. O arquivo possui %d bytes.\n", arq->tamanho);
        return;
    }

    arq->acesso = time(NULL);

    printf("Conteudo a partir da posicao %d:\n", posicao);
    printf("%s\n", &disco[arq->blocoInicial * TAM_BLOCO + posicao]);
}

// Exibe o menu principal do simulador e chama as funções
// de acordo com a opção escolhida pelo usuário
void menuSistema() {
    int opcao;
    char nome[MAX_NOME];
    char nome2[MAX_NOME];
    char conteudo[MAX_CONTEUDO];
    int permissao;
    printf("\n");

    do {
        printf("\n  MINI SISTEMA DE ARQUIVOS\n");
        printf("1  - ls\n");
        printf("2  - mkdir\n");
        printf("3  - cd\n");
        printf("4  - pwd\n");
        printf("5  - touch\n");
        printf("6  - echo > arquivo\n");
        printf("7  - cat\n");
        printf("8  - chmod\n");
        printf("9  - rm\n");
        printf("10 - cp\n");
        printf("11 - mv\n");
        printf("12 - mostrar blocos\n");
        printf("13 - stat\n");
        printf("14 - truncate\n");
        printf("15 - seek\n");
        printf("16 - trocar usuario\n");
        printf("0  - sair\n");
        printf("Opcao: ");

        scanf("%d", &opcao);
        getchar();

        printf("\n");

        switch(opcao) {
            case 1:
                lsSimulado();
                break;

            case 2:
                printf("Nome do diretorio: ");
                scanf("%s", nome);
                mkdirSimulado(nome);
                break;

            case 3:
                printf("Diretorio: ");
                scanf("%s", nome);
                cdSimulado(nome);
                break;

            case 4:
                pwdSimulado();
                break;

            case 5:
                printf("Nome do arquivo: ");
                scanf("%s", nome);
                touchSimulado(nome);
                break;

            case 6:
                printf("Arquivo: ");
                scanf("%s", nome);
                getchar();

                printf("Conteudo: ");
                fgets(conteudo, MAX_CONTEUDO, stdin);
                conteudo[strcspn(conteudo, "\n")] = '\0';

                echoSimulado(nome, conteudo);
                break;

            case 7:
                printf("Arquivo: ");
                scanf("%s", nome);
                catSimulado(nome);
                break;

            case 8:
                printf("Arquivo: ");
                scanf("%s", nome);
                printf("Permissao numerica, exemplo 644 ou 755: ");
                scanf("%o", &permissao);
                chmodSimulado(nome, permissao);
                break;

            case 9:
                printf("Arquivo: ");
                scanf("%s", nome);
                rmSimulado(nome);
                break;

            case 10:
                printf("Origem: ");
                scanf("%s", nome);
                printf("Destino: ");
                scanf("%s", nome2);
                cpSimulado(nome, nome2);
                break;

            case 11:
                printf("Origem: ");
                scanf("%s", nome);
                printf("Novo nome: ");
                scanf("%s", nome2);
                mvSimulado(nome, nome2);
                break;

            case 12:
                blocosSimulado();
                break;

            case 13:
                printf("Arquivo: ");
                scanf("%s", nome);
                statSimulado(nome);
                break;

            case 14:
                printf("Arquivo: ");
                scanf("%s", nome);
                truncateSimulado(nome);
                break;

            case 15:
                printf("Arquivo: ");
                scanf("%s", nome);
                printf("Posicao: ");
                scanf("%d", &permissao);
                seekSimulado(nome, permissao);
                break;

            case 16:
                printf("Usuario atual:\n");
                printf("0 - owner\n");
                printf("1 - group\n");
                printf("2 - public\n");
                printf("Escolha: ");
                scanf("%d", &usuarioAtual);

                if (usuarioAtual < 0 || usuarioAtual > 2) {
                    usuarioAtual = 0;
                    printf("Opcao invalida. Usuario definido como owner.\n");
                } else {
                    printf("Usuario alterado com sucesso.\n");
                }
                break;    

            case 0:
                printf("Encerrando...\n");
                break;

            default:
                printf("Opcao invalida.\n");
        }

    } while(opcao != 0);
}