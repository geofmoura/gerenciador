#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#define MAX_TEXTO 200
#define MAX_TAREFAS 50

typedef struct {
    int id;
    char nome[MAX_TEXTO];
    int prioridade;
    int tempo_horas;
    char criada_em[50];
} Tarefa;

typedef struct No {
    Tarefa tarefa;
    struct No* proximo;
    struct No* anterior;
} No;

typedef struct {
    No* primeiro;
    No* ultimo;
    int total;
} ListaTarefas;

typedef struct {
    Tarefa tarefa;
    char acao[20];
} HistoricoAcao;

typedef struct {
    HistoricoAcao acoes[50];
    int topo;
} PilhaHistorico;

    typedef struct {
    Tarefa tarefas[MAX_TAREFAS];
    int inicio;
    int fim;
    int total;
} FilaExecucao;

ListaTarefas* criar_lista() {
    ListaTarefas* lista = malloc(sizeof(ListaTarefas));
    lista->primeiro = NULL;
    lista->ultimo = NULL;
    lista->total = 0;
    return lista;
}

// adiciona uma tarefa na lista
void adicionar_tarefa_lista(ListaTarefas* lista, Tarefa tarefa) {
    No* novo = malloc(sizeof(No));
    novo->tarefa = tarefa;
    novo->proximo = NULL;
    novo->anterior = NULL;

    if (lista->primeiro == NULL) {
        lista->primeiro = lista->ultimo = novo;
    } else {
        novo->anterior = lista->ultimo;
        lista->ultimo->proximo = novo;
        lista->ultimo = novo;
    }
    lista->total++;
}

// remove uma tarefa da lista
int remover_tarefa_lista(ListaTarefas* lista, int id, Tarefa* tarefa_removida) {
    No* atual = lista->primeiro;

    while (atual != NULL && atual->tarefa.id != id) {
        atual = atual->proximo;
    }

    if (atual == NULL) return 0;

    *tarefa_removida = atual->tarefa;

    if (atual->anterior) atual->anterior->proximo = atual->proximo;
    else lista->primeiro = atual->proximo;

    if (atual->proximo) atual->proximo->anterior = atual->anterior;
    else lista->ultimo = atual->anterior;

    free(atual);
    lista->total--;
    return 1;
}

PilhaHistorico* criar_historico() {
    PilhaHistorico* pilha = malloc(sizeof(PilhaHistorico));
    pilha->topo = -1;
    return pilha;
}

void salvar_acao(PilhaHistorico* pilha, Tarefa tarefa, const char* acao) {
    if (pilha->topo < 49) {
        pilha->topo++;
        pilha->acoes[pilha->topo].tarefa = tarefa;
        strcpy(pilha->acoes[pilha->topo].acao, acao);
    }
}

int desfazer_acao(PilhaHistorico* pilha, HistoricoAcao* acao) {
    if (pilha->topo >= 0) {
        *acao = pilha->acoes[pilha->topo];
        pilha->topo--;
        return 1;
    }
    return 0;
}

FilaExecucao* criar_fila() {
    FilaExecucao* fila = malloc(sizeof(FilaExecucao));
    fila->inicio = 0;
    fila->fim = -1;
    fila->total = 0;
    return fila;
}

void adicionar_na_fila(FilaExecucao* fila, Tarefa tarefa) {
    if (fila->total < MAX_TAREFAS) {
        fila->fim = (fila->fim + 1) % MAX_TAREFAS;
        fila->tarefas[fila->fim] = tarefa;
        fila->total++;
    }
}

int executar_proxima(FilaExecucao* fila, Tarefa* tarefa) {
    if (fila->total > 0) {
        *tarefa = fila->tarefas[fila->inicio];
        fila->inicio = (fila->inicio + 1) % MAX_TAREFAS;
        fila->total--;
        return 1;
    }
    return 0;
}

void ordenar_por_nome(Tarefa tarefas[], int total) {
    for (int i = 0; i < total - 1; i++) {
        for (int j = 0; j < total - i - 1; j++) {
            if (strcmp(tarefas[j].nome, tarefas[j + 1].nome) > 0) {
                Tarefa temp = tarefas[j];
                tarefas[j] = tarefas[j + 1];
                tarefas[j + 1] = temp;
            }
        }
    }
}

