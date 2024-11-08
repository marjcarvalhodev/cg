# Simulação de Laboratório Químico em Computação Gráfica

Este projeto implementa uma simulação visual de um laboratório químico usando técnicas de computação gráfica e a biblioteca ImGui. O objetivo é permitir a interação com elementos químicos em uma interface gráfica, onde o usuário pode combinar elementos e observar as reações químicas representadas de forma visual e interativa.

## Sumário

- [Descrição do Projeto](#descrição-do-projeto)
- [Funcionalidades](#funcionalidades)
- [Estrutura do Código](#estrutura-do-código)
- [Técnicas Implementadas](#técnicas-implementadas)
- [Como Executar](#como-executar)
- [Dependências](#dependências)
- [Licença](#licença)

## Descrição do Projeto

A aplicação cria uma interface gráfica com diversos elementos químicos disponíveis para seleção. Os elementos podem ser adicionados a um béquer de mistura, onde, dependendo das combinações selecionadas, reações químicas específicas ocorrem. As reações resultam na exibição de novas substâncias e cores, simulando uma resposta visual ao processo de combinação de substâncias.

## Funcionalidades

- _Seleção de Elementos Químicos_: Cada elemento é representado como um botão em formato de erlenmeyer com uma cor específica.
- _Mistura de Elementos_: Ao selecionar dois elementos, eles são adicionados ao béquer, onde é possível observar suas cores.
- _Reação Química_: Combinações válidas de elementos produzem uma reação com um novo composto e uma nova cor.
- _Reset de Mistura_: Limpa o conteúdo do béquer para novas combinações.

## Estrutura do Código

- _main.cpp_: Ponto de entrada da aplicação, onde o ambiente do OpenGL é configurado e a janela principal é criada.
- _window.hpp_ e _window.cpp_: Implementam a lógica da interface gráfica e do controle das reações, incluindo:
  - Definição dos elementos químicos.
  - Carregamento e exibição dos botões de elementos.
  - Manipulação do béquer e execução de reações.
  - Verificação e atualização do estado do béquer com base nas combinações de elementos.

## Técnicas Implementadas

### 1. _Renderização de Formas Customizadas_

- _Representação Visual dos Elementos_: Cada elemento químico é representado como um botão em formato de erlenmeyer, feito por meio de uma combinação de triângulos e quadrados desenhados diretamente usando o ImGui.
- _Béquer de Mistura_: O recipiente onde ocorre a mistura dos elementos também é desenhado usando as primitivas gráficas de ImGui para representar um béquer com um contorno preenchido pela cor da reação resultante.

### 2. _Interação por Botões_

- _Botões Interativos_: Os botões dos elementos são projetados para detectar cliques, adicionando os elementos ao recipiente de mistura.
- _Reset da Mistura_: Um botão separado foi adicionado para limpar os elementos do béquer, permitindo reiniciar a combinação de substâncias.

### 3. _Controle de Estado e Reações Químicas_

- _Estrutura de Dados para Elementos e Reações_:
  - std::vector armazena os elementos disponíveis, cada um com um símbolo, nome e cor visual.
  - std::unordered_map armazena reações químicas, mapeando pares de elementos para os resultados das reações.
- _Verificação de Reação_: Ao adicionar dois elementos, a função checkReaction consulta o unordered_map para verificar se a combinação é válida. Se for, o sistema exibe o composto resultante e uma nova cor.

### 4. _Cores e Representação Gráfica de Reações_

- _Cores_: Cada elemento e reação é representado com uma cor específica, usando ImVec4. O resultado da reação é exibido visualmente ao usuário com uma coloração específica.
- _Texto e Nome das Reações_: Quando uma reação ocorre, o nome do composto resultante é exibido dentro do béquer, junto à sua cor.

## Como Executar

1. Clone o repositório:
   ```bash
   git clone https://github.com/usuario/simulacao-laboratorio-quimico.git
   Instale as dependências (veja a seção Dependências).
   ```

Compile o projeto:

bash
Copiar código
mkdir build
cd build
cmake ..
make
Execute a aplicação:

bash
Copiar código
./SimulacaoLaboratorio
Dependências
OpenGL: Necessário para renderização gráfica.
ImGui: Biblioteca de interface gráfica em C++.
GSL (Guideline Support Library): Utilizado para conversões seguras e outras funcionalidades.
