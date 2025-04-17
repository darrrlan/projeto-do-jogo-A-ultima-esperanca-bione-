# A Última Esperança: Bione

**A Última Esperança: Bione** é um jogo 2D desenvolvido em C utilizando a biblioteca Allegro. O jogador embarca em uma jornada repleta de desafios, onde cada fase exige habilidade, estratégia e determinação.

## 🎮 Sobre o Jogo

O enredo gira em torno de Bione, um(a) herói/heroína em um mundo pós-apocalíptico que precisa enfrentar criaturas e obstáculos para restaurar a esperança da humanidade. Com jogabilidade simples e uma progressão baseada em dificuldade, o jogo oferece uma experiência envolvente com trilha sonora e gráficos personalizados.

## 🛠️ Tecnologias Utilizadas

- **Linguagem**: C
- **Biblioteca gráfica**: [Allegro](https://liballeg.org/)
- **Recursos**:
  - Áudio: arquivos `.ogg`
  - Imagens: sprites `.png`
  - Fontes: `.ttf`
  - Sistema de save/load e rankings por dificuldade

## 📁 Estrutura do Projeto

- `allegro.c`: Arquivo principal do jogo.
- `img/`: Sprites e elementos gráficos.
- `*.ogg`: Efeitos sonoros e música.
- `*.ttf`: Fontes utilizadas na interface.
- `savegame*.txt`: Arquivos de salvamento por dificuldade.
- `ranking*.txt`: Rankings de pontuação por dificuldade.

## 🚀 Como Executar

1. Certifique-se de ter a biblioteca **Allegro 5** instalada.
2. Compile o arquivo `allegro.c`:
   ```bash
   gcc allegro.c -o bione -lallegro -lallegro_audio -lallegro_acodec -lallegro_font -lallegro_ttf -lallegro_image -lallegro_primitives
   ```
3. Execute o jogo:
   ```bash
   ./bione
   ```

> **Nota**: Pode ser necessário ajustar os caminhos dos recursos dentro do código dependendo do ambiente de execução.

## 🏆 Sistema de Ranking

O jogo possui um sistema de ranking separado para as dificuldades: fácil, médio e difícil. Os dados são salvos automaticamente em arquivos `.txt`.

## 💾 Sistema de Salvamento

O progresso do jogador pode ser salvo e carregado, permitindo continuar a jornada em outro momento.

## 🎨 Créditos

- Desenvolvido por [Seu Nome ou Equipe]
- Sons e fontes utilizados são de domínio público ou devidamente licenciados.

## 📄 Licença

Este projeto é de código aberto e pode ser utilizado e modificado.