// ordenar por prioridade
void ordenar_por_prioridade(Tarefa tarefas[], int total) {
    for (int i = 0; i < total - 1; i++) {
        int maior = i;
        for (int j = i + 1; j < total; j++) {
            if (tarefas[j].prioridade > tarefas[maior].prioridade) {
                maior = j;
            }
        }
        if (maior != i) {
            Tarefa temp = tarefas[i];
            tarefas[i] = tarefas[maior];
            tarefas[maior] = temp;
        }
    }
}

// ordena por tempo
void ordenar_por_tempo(Tarefa tarefas[], int total) {
    for (int i = 1; i < total; i++) {
        Tarefa atual = tarefas[i];
        int j = i - 1;

        while (j >= 0 && tarefas[j].tempo_horas > atual.tempo_horas) {
            tarefas[j + 1] = tarefas[j];
            j--;
        }
        tarefas[j + 1] = atual;
    }
}

void pegar_hora_atual(char* hora) {
    time_t agora;
    struct tm* info;
    time(&agora);
    info = localtime(&agora);
    strftime(hora, 50, "%d/%m/%Y às %H:%M", info);
}

// mostra prioridade com cor
const char* mostrar_prioridade(int prioridade) {
    switch (prioridade) {
        case 1: return "Baixa";
        case 2: return "Média";
        case 3: return "Alta";
        default: return "Normal";
    }
}

// limpar a tela
void limpar_tela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void ler_texto(char* texto, int tamanho, const char* pergunta) {
    printf("%s", pergunta);
    if (fgets(texto, tamanho, stdin) == NULL) {
        texto[0] = '\0';
        return;
    }

    texto[strcspn(texto, "\r\n")] = '\0';
}

int ler_numero(const char* pergunta, int minimo, int maximo) {
    int numero;
    do {
        printf("%s", pergunta);
        scanf("%d", &numero);
        if (numero < minimo || numero > maximo) {
            printf("ERRO: Digite um número entre %d e %d\n", minimo, maximo);
        }
    } while (numero < minimo || numero > maximo);
    return numero;
}

void lista_para_array(ListaTarefas* lista, Tarefa array[], int* total) {
    *total = 0;
    No* atual = lista->primeiro;
    while (atual != NULL) {
        array[*total] = atual->tarefa;
        atual = atual->proximo;
        (*total)++;
    }
}

