# CG_T1
SGI - Sistema Gráfico Interativo
===================================== Part 1.2 ====================================
Suportar as 3 transformações básicas e a rotação arbitrária em 2D. 

--- Model ---
Para tanto você vai criar uma rotina de transformação genérica, que aceita uma matriz de transformação em coordenadas homogêneas e um objeto qualquer para ser transformado e devolve este objeto após a aplicação da matriz. Esta rotina nada mais é do que uma forma extremamente simples de se implementar um engine gráfico. Para alimentar esta rotina você deve criar um conjunto de rotinas de “preparo” da matriz de transformação, que serão específicas para cada tranformação.

--- GUI - View ---
Para poder aplicar uma transformação sobre um determinado objeto do mundo, você deve permitir ao usuário que selecione um dos objetos de seu mundo na lista de objetos, escolha a transformação que deseja aplicar e entre com os dados para esta transformação em uma interface para isso.

--- Math/Algorithms ---
Implementar 5 transformações:

-Translações
-Escalonamentos em torno do centro do objeto
Rotações:
-Em torno do centro do mundo
-Em torno do centro do objeto
-Em torno de um ponto qualquer (arbitrário)‏
Na translação você simplesmente calcula a matriz no sistema de coordenadas homogêneo e aplica.

Para o escalonamento e para a rotação você vai precisar determinar o centro geométrico ou centro de massa do objeto a ser escalonado ou rotacionado. No caso da rotação nós já discutimos a razão para tanto: a rotação arbitrária que nos parece natural, é aquela onde um objeto roda em torno de seu centro. No caso do escalonamento temos a mesma situação: o escalonamento somente parece natural se o objeto parece “encolher” ou “inchar”.
============================ Part 1.2 ===============================
