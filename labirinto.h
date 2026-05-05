#ifndef LABIRINTO_H
#define LABIRINTO_H

#include <stdio.h>

#define MAX_L 40
#define MAX_C 40

typedef struct {
  int r, c;
} Pos;

/* ===== Mochila (lista encadeada ordenada) ===== */
typedef struct item {
  int valor;
  struct item* prox;
} Item;

typedef struct {
  Item* ini;   /* menor valor sempre aqui */
  int total;
  int qtd;
} Mochila;

void mochila_init(Mochila* m);
void mochila_push_ordenado(Mochila* m, int valor); /* insere em ordem crescente */
int  mochila_pop_menor(Mochila* m, int* valor);    /* remove primeiro (menor) */
void mochila_print(const Mochila* m);
void mochila_free(Mochila* m);

/* ===== Pilha (backtracking) ===== */
typedef struct snode {
  Pos p;
  struct snode* prox;
} SNode;

typedef struct {
  SNode* topo;
  int tam;
} Pilha;

void pilha_init(Pilha* s);
int  pilha_vazia(Pilha* s);
void pilha_push(Pilha* s, Pos p);
int  pilha_pop(Pilha* s, Pos* out);
int  pilha_peek(Pilha* s, Pos* out);
void pilha_free(Pilha* s);

/* ===== Labirinto ===== */
typedef struct {
  int L, C;
  char grid[MAX_L][MAX_C];
  Pos start;
  Pos saida;
} Lab;

int lab_load(Lab* lab, const char* path); /* lê txt, descobre L,C,P,S */
void lab_print(const Lab* lab, Pos player, const int** mark); /* mark pode ser NULL */
int lab_resolve(Lab* lab, Mochila* m, int delay_ms, const char* out_path);

#endif