void array_para_lista(ListaTarefas* lista, Tarefa array[], int total) {
    No* atual = lista->primeiro;
    while (atual != NULL) {
        No* proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
    lista->primeiro = lista->ultimo = NULL;
    lista->total = 0;

    for (int i = 0; i < total; i++) {
        adicionar_tarefa_lista(lista, array[i]);
    }
}

void mostrar_cabecalho() {
    limpar_tela();
    printf("\n");
    printf("===========================================================\n");
    printf("           ORGANIZADOR DE TAREFAS\n");
    printf("           Simples, pratico e eficiente!\n");
    printf("===========================================================\n");
}

void mostrar_tarefas(ListaTarefas* lista) {
    printf("\n MINHAS TAREFAS (%d)\n", lista->total);
    printf("   -------------------------------\n");

    if (lista->total == 0) {
        printf("   Parabéns! Você não tem tarefas pendentes!\n");
        printf("   Que tal adicionar uma nova tarefa?\n");
        return;
    }

    No* atual = lista->primeiro;
    int contador = 1;

    while (atual != NULL) {
        Tarefa t = atual->tarefa;
        printf("%d. %s [ID: %d]\n", contador, t.nome, t.id);
        printf("   %s | Tempo: %dh | Criada: %s\n", 
               mostrar_prioridade(t.prioridade), t.tempo_horas, t.criada_em);

        if (atual->proximo != NULL) {
            printf("   ⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯⋯\n");
        }

        atual = atual->proximo;
        contador++;
    }
}

void mostrar_fila(FilaExecucao* fila) {
    printf("\n FILA DE EXECUÇÃO (%d)\n", fila->total);
    printf("───────────────────────────────────────────────────────────\n");

    if (fila->total == 0) {
        printf("   Sua fila está vazia\n");
        printf("   Adicione tarefas aqui para se organizar!\n");
        return;
    }

    int pos = fila->inicio;
    for (int i = 0; i < fila->total; i++) {
        Tarefa t = fila->tarefas[pos];
        if (i == 0) {
            printf(">> PRÓXIMA: %s [%d]\n", t.nome, t.id);
        } else {
            printf("   %dª: %s [%d]\n", i + 1, t.nome, t.id);
        }
        pos = (pos + 1) % MAX_TAREFAS;
    }
}

void mostrar_menu() {
    printf("\n O QUE VOCÊ QUER FAZER?\n");
    printf("───────────────────────────────────────────────────────────\n");
    printf("1.  Adicionar nova tarefa\n");
    printf("2.  Remover uma tarefa\n");
    printf("3.  Organizar por nome (A-Z)\n");
    printf("4.  Organizar por prioridade\n");
    printf("5.  Organizar por tempo\n");
    printf("6.  Colocar na fila de execução\n");
    printf("7.  Executar próxima tarefa\n");
    printf("8.  Desfazer última ação\n");
    printf("0.  Sair\n");
    printf("───────────────────────────────────────────────────────────\n");
}


void adicionar_tarefa(ListaTarefas* lista, PilhaHistorico* historico) {
    Tarefa nova;
    char hora[50];

    printf("\n VAMOS ADICIONAR UMA TAREFA!\n");
    printf("───────────────────────────────────────────────────────────\n");

    getchar();
    ler_texto(nova.nome, MAX_TEXTO, "Nome da tarefa: ");

    if (strlen(nova.nome) == 0) {
        printf("ERRO: O nome não pode estar vazio!\n");
        return;
    }

    printf("\nQual a prioridade?\n");
    printf("   1 - Baixa\n");
    printf("   2 - Média\n");
    printf("   3 - Alta\n");
    nova.prioridade = ler_numero("Escolha: ", 1, 3);

    nova.tempo_horas = ler_numero("\nQuantas horas vai levar? ", 1, 100);

    nova.id = rand() % 9000 + 1000;
    pegar_hora_atual(hora);
    strcpy(nova.criada_em, hora);

    adicionar_tarefa_lista(lista, nova);
    salvar_acao(historico, nova, "adicionar");

    printf("\nSUCESSO: Tarefa '%s' foi adicionada! (ID: %d)\n", nova.nome, nova.id);
}

void remover_tarefa(ListaTarefas* lista, PilhaHistorico* historico) {
    if (lista->total == 0) {
        printf("\n Você não tem tarefas para remover!\n");
        return;
    }

    printf("\n REMOVER TAREFA\n");
    printf("───────────────────────────────────────────────────────────\n");

    No* atual = lista->primeiro;
    printf("Suas tarefas:\n");
    while (atual != NULL) {
        printf("   [%d] %s\n", atual->tarefa.id, atual->tarefa.nome);
        atual = atual->proximo;
    }

    int id = ler_numero("\nDigite o ID da tarefa: ", 1000, 9999);

    Tarefa removida;
    if (remover_tarefa_lista(lista, id, &removida)) {
        salvar_acao(historico, removida, "remover");
        printf("\nSUCESSO: Tarefa '%s' foi removida!\n", removida.nome);
    } else {
        printf("\nERRO: Não encontrei tarefa com ID %d\n", id);
    }
}

void organizar_tarefas(ListaTarefas* lista, int tipo) {
    if (lista->total <= 1) {
        printf("\n Você precisa de pelo menos 2 tarefas para organizar!\n");
        return;
    }

    Tarefa tarefas[MAX_TAREFAS];
    int total;

    lista_para_array(lista, tarefas, &total);

    printf("\n Organizando suas tarefas...\n");

    switch (tipo) {
        case 1:
            ordenar_por_nome(tarefas, total);
            printf("SUCESSO: Organizadas por nome (A-Z)!\n");
            break;
        case 2:
            ordenar_por_prioridade(tarefas, total);
            printf("SUCESSO: Organizadas por prioridade (mais importantes primeiro)!\n");
            break;
        case 3:
            ordenar_por_tempo(tarefas, total);
            printf("SUCESSO: Organizadas por tempo (mais rápidas primeiro)!\n");
            break;
    }

    array_para_lista(lista, tarefas, total);
}

void colocar_na_fila(ListaTarefas* lista, FilaExecucao* fila) {
    if (lista->total == 0) {
        printf("\n Você precisa criar tarefas primeiro!\n");
        return;
    }

    printf("\n ADICIONAR À FILA\n");
    printf("───────────────────────────────────────────────────────────\n");

    No* atual = lista->primeiro;
    printf("Tarefas disponíveis:\n");
    while (atual != NULL) {
        printf("   [%d] %s\n", atual->tarefa.id, atual->tarefa.nome);
        atual = atual->proximo;
    }

    int id = ler_numero("\nDigite o ID: ", 1000, 9999);

    atual = lista->primeiro;
    while (atual != NULL) {
        if (atual->tarefa.id == id) {
            adicionar_na_fila(fila, atual->tarefa);
            printf("\nSUCESSO: Tarefa '%s' foi para a fila!\n", atual->tarefa.nome);
            return;
        }
        atual = atual->proximo;
    }

    printf("\nERRO: Não encontrei tarefa com ID %d\n", id);
}

void executar_tarefa(FilaExecucao* fila) {
    Tarefa tarefa;

    if (executar_proxima(fila, &tarefa)) {
        printf("\n EXECUTANDO TAREFA!\n");
        printf("───────────────────────────────────────────────────────────\n");
        printf("CONCLUÍDA: Você concluiu '%s'\n", tarefa.nome);
        printf("Prioridade: %s | Tempo: %dh\n", mostrar_prioridade(tarefa.prioridade), tarefa.tempo_horas);
        printf("\nParabéns! Mais uma tarefa concluída!\n");
    } else {
        printf("\n Não há tarefas na fila para executar\n");
    }
}

void desfazer_ultima_acao(ListaTarefas* lista, PilhaHistorico* historico) {
    HistoricoAcao acao;

    if (desfazer_acao(historico, &acao)) {
        if (strcmp(acao.acao, "adicionar") == 0) {
            Tarefa removida;
            remover_tarefa_lista(lista, acao.tarefa.id, &removida);
            printf("\nDESFEITO: Desfiz a adição de '%s'\n", acao.tarefa.nome);
        } else if (strcmp(acao.acao, "remover") == 0) {
            adicionar_tarefa_lista(lista, acao.tarefa);
            printf("\nRESTAURADO: Restaurei a tarefa '%s'\n", acao.tarefa.nome);
        }
        printf("SUCESSO: Ação desfeita!\n");
    } else {
        printf("\n Não há ações para desfazer\n");
    }
}

int main() {
    setlocale(LC_ALL, "Portuguese_Brazil.utf8");
    srand(time(NULL));

    ListaTarefas* minhas_tarefas = criar_lista();
    PilhaHistorico* historico = criar_historico();
    FilaExecucao* fila = criar_fila();

    int opcao;

    printf("Bem-vindo ao seu Organizador de Tarefas!\n");
    printf("Vamos te ajudar a ser mais produtivo!\n");
    printf("Pressione ENTER para começar...");
    getchar();

    do {
        mostrar_cabecalho();
        mostrar_tarefas(minhas_tarefas);
        mostrar_fila(fila);
        mostrar_menu();

        opcao = ler_numero("Sua escolha: ", 0, 8);

        switch (opcao) {
            case 1: adicionar_tarefa(minhas_tarefas, historico); break;
            case 2: remover_tarefa(minhas_tarefas, historico); break;
            case 3: organizar_tarefas(minhas_tarefas, 1); break;
            case 4: organizar_tarefas(minhas_tarefas, 2); break;
            case 5: organizar_tarefas(minhas_tarefas, 3); break;
            case 6: colocar_na_fila(minhas_tarefas, fila); break;
            case 7: executar_tarefa(fila); break;
            case 8: desfazer_ultima_acao(minhas_tarefas, historico); break;
            case 0:
                printf("\nObrigado por usar o organizador!\n");
                printf("Continue sendo produtivo!\n");
                break;
        }

        if (opcao != 0) {
            printf("\nPressione ENTER para continuar...");
            getchar();
            getchar();
        }

    } while (opcao != 0);

    No* atual = minhas_tarefas->primeiro;
    while (atual != NULL) {
        No* proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
    free(minhas_tarefas);
    free(historico);
    free(fila);

    return 0;
}