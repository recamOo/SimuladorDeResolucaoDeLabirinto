# SimuladorDeResolucaoDeLabirinto

---

# 🌀 Simulador de Resolução de Labirinto

Este projeto é um programa escrito em **C** que simula um explorador tentando encontrar a saída de um labirinto. Ele não apenas caminha pelo mapa, mas também gerencia uma mochila de itens e lida com eventos no caminho.

## 📖 O que este código faz?

Imagine que você coloca um robô dentro de um labirinto de papel. O robô tem uma lanterna, uma mochila e um diário de bordo. Este programa faz exatamente isso:
1. Ele lê um arquivo de texto que representa o mapa.
2. Ele decide para onde ir usando uma técnica chamada **Busca em Profundidade**.
3. Ele coleta tesouros e perde itens em armadilhas.
4. No final, ele grava um arquivo mostrando o caminho exato que ele fez para vencer.

---

## 🛠️ Como funciona o "Cérebro" do programa?

Para resolver o labirinto, o código usa três conceitos fundamentais de computação:

### 1. A Pilha (O rastro de migalhas)
O explorador usa uma **Pilha** para não se perder. 
* Toda vez que ele dá um passo para uma nova área, ele anota essa posição e a coloca no topo de uma "pilha de anotações".
* Se ele chegar em um beco sem saída, ele olha para o topo da pilha, vê de onde veio e volta um passo (**Backtracking**). 
* É como o fio de Ariadne no mito do Minotauro: ele sempre sabe como voltar para tentar um caminho diferente.


### 2. A Mochila (Lista Ordenada)
Durante a caminhada, o explorador pode encontrar:
* **Tesouros (T):** Adicionam um valor aleatório à mochila. O código guarda esses valores em ordem (do menor para o maior).
* **Armadilhas (A):** Se o explorador cair em uma, ele perde o item de **menor valor** que tiver na mochila.
* A mochila funciona como um "colar de contas", onde cada conta é um item conectado ao próximo (**Lista Encadeada**).


### 3. O Mapa (Grade de Caracteres)
O labirinto é lido de um arquivo `.txt` onde:
* `P`: É o ponto de partida (Player).
* `S`: É a saída (Saída).
* `#` ou paredes: Bloqueios que o explorador não pode atravessar.
* ` `: Espaços vazios por onde ele pode caminhar.

---

## 📂 Organização dos Arquivos

O projeto é dividido em três partes para manter a organização:

* **`labirinto.h` (O Manual):** Contém as "regras" e as descrições de como as estruturas (Pilha, Mochila, Labirinto) devem ser montadas.
* **`labirinto.c` (Os Músculos):** É onde a mágica acontece. Contém o código que realmente move o jogador, carrega o mapa e decide o que fazer com os itens.
* **`main.c` (O Interruptor):** É o ponto de partida. Ele liga o sistema, aponta qual arquivo de mapa ler e inicia a simulação.

---

## 🚀 Como testar?

1.  **Crie um mapa:** Crie um arquivo chamado `mapa.txt` na mesma pasta do código. Exemplo:
    ```text
    ##########
    #P  #   T#
    # # # ## #
    # T   A  #
    # ######S#
    ##########
    ```
2.  **Compile o código:** No terminal, use:
    ```bash
    gcc main.c labirinto.c -o simulador
    ```
3.  **Execute:**
    ```bash
    ./simulador
    ```

Ao final, o programa criará um arquivo chamado `solucao.txt` onde o caminho percorrido será marcado com pontinhos (`.`).

---

## 💡 Curiosidade Técnica
O código foi feito para ser "bilíngue": ele entende se você está rodando no **Windows** ou no **Linux**, ajustando comandos como o de limpar a tela e o tempo de espera (`sleep`) para que a animação da caminhada funcione perfeitamente em qualquer computador!

---
