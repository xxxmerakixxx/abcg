# Aquário

## Integrantes:
Isabela Lima Sigaki     RA: 11201810447
Julia Oliveira          RA: 11201810014

![build workflow](https://github.com/hbatagelo/abcg/actions/workflows/build.yml/badge.svg)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/hbatagelo/abcg)](https://github.com/hbatagelo/abcg/releases/latest)

Desenvolvimento do Projeto Aquário como atividade para a disciplina [MCTA008-17 Computer Graphics](http://professor.ufabc.edu.br/~harlen.batagelo/cg/) na [UFABC](https://www.ufabc.edu.br/).

[Documentation](https://hbatagelo.github.io/abcg/abcg/doc/html/) | [Release notes](CHANGELOG.md)

O projeto foi criado utilizando o framework ABCg que facilita o desenvolvimento de aplicações baseadas em 
[OpenGL](https://www.opengl.org), [OpenGL ES](https://www.khronos.org), [WebGL](https://www.khronos.org/webgl/), e [Vulkan](https://www.vulkan.org). 

***

## Sobre o projeto
O objetivo do projeto é desenvolver uma aplicação que mostre gráficos 2D com primitivas do OpenGL e permita a interação com o teclado e/ou mouse. 

Desenvolvemos a aplicação de um aquário, nele estão contidos o fundo azul para representar a água, pequenos peixes coloridos que ficam nadando e um peixe principal laranja que o usuário pode controlar por meio das setas do teclado.

O aquário foi desenvolvido baseado no projeto "Asteroides" ensinado na própria disciplina. 
***

## Alterações realizadas

As principais alterações feitas foram:
1. Formato, tamanho, cor da Nave para se tornar um peixe
   Substituímos no array positions os vec2 com pontos desejados para desenhar o peixe e seus respectivos indices para ligar os pontos dos triângulos.  Houve uma alteração na escala do peixe para torná-lo maior, consequentemente o principal na tela. Por fim, utilizando um vec4 mudamos sua cor para um tom de laranja no formato rgb que foi normalizado.
   
2. Adição de movimentos ao peixe principal
  Adicionamos um evento para que o peixe possa se mover e restringimos seus movimentos para ele não sair do campo de visualização
 
3. Substituição dos asteróides (arrays) por Elements para recriar peixes menores secundários que nadam no aquário em várias direções e troca de cores
   Utilizando a mesma base do peixe acima inserimos peixes coloridos que nadam no aquário
   
5. Cor de fundo para simular água
   Mudamos a cor do fundo para azul


## License

ABCg is licensed under the MIT License. See [LICENSE](https://github.com/hbatagelo/abcg/blob/main/LICENSE) for more information.
