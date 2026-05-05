#include "labirinto.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
static void sleep_ms(int ms){ Sleep(ms); }
#else
#include <unistd.h>
static void sleep_ms(int ms){ usleep(ms * 1000); }
#endif

/* ============ Mochila ============ */

void mochila_init(Mochila* m){
  m->ini = NULL;
  m->total = 0;
  m->qtd = 0;
}

void mochila_push_ordenado(Mochila* m, int valor){
  Item* novo = (Item*)malloc(sizeof(Item));
  if(!novo){ perror("malloc"); exit(1); }
  novo->valor = valor;
  novo->prox = NULL;

  /* inserção ordenada crescente */
  if(m->ini == NULL || valor <= m->ini->valor){
    novo->prox = m->ini;
    m->ini = novo;
  } else {
    Item* cur = m->ini;
    while(cur->prox && cur->prox->valor < valor) cur = cur->prox;
    novo->prox = cur->prox;
    cur->prox = novo;
  }

  m->total += valor;
  m->qtd++;
}

int mochila_pop_menor(Mochila* m, int* valor){
  if(!m->ini) return 0;
  Item* x = m->ini;
  m->ini = x->prox;
  if(valor) *valor = x->valor;
  m->total -= x->valor;
  m->qtd--;
  free(x);
  return 1;
}

void mochila_print(const Mochila* m){
  printf("Mochila (qtd=%d, total=%d): [", m->qtd, m->total);
  const Item* cur = m->ini;
  while(cur){
    printf("%d", cur->valor);
    if(cur->prox) printf(", ");
    cur = cur->prox;
  }
  printf("]\n");
}

void mochila_free(Mochila* m){
  Item* cur = m->ini;
  while(cur){
    Item* nx = cur->prox;
    free(cur);
    cur = nx;
  }
  m->ini = NULL;
  m->total = 0;
  m->qtd = 0;
}

/* ============ Pilha ============ */

void pilha_init(Pilha* s){ s->topo = NULL; s->tam = 0; }
int pilha_vazia(Pilha* s){ return s->topo == NULL; }

void pilha_push(Pilha* s, Pos p){
  SNode* n = (SNode*)malloc(sizeof(SNode));
  if(!n){ perror("malloc"); exit(1); }
  n->p = p;
  n->prox = s->topo;
  s->topo = n;
  s->tam++;
}

int pilha_pop(Pilha* s, Pos* out){
  if(!s->topo) return 0;
  SNode* n = s->topo;
  s->topo = n->prox;
  if(out) *out = n->p;
  free(n);
  s->tam--;
  return 1;
}

int pilha_peek(Pilha* s, Pos* out){
  if(!s->topo) return 0;
  if(out) *out = s->topo->p;
  return 1;
}

void pilha_free(Pilha* s){
  Pos tmp;
  while(pilha_pop(s, &tmp)) {}
}

/* ============ Util ============ */

static int dentro(const Lab* lab, int r, int c){
  return r >= 0 && r < lab->L && c >= 0 && c < lab->C;
}

static int caminhavel(char ch){
  return (ch == ' ' || ch == 'P' || ch == 'T' || ch == 'A' || ch == 'S');
}

