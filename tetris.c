#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* =========================
   Tetris Stack - Nível Aventureiro
   Fila circular (5 peças) + Pilha (3 peças)
   Autor: aluno "grad" (Luiza team)
   ========================= */

#define Q_CAP 5   // capacidade fixa da fila (sempre cheia)
#define S_CAP 3   // capacidade fixa da pilha de reserva

/* ---------- Modelo de Peça ---------- */
typedef struct {
    char tipo;   // 'I','O','T','L','J','S','Z'
    int  id;     // identificador único incremental
} Peca;

/* ---------- Fila Circular ---------- */
typedef struct {
    Peca v[Q_CAP];
    int ini;     // índice do primeiro
    int fim;     // índice do próximo livre
    int qt;      // quantidade atual
} Fila;

/* ---------- Pilha Linear ---------- */
typedef struct {
    Peca v[S_CAP];
    int topo;    // -1 vazia, 0..S_CAP-1 cheia
} Pilha;

/* ---------- Protótipos ---------- */
void fila_init(Fila *q);
int  fila_cheia(const Fila *q);
int  fila_vazia(const Fila *q);
int  fila_enfileirar(Fila *q, Peca x);
int  fila_primeiro(const Fila *q, Peca *out);
int  fila_desenfileirar(Fila *q, Peca *out);

void pilha_init(Pilha *s);
int  pilha_vazia(const Pilha *s);
int  pilha_cheia(const Pilha *s);
int  pilha_push(Pilha *s, Peca x);
int  pilha_pop(Pilha *s, Peca *out);

Peca gerarPeca(void);
void exibir_fila(const Fila *q);
void exibir_pilha(const Pilha *s);

/* ---------- Estado global simples para gerar IDs ---------- */
static int NEXT_ID = 1;

/* ---------- Implementações ---------- */
void fila_init(Fila *q){
    q->ini = 0; q->fim = 0; q->qt = 0;
}
int fila_cheia(const Fila *q){ return q->qt == Q_CAP; }
int fila_vazia(const Fila *q){ return q->qt == 0; }

int fila_enfileirar(Fila *q, Peca x){
    if (fila_cheia(q)) return 0;
    q->v[q->fim] = x;
    q->fim = (q->fim + 1) % Q_CAP;
    q->qt++;
    return 1;
}

int fila_primeiro(const Fila *q, Peca *out){
    if (fila_vazia(q)) return 0;
    *out = q->v[q->ini];
    return 1;
}

int fila_desenfileirar(Fila *q, Peca *out){
    if (fila_vazia(q)) return 0;
    *out = q->v[q->ini];
    q->ini = (q->ini + 1) % Q_CAP;
    q->qt--;
    return 1;
}

void pilha_init(Pilha *s){ s->topo = -1; }
int  pilha_vazia(const Pilha *s){ return s->topo < 0; }
int  pilha_cheia(const Pilha *s){ return s->topo >= S_CAP - 1; }

int  pilha_push(Pilha *s, Peca x){
    if (pilha_cheia(s)) return 0;
    s->v[++s->topo] = x;
    return 1;
}

int  pilha_pop(Pilha *s, Peca *out){
    if (pilha_vazia(s)) return 0;
    *out = s->v[s->topo--];
    return 1;
}

/* Gera peça automaticamente (tipos rodando) */
Peca gerarPeca(void){
    static const char tipos[] = {'I','O','T','L','J','S','Z'};
    static int idx = 0;
    Peca p;
    p.tipo = tipos[idx];
    p.id   = NEXT_ID++;
    idx = (idx + 1) % (int)(sizeof(tipos)/sizeof(tipos[0]));
    return p;
}

/* Impressões de estado */
void exibir_fila(const Fila *q){
    printf("Fila (frente -> tras) [%d/%d]: ", q->qt, Q_CAP);
    if (fila_vazia(q)){ puts("(vazia)"); return; }
    int i = q->ini;
    for (int k = 0; k < q->qt; k++){
        Peca p = q->v[i];
        printf("[%c#%d]%s", p.tipo, p.id, (k==q->qt-1?"":" -> "));
        i = (i + 1) % Q_CAP;
    }
    putchar('\n');
}

void exibir_pilha(const Pilha *s){
    printf("Pilha reserva (base -> topo) [%d/%d]: ", s->topo+1, S_CAP);
    if (pilha_vazia(s)){ puts("(vazia)"); return; }
    for (int i = 0; i <= s->topo; i++){
        Peca p = s->v[i];
        printf("[%c#%d]%s", p.tipo, p.id, (i==s->topo?"":" | "));
    }
    putchar('\n');
}

/* ---------- Loop principal (menu) ---------- */
int main(void){
    Fila fila; Pilha reserva;
    fila_init(&fila);
    pilha_init(&reserva);

    /* Inicializa a fila com 5 peças (sempre cheia) */
    for (int i = 0; i < Q_CAP; i++){
        if (!fila_enfileirar(&fila, gerarPeca())){
            fprintf(stderr, "Falha ao inicializar a fila.\n");
            return 1;
        }
    }

    int op;
    do {
        puts("\n=== TETRIS STACK - Nível Aventureiro ===");
        exibir_fila(&fila);
        exibir_pilha(&reserva);
        puts("\nMenu:");
        puts("1 - Jogar peça (consome a da frente da fila)");
        puts("2 - Reservar peça (manda a da frente para a pilha)");
        puts("3 - Usar peça reservada (pop da pilha)");
        puts("0 - Sair");
        printf("Escolha: ");
        if (scanf("%d", &op)!=1){ puts("Entrada inválida."); return 0; }

        if (op == 1){
            /* Jogar peça: retira da frente e repõe com nova para manter 5 */
            Peca jogada;
            if (!fila_desenfileirar(&fila, &jogada)){
                puts("Fila vazia (não deveria acontecer)!");
            } else {
                printf(">> Jogou a peça da fila: [%c#%d]\n", jogada.tipo, jogada.id);
                /* Repor automaticamente para manter a fila cheia */
                if (!fila_enfileirar(&fila, gerarPeca())){
                    puts(">> ERRO: fila cheia ao repor (inconsistência)!");
                }
            }
        }
        else if (op == 2){
            /* Reservar: pega a da frente e empilha; repõe a fila com nova */
            Peca front;
            if (!fila_primeiro(&fila, &front)){
                puts("Fila vazia (não deveria acontecer)!");
            } else if (pilha_cheia(&reserva)){
                puts(">> Pilha de reserva cheia! Use alguma peça reservada primeiro.");
            } else {
                /* Consome da fila e empilha */
                fila_desenfileirar(&fila, &front);
                if (!pilha_push(&reserva, front)){
                    puts(">> ERRO ao reservar (pilha)!");
                } else {
                    printf(">> Reservou: [%c#%d] (foi para a pilha)\n", front.tipo, front.id);
                    /* Repor automaticamente a fila */
                    if (!fila_enfileirar(&fila, gerarPeca())){
                        puts(">> ERRO: fila cheia ao repor (inconsistência)!");
                    }
                }
            }
        }
        else if (op == 3){
            /* Usar peça reservada: pop da pilha (não mexe na fila) */
            Peca usada;
            if (!pilha_pop(&reserva, &usada)){
                puts(">> Não há peças reservadas para usar.");
            } else {
                printf(">> Usou peça reservada: [%c#%d]\n", usada.tipo, usada.id);
            }
        }
        else if (op == 0){
            puts("Encerrando. Valeu jogar o Tetris Stack!");
        }
        else {
            puts("Opção inválida.");
        }

    } while (op != 0);

    return 0;
}