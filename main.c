#include "labirinto.h"
#include <stdio.h>

int main(void){
  Lab lab;
  Mochila m;
  mochila_init(&m);

  const char* in_path  = "mapa.txt";
  const char* out_path = "solucao.txt";

  if(!lab_load(&lab, in_path)){
    printf("Erro: nao foi possivel carregar '%s' (verifique arquivo, P e S).\n", in_path);
    return 1;
  }

  int delay_ms = 100; /* ajuste: 0 para sem delay, 50~200 para visualizar */

  int ok = lab_resolve(&lab, &m, delay_ms, out_path);
  if(ok){
    printf("\nSAIDA ENCONTRADA!\n");
    printf("Total de tesouros: %d (qtd=%d)\n", m.total, m.qtd);
    printf("Solucao gravada em: %s\n", out_path);
  } else {
    printf("\nNao foi possivel encontrar a saida.\n");
  }

  mochila_free(&m);
  return 0;
}