static void clear_screen(void){
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

/* ============ Carregar lab ============ */

int lab_load(Lab* lab, const char* path){
  FILE* f = fopen(path, "r");
  if(!f) return 0;

  char line[256];
  int L = 0, Cmax = 0;
  int i,j,c;
  /* zera grid */
  for(i=0;i<MAX_L;i++)
    for(j=0;j<MAX_C;j++)
      lab->grid[i][j] = ' ';

  lab->start.r = lab->start.c = -1;
  lab->saida.r = lab->saida.c = -1;

  while(fgets(line, sizeof(line), f) && L < MAX_L){
    int len = (int)strlen(line);
    while(len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) line[--len] = '\0';
    if(len > MAX_C) len = MAX_C;
    if(len > Cmax) Cmax = len;

    for(c=0;c<len;c++){
      char ch = line[c];
      lab->grid[L][c] = ch;

      if(ch == 'P'){ lab->start.r = L; lab->start.c = c; }
      if(ch == 'S'){ lab->saida.r = L; lab->saida.c = c; }
    }
    L++;
  }

  fclose(f);

  lab->L = L;
  lab->C = Cmax;

  if(lab->start.r < 0 || lab->saida.r < 0) return 0;
  return 1;
}

void lab_print(const Lab* lab, Pos player, const int** mark){
  int c, r;
  (void)mark; /* mark não usado aqui (mantive assinatura extensível) */
  for(r=0;r<lab->L;r++){
    for(c=0;c<lab->C;c++){
      if(r == player.r && c == player.c) putchar('P');
      else putchar(lab->grid[r][c]);
    }
    putchar('\n');
  }
}

/* ============ Resolver (DFS com backtracking) ============ */
/* Estratégia:
   - Usamos pilha para o caminho atual.
   - 'visit[r][c]' marca células já visitadas para não entrar em loop.
   - 'parent[r][c]' guarda o predecessor para reconstruir caminho final e gravar em arquivo.
*/
int lab_resolve(Lab* lab, Mochila* m, int delay_ms, const char* out_path){
  int visit[MAX_L][MAX_C] = {0};
  Pos parent[MAX_L][MAX_C];
  int r,c;
  for(r=0;r<MAX_L;r++) for(c=0;c<MAX_C;c++) parent[r][c] = (Pos){-1,-1};

  Pilha st; pilha_init(&st);

  Pos s = lab->start;
  pilha_push(&st, s);
  visit[s.r][s.c] = 1;

  /* RNG */
  srand((unsigned)time(NULL));

  /* movimentos 4-direções */
  const int dr[4] = {-1, 1, 0, 0};
  const int dc[4] = {0, 0, -1, 1};

  while(!pilha_vazia(&st)){
    Pos cur; pilha_peek(&st, &cur);

    /* visual */
    clear_screen();
    lab_print(lab, cur, NULL);
    mochila_print(m);
    if(delay_ms > 0) sleep_ms(delay_ms);

    char ch = lab->grid[cur.r][cur.c];

    /* interações */
    if(ch == 'T'){
      int valor = (rand() % 100) + 1;
      mochila_push_ordenado(m, valor);
      lab->grid[cur.r][cur.c] = ' '; /* tesouro coletado */
    } else if(ch == 'A'){
      int perdido;
      if(mochila_pop_menor(m, &perdido)){
        /* ok */
      } else {
        /* mochila vazia -> só segue */
      }
      lab->grid[cur.r][cur.c] = ' '; /* armadilha consumida */
    } else if(ch == 'S'){
      /* achou saída: gravar solução */
      FILE* out = fopen(out_path, "w");
      if(!out){
        pilha_free(&st);
        return 0;
      }

      /* faz uma cópia do grid e marca caminho com '.' */
      char sol[MAX_L][MAX_C];
      int r,c;
      for(r=0;r<lab->L;r++)
        for(c=0;c<lab->C;c++)
          sol[r][c] = lab->grid[r][c];

      Pos p = cur;
      while(!(p.r == -1 && p.c == -1)){
        char here = sol[p.r][p.c];
        if(here == ' ' ) sol[p.r][p.c] = '.';
        /* mantém P e S */
        if(p.r == lab->start.r && p.c == lab->start.c) sol[p.r][p.c] = 'P';
        if(p.r == lab->saida.r && p.c == lab->saida.c) sol[p.r][p.c] = 'S';
        Pos prev = parent[p.r][p.c];
        p = prev;
      }
      
      for(r=0;r<lab->L;r++){
        for(c=0;c<lab->C;c++) fputc(sol[r][c], out);
        fputc('\n', out);
      }
      fclose(out);

      pilha_free(&st);
      return 1;
    }

    /* tenta avançar para algum vizinho não visitado */
    int avancou = 0;
    int k;
    for(k=0;k<4;k++){
      int nr = cur.r + dr[k];
      int nc = cur.c + dc[k];
      if(!dentro(lab, nr, nc)) continue;
      if(visit[nr][nc]) continue;
      if(!caminhavel(lab->grid[nr][nc])) continue;

      Pos nxt = {nr, nc};
      visit[nr][nc] = 1;
      parent[nr][nc] = cur;
      pilha_push(&st, nxt);
      avancou = 1;
      break;
    }

    /* se não avançou, backtrack */
    if(!avancou){
      Pos lixo;
      pilha_pop(&st, &lixo);
    }
  }

  /* não encontrou saída */
  pilha_free(&st);
  return 0;
}
