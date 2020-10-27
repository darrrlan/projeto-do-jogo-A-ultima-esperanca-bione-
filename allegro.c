#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//VARIÁVEIS GLOBAIS//
ALLEGRO_BITMAP *menu_fundo,*botao_sair,*botao_credito,*credito,*voltar_menu, *ponto,*fundo_tutorial,*folha_sprite,*fundo_do_jogo,*folha_sprite_zombie,*chao,*fundo_novo = NULL;
ALLEGRO_BITMAP *fundo_novob, *arvore, *menu_pause,*vida_do_personagem,*gameover1,*config, *folha_sprite_humano = NULL;
ALLEGRO_FONT *fonte = NULL;//variavel da fonte
ALLEGRO_DISPLAY *janela= NULL;//varíavel da janela principal
ALLEGRO_EVENT_QUEUE *fila_eventos,*fila_eventos_mouse,*fila_eventos_keyboard= NULL, *fila_eventos2;//variavel de fila de eventos
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_AUDIO_STREAM *musica = NULL;
ALLEGRO_SAMPLE *pulo = NULL;


int estado = 0;
int desenha = 1;
int sair = 0;
int tecla = 0;
int tecla_direcao[4]={0,0,0,0}; //vetor que guarda quando cada uma das 4 direcoes foi pressionada
int X=0;
int continua = 0;
int nivel_de_dificuldade;
char str[25];
int inicia_tutorial = 0;
int som = 1;
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// MOVIMENTOS PERSONAGEM //
enum movimento {PARADO,MOVIMENTO_PARA_DIREITA,MOVIMETO_PARA_ESQUERDA,MOVIMENTO_PARA_DIREITA_FUNDO,ATAQUE};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//STRUCT SPRITE//
typedef struct{
    int largura_sprite, altura_sprite;
    int colunas_folhas, coluna_atual;
    int linha_atual, linhas_folhas;
    int regiao_x_folha, regiao_y_folha;
    int frames_sprite, cont_frames;
    int inverte_sprite, tempo_salto;
    int pos_x_sprite, pos_y_sprite;
    float vel_x_sprite;
    int vel_y_sprite;
    int  pulo;
}Sprite;//struct com as variaveis
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//FUNÇÃO PARA PASSAR OS PARAMETROS DO PERSONAGEM PARA UMA STRUCT

Sprite cerrega_sprite_personagem(){
  Sprite personagem;
    personagem.largura_sprite = 128;
    personagem.altura_sprite = 201;
    personagem.colunas_folhas = 10;
    personagem.coluna_atual = 0;
    personagem.linha_atual = 0;
    personagem.linhas_folhas = 3;
    personagem.regiao_x_folha = 0;
    personagem.regiao_y_folha = 0;
    personagem.frames_sprite = 6;
    personagem.cont_frames = 0;
    personagem.inverte_sprite = 0;
    personagem.pos_x_sprite = 100;
    personagem.pos_y_sprite = 485;
    personagem.vel_x_sprite = 4;
    personagem.vel_y_sprite = 10;
    personagem.tempo_salto = -1;
    personagem.pulo = 0;

    return personagem;//retorna o personagem
}//CARREGA_SPRITE_PERSONAGEM
Sprite cerrega_sprite_zombie(int pos_x){

  Sprite zombie;
    zombie.largura_sprite = 128;
    zombie.altura_sprite = 173;
    zombie.colunas_folhas = 10;
    zombie.coluna_atual = 0;
    zombie.linha_atual = 0;
    zombie.linhas_folhas = 1;
    zombie.regiao_x_folha = 0;
    zombie.regiao_y_folha = 0;
    zombie.frames_sprite = 10;
    zombie.cont_frames = 0;
    zombie.inverte_sprite = 1;
    zombie.pos_x_sprite = 1280+pos_x;
    zombie.pos_y_sprite = 485;
    zombie.vel_x_sprite = 6;
    zombie.vel_y_sprite = 10;
    zombie.tempo_salto = -1;
    zombie.pulo = 0;

    return zombie;//retorna o personagem
}//CARREGA_SPRITE_PERSONAGEM
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//STRUCT PARA SALVAR
typedef struct{

  int dificuldade;
  int pos_x_save;
  int pos_y_save;
  int pont_atual;
  int pos_x_savez;
  int pos_y_savez;
  int numero_de_vidas;
  float vida;

}Salvando;

//STRUCT JOGADOR//
typedef struct{

  char nome[25];
  int pontuacao;

}Jogador;

//FUNÇÃO PARA CRIPTOGRAFAR O NOME DO JOGADOR//
void criptografa(char* str){

  while (*str != '\0') {
    *str = *str + 2;
    str++;
  }//while

}//criptografa

//FUNÇÃO PARA DESCRIPTOGRAFAR O NOME DO JOGADOR//
void descriptografa(char* msg){

  while (*msg != '\0') {
    *msg = *msg - 2;
    msg++;
  }
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//CONSTANTES//

//DIMENSÕES DA TELA//
#define ALTURA 720
#define LARGURA 1280

//ESTADOS DO MENU//
#define ESTADO_MENU 0
#define ESTADO_CREDITO 1
#define ESTADO_PONTUACAO 2
#define ESTADO_TUTORIAL 3
#define ESTADO_DIFICULDADE 4
#define ESTADO_INICIA_O_JOGO 5
#define ESTADO_FIM 6
#define ESTADO_CONFIGURACAO 7

//TAXA DE ATUALIZAÇÃO//
#define FPS 60.0
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//FUNÇÃO PARA MOSTRAR MENSAGEM DE ERRO//
void error_msg(char *text){
  al_show_native_message_box(NULL,"ERRO",
  "Ocorreu o seguinte erro e o programa sera finalizado:",
  text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}//ERROR_MSG
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

void entrada_do_nome(ALLEGRO_EVENT evento);
void exibe_a_string();

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//FUNÇÃO DESTRUIR//
void destruir()
{
  //DESTROI JANELAS//
  if(janela)
    al_destroy_display(janela);

  //DESTROI FILA EVENTOS//
  if(fila_eventos)
    al_destroy_event_queue(fila_eventos);

  //DESTROI FILA EVENTOS DO MOUSE//
  if(fila_eventos_mouse)
    al_destroy_event_queue(fila_eventos_mouse);

  //DESTROI IMAGEM DE FUNDO DO MENU//
  if(menu_fundo)
    al_destroy_bitmap(menu_fundo);

  if(menu_pause)
      al_destroy_bitmap(menu_pause);

  //DESTROI O BOTAO_SAIR//
  if(botao_sair)
    al_destroy_bitmap(botao_sair);

  if(fundo_do_jogo)
    al_destroy_bitmap(fundo_do_jogo);

  //DESTROI O BOTAO_CREDITO//
  if(botao_credito)
    al_destroy_bitmap(botao_credito);

  //DESTROI CREDITO//
  if(credito)
    al_destroy_bitmap(credito);

  //DESTROI VOLTAR_MENU
  if(voltar_menu)
    al_destroy_bitmap(voltar_menu);

  //DESTROI PONTO//
  if(ponto)
    al_destroy_bitmap(ponto);

  //DESTROI O FUNDO_TUTORIAL//
  if(fundo_tutorial)
    al_destroy_bitmap(fundo_tutorial);

  //DESTROI AS FONTES//
  if(fonte)
    al_destroy_font(fonte);

  //DESTROI O TIMER//
  if(timer)
    al_destroy_timer(timer);

  }//DESTRUIR
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//FUNÇÃO QUE INICIALIZA ADDONS//
int inicializar()
{
  //INICIA A ALLEGRO5//
  al_init();
    if (!al_init()){
      error_msg("Falha ao inicializar a Allegro");
      return -1;
  }//INICIANDO ALLEGRO//


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //INICIA ADDON DE AUDIO//
  al_install_audio();
    if(!al_install_audio()){
      error_msg("Falha ao inicializar audio");
      return -1;
  }//ADDON AUDIO//

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //ADDON DE SUPORTE PARA AUDIO//
    if(!al_init_acodec_addon()){
        error_msg("Falha ao inicializar o codec de audio!");
        return 0;
  }//IF !ADDON SUPORTE

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //CRIA O MIXER E ADICIONA 5 SAMPLES DE AUDIO NELE//
    if (!al_reserve_samples(5)){
        error_msg("Falha ao reservar amostrar de audio!");
        return 0;
  }//IF !MIXER

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //CARREGA A MÚSICA//
   musica = al_load_audio_stream("music.ogg", 4, 1024);
    if (!musica){
        error_msg( "Audio nao carregado!");
        return 0;
  }//MUSICA

  //CARREGA EFEITOS SONOROS

  //EFEITO SONORO - PULO
   pulo = al_load_sample("pulo.ogg");
    if(!pulo){
        error_msg("Efeito Sonoro - PULO - Não Carregado!");
        return 0;
    }//EFEITO SONORO PULOs

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //CRIA O TIMER//
   timer= al_create_timer(1.0/FPS);
    if(!timer){
        error_msg("Falha ao criar o timer");
        return 0;
  }//TIMER



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //INICA ADDON DE IMAGEM//
  al_init_image_addon();
        if (!al_init_image_addon()){
          error_msg("Falha ao inicializar add-on allegro_image");
          return -1;
  }//ADDON IMAGEM

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //INICIA ADDON DE FONTE//
  al_init_font_addon();
        if (!al_init_ttf_addon()){
          error_msg("Falha ao inicializar add-on allegro_ttf");
          return -1;
  }//ADDON FONTE

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //INSTALA TECLADO//
        if (!al_install_keyboard()){
          error_msg("Falha ao inicializar o teclado");
          return 0;
    }//INSTALA TECLADO//

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //INSTALA MOUSE//
        if (!al_install_mouse()){
          error_msg("Falha ao inicializar o mouse");
          al_destroy_display(janela);
          return -1;
    }//INSTALA MOUSE


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //CARREGA A IMAGEM DO CHÃO//
    chao = al_load_bitmap("img/chao.png");
        if (!chao){
          error_msg("Falha ao carregar chao");
          return -1;
    }//IMAGEM CHÃO

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //CRIA FILA DE EVENTOS DE JANELA//
  fila_eventos=al_create_event_queue();
        if (!fila_eventos){
          error_msg("Falha ao criar fila de eventos");
          al_destroy_display(janela);
          return -1;
  }//FILA DE EVENTOS JANELA

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //CRIA FILA DE EVENTOS DE TECLADO//
  fila_eventos_keyboard = al_create_event_queue();
        if (!fila_eventos){
          error_msg("Falha ao criar fila de eventos");
          al_destroy_display(janela);
          return 0;
    }//FILA DE EVENTOS TECLADO//

    folha_sprite_zombie = al_load_bitmap("sprites/Zubisao 173,128.png");
          if (!folha_sprite_zombie){
            error_msg("Falha ao carregar sprites");
            al_destroy_timer(timer);
            al_destroy_display(janela);
            al_destroy_event_queue(fila_eventos);
        return 0;
    }

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //CRIA FILA DE EVENTOS DE MOUSE//
  fila_eventos_mouse=al_create_event_queue();
        if (!fila_eventos_mouse){
          error_msg("Falha ao criar fila de eventos");
          al_destroy_display(janela);
          return -1;
  }//FILA DE EVENTOS MOUSE//

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //CRIA A JANELA//
  janela = al_create_display(LARGURA, ALTURA);
        if (!janela){
          error_msg("Falha ao criar janela");
          return -1;
  }//CRIAÇÃO JANELA//

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //ATRIBUI O CURSOR PADRÃO PARA O MOUSE//
        if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT)){
          error_msg("Falha ao atribuir ponteiro do mouse");
          al_destroy_display(janela);
          return -1;
  }//CURSOR MOUSE//

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //CARREGA FOLHA DE SPRITES//
  folha_sprite = al_load_bitmap("sprites/FolhaSprite.png");
        if (!folha_sprite){
          error_msg("Falha ao carregar sprites");
          al_destroy_timer(timer);
          al_destroy_display(janela);
          al_destroy_event_queue(fila_eventos);
      return 0;
  }//FOLHA SPRITE//

  //CARREGA O FUNDO DO JOGO
  fundo_novo = al_load_bitmap("img/novofundo.png");
  if (!fundo_novo){
    error_msg("Falha ao carregar fudo_novo");
return 1;
}

//CARREGA O FUNDO DO JOGO
  fundo_novob = al_load_bitmap("img/Fundob.png");
  if (!fundo_novob){
    error_msg("Falha ao carregar fundo_novob");
return 1;
}

// CARREGA IMAGEM ARVORE
arvore = al_load_bitmap("img/Árvore.png");
if (!arvore){
  error_msg("Falha ao carregar arvore");
return 1;
}

//CARREGA O SPRITE DA VIDA DO PERSONAGEM
vida_do_personagem = al_load_bitmap("img/1.png");
if (!vida_do_personagem){
  error_msg("Falha ao carregar vida");
return 1;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //FILA DE EVENTOS//

    //FILA DE EVENTOS MOUSE
    al_register_event_source(fila_eventos_mouse, al_get_mouse_event_source());

    //FILA DE EVENTOS TECLADO
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());

    //FILA DE EVENTOS JANELA
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));

    //FILA DE EVENTOS TIMER
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));

    //INICIA TIMER
    al_start_timer(timer);
    return 0;
}//FUNÇÃO INICIALIZAR//

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//FUNÇÃO MENU//
int menu(){

  //DEFINE O TÍTULO DA JANELA
  al_set_window_title(janela, "A última esperança - Bione");

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //CARREGA A IMAGEM DE FUNDO DO MENU
  menu_fundo = al_load_bitmap("img/Menus.jpg");

  //CRIA BOTÃO SAIR
  botao_sair = al_create_bitmap(130 ,70);

  //CRIA BOTÃO CREDITO
  botao_credito = al_create_bitmap(130 ,70);

  al_draw_bitmap(menu_fundo, 0, 0, 0);

  //CARREGA A FONTE DO MENU
  fonte = al_load_font("stranger.ttf", 88, 0);

  //ESCREVE OS BOTÕES
  al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 2, 310, ALLEGRO_ALIGN_CENTRE, "Iniciar");
  al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 2, 380, ALLEGRO_ALIGN_CENTRE, "Configuração");
  al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 2,  450, ALLEGRO_ALIGN_CENTRE, "Pontuação");
  al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 2, 520 ,ALLEGRO_ALIGN_CENTRE, "Créditos");
  al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 2, 600, ALLEGRO_ALIGN_CENTRE, "Sair");

  //ATUALIZA A JANELA
  al_flip_display();

//LAÇO DE REPETIÇÃO DO MENU
  while(!sair){

    //LAÇO DE REPETIÇÃO PARA FECHAR COM O 'X' DA JANELA
       while(!al_is_event_queue_empty(fila_eventos)){
          ALLEGRO_EVENT ev;
          al_wait_for_event(fila_eventos, &ev);

         //SE O EVENTO FOR O 'X' FECHA A JANELA
        if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                sair =1;
                estado= ESTADO_FIM;
            }//EVENTO 'X'
       }// WHILE - FECHANDO COM O 'X' DA JANELA

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //DEFININDO CLICK DO MOUSE NOS BOTÕES DO MENU//
    while (!al_is_event_queue_empty(fila_eventos_mouse)){

      //DEFININDO NOME DO EVENTO
      ALLEGRO_EVENT evento;

      //GRAVANDO EVENTOS NA FILA DE EVENTOS DO MOUSE
        al_wait_for_event(fila_eventos_mouse, &evento);
        if(som==0){
          printf("%i\n",som );
          al_attach_audio_stream_to_mixer(musica, al_get_default_mixer());
          al_set_audio_stream_playmode(musica, ALLEGRO_PLAYMODE_LOOP);
        }else{

        }

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //BOTAO SAIR//
      //CASO EVENTO FOR O CLICK DO MOUSE
        if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

       //DEFININDO HITBOX DO BOTÃO SAIR E CASO CLICAR NO BOTÃO, SAIR DO JOGO
        if (evento.mouse.x >= LARGURA/ 2 - al_get_bitmap_width(botao_sair) / 2 &&
              evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(botao_sair) / 2 &&
              evento.mouse.y >= 640 - al_get_bitmap_height(botao_sair) / 2 &&
              evento.mouse.y <= 610 + al_get_bitmap_height(botao_sair) / 2){
                    sair = 1;
                    estado= ESTADO_FIM;
        }//IF DA AÇÃO CASO CLICAR NO BOTAO SAIR
      }//IF DA HITBOX (BOTAO SAIR)//

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //BOTAO CRÉDITOS//
      //CASO EVENTO FOR O CLICK DO MOUSE
        if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

        //DEFININDO HITBOX DO BOTÃO CRÉDITOS E CASO CLICAR, ABRIR CRÉDITOS
        if (evento.mouse.x >= LARGURA/ 2 - al_get_bitmap_width(botao_sair) / 2 &&
              evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(botao_sair) / 2 &&
              evento.mouse.y >= 570 - al_get_bitmap_height(botao_sair) / 2 &&
              evento.mouse.y <= 530 + al_get_bitmap_height(botao_sair) / 2){
                    sair = 1;
                    estado= ESTADO_CREDITO;
        }//IF DA AÇÃO CASO CLICAR NO BOTÃO CRÉDITOS
      }//IF DA HITBOX (BOTÃO CRÉDITOS)

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //BOTÃO PONTUAÇÃO//
      //CASO EVENTO FOR O CLICK DO MOUSE
        if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

        //DEFININDO HITBOX DO BOTÃO PONTUAÇÃO E CASO CLICAR, MOSTRAR PONTUAÇÃO
        if (evento.mouse.x >= LARGURA/ 2 - al_get_bitmap_width(botao_sair) / 2 &&
              evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(botao_sair) / 2 &&
              evento.mouse.y >= 490 - al_get_bitmap_height(botao_sair) / 2 &&
              evento.mouse.y <= 470 + al_get_bitmap_height(botao_sair) / 2){
                    sair = 1;
                    estado = ESTADO_PONTUACAO;
       }// IF DA AÇÃO CASO CLICAR NO BOTÃO PONTUAÇÃO
     }//IF DA HITBOX (BOTÃO PONTUAÇÃO)

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //BOTÃO TUTORIAL//
      //CASO EVENTO FOR O CLICK DO MOUSE
        if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

        //DEFININDO HITBOX DO BOTÃO TUTORIAL E CASO CLICAR, INFORMA TUTORIAL
        if (evento.mouse.x >= LARGURA/ 2 - al_get_bitmap_width(botao_sair) / 2 &&
              evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(botao_sair) / 2 &&
              evento.mouse.y >= 430 - al_get_bitmap_height(botao_sair) / 2 &&
              evento.mouse.y <= 400 + al_get_bitmap_height(botao_sair) / 2){
                    sair = 1;
                    estado =ESTADO_CONFIGURACAO;
      }//IF DA AÇÃO CASO CLICAR NO BOTÃO TUTORIAL
    }//IF DA HITBOX (BOTÃO TUTORIAL)

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //BOTÃO INICIAR//
    //CASO EVENTO FOR O CLICK DO MOUSE
      if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

      //DEFININDO HITBOX DO BOTÃO INICIAR E CASO CLICAR, INICIA O JOGO
      if (evento.mouse.x >= LARGURA/ 2 - al_get_bitmap_width(botao_sair) / 2 &&
            evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(botao_sair) / 2 &&
            evento.mouse.y >= 340 - al_get_bitmap_height(botao_sair) / 2 &&
            evento.mouse.y <= 330 + al_get_bitmap_height(botao_sair) / 2){
                  sair = 1;
                  estado = ESTADO_DIFICULDADE;
        }//IF DA AÇÃO CASO CLICAR NO BOTÃO INCIAR
      }//IF DA HITBOX (BOTÃO INCIAR)

    }//FECHANDO WHILE DE EVENTOS DO MOUSE
  }//LAÇO DE REPETIÇÃO DO MENU

  sair--;//decrementando para while voltar a funcionar
  return 0;
}//FECHANDO FUNÇÃO MENU
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//FUNÇÃO DAS DIFICULDADES DO JOGO
int dificuldade(){

    al_draw_bitmap(menu_fundo, 0, 0, 0);

    //CARREGA A FONTE DO MENU
    fonte = al_load_font("stranger.ttf", 88, 0);

    //ESCREVE OS BOTÕES
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 2, 310, ALLEGRO_ALIGN_CENTRE, "Tutorial");
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 2, 380, ALLEGRO_ALIGN_CENTRE, "Facíl");
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 2, 450, ALLEGRO_ALIGN_CENTRE, "Médio");
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 2, 520 ,ALLEGRO_ALIGN_CENTRE, "Difícil");
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 2, 600, ALLEGRO_ALIGN_CENTRE, "voltar");

    //ATUALIZA A JANELA
    al_flip_display();

  //LAÇO DE REPETIÇÃO DO MENU
    while(!sair){

      //LAÇO DE REPETIÇÃO PARA FECHAR COM O 'X' DA JANELA
         while(!al_is_event_queue_empty(fila_eventos)){
            ALLEGRO_EVENT ev;
            al_wait_for_event(fila_eventos, &ev);

           //SE O EVENTO FOR O 'X' FECHA A JANELA
          if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                  sair =1;
                  estado = ESTADO_FIM;
              }//EVENTO 'X'
         }// WHILE - FECHANDO COM O 'X' DA JANELA

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    //DEFININDO CLICK DO MOUSE NOS BOTÕES DO MENU//
      while (!al_is_event_queue_empty(fila_eventos_mouse)){

        //DEFININDO NOME DO EVENTO
        ALLEGRO_EVENT evento;

        //GRAVANDO EVENTOS NA FILA DE EVENTOS DO MOUSE
          al_wait_for_event(fila_eventos_mouse, &evento);

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    //BOTAO SAIR//
        //CASO EVENTO FOR O CLICK DO MOUSE
          if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

         //DEFININDO HITBOX DO BOTÃO SAIR E CASO CLICAR NO BOTÃO, SAIR DO JOGO
          if (evento.mouse.x >= LARGURA/ 2 - al_get_bitmap_width(botao_sair) / 2 &&
                evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(botao_sair) / 2 &&
                evento.mouse.y >= 640 - al_get_bitmap_height(botao_sair) / 2 &&
                evento.mouse.y <= 610 + al_get_bitmap_height(botao_sair) / 2){
                      sair = 1;
                      estado = ESTADO_MENU;
          }//IF DA AÇÃO CASO CLICAR NO BOTAO SAIR
        }//IF DA HITBOX (BOTAO SAIR)//

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    //BOTAO DIFICIL//
        //CASO EVENTO FOR O CLICK DO MOUSE
          if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

          //DEFININDO HITBOX DO BOTÃO DIFICIL
          if (evento.mouse.x >= LARGURA/ 2 - al_get_bitmap_width(botao_sair) / 2 &&
                evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(botao_sair) / 2 &&
                evento.mouse.y >= 570 - al_get_bitmap_height(botao_sair) / 2 &&
                evento.mouse.y <= 530 + al_get_bitmap_height(botao_sair) / 2){
                      sair = 1;
                      nivel_de_dificuldade = 3;
                      estado = ESTADO_INICIA_O_JOGO;
          }//IF DA AÇÃO CASO CLICAR NO BOTÃO DIFICIL
        }//IF DA HITBOX (BOTÃO DIFICIL)

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    //BOTÃO MEDIO//
        //CASO EVENTO FOR O CLICK DO MOUSE
          if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

          //DEFININDO HITBOX DO BOTÃO MEDIO
          if (evento.mouse.x >= LARGURA/ 2 - al_get_bitmap_width(botao_sair) / 2 &&
                evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(botao_sair) / 2 &&
                evento.mouse.y >= 490 - al_get_bitmap_height(botao_sair) / 2 &&
                evento.mouse.y <= 470 + al_get_bitmap_height(botao_sair) / 2){
                      sair = 1;
                      nivel_de_dificuldade = 2;
                      estado = ESTADO_INICIA_O_JOGO;
         }// IF DA AÇÃO CASO CLICAR NO BOTÃO MEDIO
       }//IF DA HITBOX (BOTÃO MEDIO)

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    //BOTÃO FACIL//
        //CASO EVENTO FOR O CLICK DO MOUSE
          if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

          //DEFININDO HITBOX DO BOTÃO FACIL
          if (evento.mouse.x >= LARGURA/ 2 - al_get_bitmap_width(botao_sair) / 2 &&
                evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(botao_sair) / 2 &&
                evento.mouse.y >= 430 - al_get_bitmap_height(botao_sair) / 2 &&
                evento.mouse.y <= 400 + al_get_bitmap_height(botao_sair) / 2){
                      sair = 1;
                      nivel_de_dificuldade = 1;
                      estado = ESTADO_INICIA_O_JOGO;
        }//IF DA AÇÃO CASO CLICAR NO BOTÃO FACIL
      }//IF DA HITBOX (BOTÃO FACIL)
      if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

      //DEFININDO HITBOX DO BOTÃO FACIL
      if (evento.mouse.x >= LARGURA/ 2 - al_get_bitmap_width(botao_sair) / 2 &&
            evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(botao_sair) / 2 &&
            evento.mouse.y >= 360 - al_get_bitmap_height(botao_sair) / 2 &&
            evento.mouse.y <= 330 + al_get_bitmap_height(botao_sair) / 2){
                  sair = 1;
                  estado = ESTADO_TUTORIAL;
    }//IF DA AÇÃO CASO CLICAR NO BOTÃO FACIL
  }//IF DA HITBOX (BOTÃO FACIL)
  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
      }//FECHANDO WHILE DE EVENTOS DO MOUSE
    }//LAÇO DE REPETIÇÃO DO MENU

    sair--;//decrementando para while voltar a funcionar
    return 0;
  }//FECHANDO FUNÇÃO DIFICULDADES
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//FUNÇÃO CRÉDITOS//
int creditos(){

  //CARREGA IMAGEM DE FUNDO DOS CRÉDITOS//
    credito = al_load_bitmap("img/Créditos.png");

  al_draw_bitmap(credito, 0, 0, 0);

  fonte = al_load_font("arial.ttf", 48, 0);//FONTE

  //BOTÃO VOLTAR//
  al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 2, 550, ALLEGRO_ALIGN_CENTRE, "voltar");

  //ATUALIZA A JANELA
  al_flip_display();

  //CRIA O BITMAP DO BOTAO VOLTAR//
  voltar_menu = al_create_bitmap(130 ,70);

  //LAÇO DE REPETIÇÃO PARA O BOTÃO VOLTAR DA TELA DE CREDITOS//
    while(!sair){

  //ALOCANDO EVENTO NA FILA DE EVENTOS DO MOUSE
    while (!al_is_event_queue_empty(fila_eventos_mouse)){
      ALLEGRO_EVENT evento;
      al_wait_for_event(fila_eventos_mouse, &evento);

      //CASO O EVENTO FOR O CLICK DO MOUSE
          if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

      //DEFININDO HITBOX DO BOTÃO VOLTAR DA TELA DE CRÉDITOS
          if (evento.mouse.x >= LARGURA/ 2 - al_get_bitmap_width(voltar_menu) / 2 &&
                evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(voltar_menu) / 2 &&
                evento.mouse.y >= 590 - al_get_bitmap_height(voltar_menu) / 2 &&
                evento.mouse.y <= 560 + al_get_bitmap_height(voltar_menu) / 2){
                      sair = 1;
                      estado= ESTADO_MENU;//para voltar para o menu
        }//IF DA AÇÃO CASO CLICAR NO BOTÃO VOLTAR
      }//IF DA HITBOX DO BOTÃO VOLTAR
    }//FIM DO LAÇO DE REPETIÇÃO
  }//FIM DO LAÇO DE REPETIÇÃO PARA O BOTÃO VOLTAR DA TELA DE CREDITOS

   sair--;//DECREMENTANDO SAIR PARA O WHILE RODAR
   return 0;
}//FUNÇÃO CRÉDITOS//

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//FUNÇÃO PONTOS//
int pontos(){

  Jogador jogador;
  FILE* salvar;
  int H = 325;

  //CARREGA IMAGEM DO SCORE
    ponto = al_load_bitmap("img/SCORE.jpg");
      al_draw_bitmap(ponto, 0, 0, 0);

    fonte = al_load_font("stranger.ttf", 85, 0);//FONTE

  //ESCREVE BOTÕES NA TELA SCORE
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 2, 380, ALLEGRO_ALIGN_CENTRE, "Facíl");
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 2, 450, ALLEGRO_ALIGN_CENTRE, "Médio");
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 2, 520 ,ALLEGRO_ALIGN_CENTRE, "Difícil");
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 2, 600, ALLEGRO_ALIGN_CENTRE, "Voltar");

  //ATUALIZA JANELA
    al_flip_display();

  //CRIA O BITMAP DO BOTÃO VOLTAR
  voltar_menu = al_create_bitmap(130 ,70);

  while(!sair){

    //ALOCA EVENTO NA  FILA DE EVENTOS DO MOUSE
      while (!al_is_event_queue_empty(fila_eventos_mouse)){
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos_mouse, &evento);

    //CASO O EVENTO FOR O CLICK DO MOUSE
        if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

    //DEFININDO HITBOX DO BOTÃO VOLTAR DA TELA SCORE
        if (evento.mouse.x >= LARGURA/ 2 - al_get_bitmap_width(voltar_menu) / 2 &&
              evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(voltar_menu) / 2 &&
              evento.mouse.y >= 640 - al_get_bitmap_height(voltar_menu) / 2 &&
              evento.mouse.y <= 610 + al_get_bitmap_height(voltar_menu) / 2){
                    sair = 1;
                    estado = ESTADO_MENU;
        }//IF DA AÇÃO CASO CLICAR NO BOTÃO VOLTAR
      }//IF DA HITBOX DO BOTÃO VOLTAR
      //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //BOTAO DIFICIL//
      //CASO EVENTO FOR O CLICK DO MOUSE
        if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

        //DEFININDO HITBOX DO BOTÃO DIFICIL
        if (evento.mouse.x >= LARGURA/ 2 - al_get_bitmap_width(botao_sair) / 2 &&
              evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(botao_sair) / 2 &&
              evento.mouse.y >= 570 - al_get_bitmap_height(botao_sair) / 2 &&
              evento.mouse.y <= 530 + al_get_bitmap_height(botao_sair) / 2){
                    nivel_de_dificuldade = 3;
        }//IF DA AÇÃO CASO CLICAR NO BOTÃO DIFICIL
      }//IF DA HITBOX (BOTÃO DIFICIL)

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //BOTÃO MEDIO//
      //CASO EVENTO FOR O CLICK DO MOUSE
        if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

        //DEFININDO HITBOX DO BOTÃO MEDIO
        if (evento.mouse.x >= LARGURA/ 2 - al_get_bitmap_width(botao_sair) / 2 &&
              evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(botao_sair) / 2 &&
              evento.mouse.y >= 490 - al_get_bitmap_height(botao_sair) / 2 &&
              evento.mouse.y <= 470 + al_get_bitmap_height(botao_sair) / 2){
                    nivel_de_dificuldade = 2;
       }// IF DA AÇÃO CASO CLICAR NO BOTÃO MEDIO
     }//IF DA HITBOX (BOTÃO MEDIO)

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //BOTÃO FACIL//
      //CASO EVENTO FOR O CLICK DO MOUSE
        if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

        //DEFININDO HITBOX DO BOTÃO FACIL
        if (evento.mouse.x >= LARGURA/ 2 - al_get_bitmap_width(botao_sair) / 2 &&
              evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(botao_sair) / 2 &&
              evento.mouse.y >= 430 - al_get_bitmap_height(botao_sair) / 2 &&
              evento.mouse.y <= 400 + al_get_bitmap_height(botao_sair) / 2){
                    nivel_de_dificuldade = 1;
        }//IF DA AÇÃO CASO CLICAR NO BOTÃO FACIL
      }//IF DA HITBOX (BOTÃO FACIL)

      if (nivel_de_dificuldade != 0) {
        ponto = al_load_bitmap("img/SCORE.jpg");
          al_draw_bitmap(ponto, 0, 0, 0);

        fonte = al_load_font("stranger.ttf", 85, 0);//FONTE

        //ESCREVE BOTÕES NA TELA SCORE
        al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 2, 600, ALLEGRO_ALIGN_CENTRE, "Voltar");

        //CRIA O BITMAP DO BOTÃO VOLTAR
        voltar_menu = al_create_bitmap(130 ,70);

        //MOSTRANDO O RANKING DE ACORDO COM A DIFICULDADE
        if (nivel_de_dificuldade == 3) {//CARREGA O RANKING DIFICIL
          if ((salvar = fopen("rankingdificil.txt", "rb")) != NULL){// Abre o arquivo e verifica se não ocorreu um erro.
            continua++;
          }
        }else if(nivel_de_dificuldade == 2){//CARREGA O RANKING MEDIO
          if ((salvar = fopen("rankingmedio.txt", "rb")) != NULL){// Abre o arquivo e verifica se não ocorreu um erro.
            continua++;
          }
        }else if(nivel_de_dificuldade == 1){// CARREGA O RANKING FACIL
          if ((salvar = fopen("rankingfacil.txt", "rb")) != NULL){// Abre o arquivo e verifica se não ocorreu um erro.
            continua++;
          }
        }
        //EXIBE AS 5 PRIMEIRAS PONTUAÇÕS DO RANKING
        for (int i = 0; i < 5; i++) {

          fread(jogador.nome, sizeof(char), sizeof(jogador.nome), salvar);
          fread(&jogador.pontuacao, sizeof(int), 1, salvar);
          descriptografa(jogador.nome);

          al_draw_textf(fonte, al_map_rgb(255, 255, 255), LARGURA/ 2, H, ALLEGRO_ALIGN_CENTRE, "| %2d - %s | %d |", i+1, jogador.nome, jogador.pontuacao);

          H += 45;
        }
        fclose(salvar);

        //ATUALIZA JANELA
        al_flip_display();

        while (nivel_de_dificuldade != 0) {
          //ALOCA EVENTO NA  FILA DE EVENTOS DO MOUSE
          while (!al_is_event_queue_empty(fila_eventos_mouse)){
              ALLEGRO_EVENT evento;
              al_wait_for_event(fila_eventos_mouse, &evento);

          //CASO O EVENTO FOR O CLICK DO MOUSE
              if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

          //DEFININDO HITBOX DO BOTÃO VOLTAR DA TELA SCORE
              if (evento.mouse.x >= LARGURA/ 2 - al_get_bitmap_width(voltar_menu) / 2 &&
                    evento.mouse.x <= LARGURA / 2 + al_get_bitmap_width(voltar_menu) / 2 &&
                    evento.mouse.y >= 640 - al_get_bitmap_height(voltar_menu) / 2 &&
                    evento.mouse.y <= 610 + al_get_bitmap_height(voltar_menu) / 2){
                          nivel_de_dificuldade = 0;
                          sair++;
                          estado = ESTADO_PONTUACAO;
            }//FIM DO LAÇO DA FILA DE EVENTOS DO MOUSE
            }//IF DA AÇÃO CASO CLICAR NO BOTÃO VOLTAR
          }//IF DA HITBOX DO BOTÃO VOLTAR
        }//while
      }//if

    }//FIM DO LAÇO DA FILA DE EVENTOS DO MOUSE
  }//FIM DO LAÇO DE REPETIÇÃO DO BOTÃO VOLTAR

   sair--;//decrementar para o while funcionar
   return 0;
}//FIM DA FUNÇÃO PONTOS

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//FUNÇÃO TUTORIAL//
void tutorial(){
  inicia_tutorial = 1;
  nivel_de_dificuldade = 1;
  estado = ESTADO_INICIA_O_JOGO;
}//FIM DA FUNÇÃO TUTORIAL

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//FUNÇÃO PARA A ESCRITA DO NOME DO JOGADOR PARA O RANKING
void entrada_do_nome(ALLEGRO_EVENT evento)
{
  if (evento.type == ALLEGRO_EVENT_KEY_CHAR){
    if (strlen(str) <= 16){
      char temp[] = {evento.keyboard.unichar, '\0'};
      if (evento.keyboard.unichar == ' '){
        strcat(str, temp);
    }else if (evento.keyboard.unichar >= '0' && evento.keyboard.unichar <= '9'){
        strcat(str, temp);
      }else if (evento.keyboard.unichar >= 'A' && evento.keyboard.unichar <= 'Z'){
        strcat(str, temp);
      }else if (evento.keyboard.unichar >= 'a' && evento.keyboard.unichar <= 'z'){
        strcat(str, temp);
      }
    }
    if (evento.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && strlen(str) != 0){
      str[strlen(str) - 1] = '\0';
    }
  }
}//FIM DA FUNÇÃO PARA A ESCRITA DO NOME DO JOGADOR PARA O RANKING

//FUNÇÃO PARA EXIBIR O NOME
void exibe_a_string(){

  if (strlen(str) > 0){
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 2,
        (885- al_get_font_ascent(fonte)) / 2,
        ALLEGRO_ALIGN_CENTRE, str);
  }
}// FIM DA FUNÇÃO PARA EXIBIR O NOME

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


//FUNÇÃO INICIAR//
void iniciar(){

  FILE* salvar;
  Salvando salvo[10];
  Jogador jogador[5];

  int quantidade_de_zombies;
  int quantidade_de_humanos;
  fonte = al_load_font("arial.ttf", 88, 0);

  //CARREGA A IMAGEM DE FUNDO DO JOGO
    fundo_do_jogo = al_load_bitmap("img/Fundo.png");

  //DESENHA A IMAGEM DE FUNDO DO JOGO
    al_draw_bitmap(fundo_do_jogo, 0, 0, 0);

  //CHAMANDO ENUM DO MOVIMENTO DO PERSONAGEM
    enum movimento movi_perso;
    int pressed[4]={0,0,0,0};


  //CARREGANDO OS PARAMETROS DO SPRITE
  Sprite zombies[7];
  for (int i = 0; i < 7; i++) {
    zombies[i] = cerrega_sprite_zombie(100*i);
  }//for

  //CARREGA STRUCT PERSONAGEM
  Sprite personagem = cerrega_sprite_personagem();

//VARIÁVEIS
  srand(time(NULL));
  int desenha = 1;
  int desloca_chao = 0;
  int desloca_fundoa = 0;
  int desloca_fundob = 0;
  int desloca_arvore = 0;
  int pausa = 0;
  int score = 0;
  float vida = 50;
  char teste;
  int aumenta_dano;
  int sair = 0;
  int concluido = 0;
  int auxP;
  char auxN[25];
  int numero_de_vidas = 5;
  int cont=-1;



//ESCOLHA DA DIFICULDADE
if(nivel_de_dificuldade == 1){
  quantidade_de_zombies =3;
  aumenta_dano=0;

}
if(nivel_de_dificuldade == 2){
  quantidade_de_zombies = 5;
  aumenta_dano=0.2;
}
if(nivel_de_dificuldade == 3){
  quantidade_de_zombies=7;
  aumenta_dano=0.2;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //CARREGA O SAVE DO GAME

  if (nivel_de_dificuldade == 3) {//CARREGA O SAVE DIFICIL
    if ((salvar = fopen("savegamedificil.txt", "rb")) != NULL){// Abre o arquivo e verifica se não ocorreu um erro.
      continua++;
    }
  }else if(nivel_de_dificuldade == 2){//CARREGA O SAVE MEDIO
    if ((salvar = fopen("savegamemedio.txt", "rb")) != NULL){// Abre o arquivo e verifica se não ocorreu um erro.
      continua++;
    }
  }else if(nivel_de_dificuldade == 1){// CARREGA O SAVE FACIL
    if ((salvar = fopen("savegamefacil.txt", "rb")) != NULL){// Abre o arquivo e verifica se não ocorreu um erro.
      continua++;
    }
  }
    teste = getc(salvar);
    printf("%c\n",teste );
  if(teste =='a'){
    fseek(salvar,2,SEEK_SET);
    fread(&salvo[0].pos_x_save, sizeof(int), 1, salvar);
    fread(&salvo[0].pos_y_save, sizeof(int), 1, salvar);
    fread(&salvo[0].pont_atual, sizeof(int), 1, salvar);
    fread(&salvo[0].vida, sizeof(float), 1, salvar);
    fread(&salvo[0].numero_de_vidas,sizeof(int),1,salvar);

      if (salvo[0].pont_atual != 0) {
        personagem.pos_x_sprite = salvo[0].pos_x_save;
        personagem.pos_y_sprite = salvo[0].pos_y_save;
        score = salvo[0].pont_atual;
        vida = salvo[0].vida;
        numero_de_vidas = salvo[0].numero_de_vidas;
          for (int i = 0; i < quantidade_de_zombies; i++) {
            fread(&salvo[i].pos_x_savez, sizeof(int), 1, salvar);
            fread(&salvo[i].pos_y_savez, sizeof(int), 1, salvar);
            zombies[i].pos_x_sprite = salvo[i].pos_x_savez;
            zombies[i].pos_y_sprite = salvo[i].pos_y_savez;
          }//for
      }//if interno
      fclose(salvar);
   }//if externo

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //LAÇO DE REPETIÇÃO PARA FUNCIONAMENTO DO JOGO
  while(!sair){

    //REGISTRANDO EVENDO NA FILA_EVENTOS
      ALLEGRO_EVENT evento;
      al_wait_for_event(fila_eventos, &evento);

      //SE O EVENTO FOR O 'X' FECHA A JANELA
      if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
        sair =1;
        estado = ESTADO_FIM;
      }//EVENTO 'X'




      if(vida <= 0){

        strcpy(str, "");

        fonte = al_load_font("arial.ttf", 40, 0); //FONTE

        //ATUALIZA A JANELA
        al_flip_display();

        while (!sair){

          while (!al_is_event_queue_empty(fila_eventos)){
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);

          if (!concluido){
              entrada_do_nome(evento);

              if (evento.type == ALLEGRO_EVENT_KEY_DOWN && evento.keyboard.keycode == ALLEGRO_KEY_ENTER){
                concluido = 1;
                cont++;
                sair = 1;
              }
            }
            if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
             sair = 1;
            }
          }
            gameover1=al_load_bitmap("img/Gameover2a.png");
          al_draw_bitmap(gameover1, 0, 0, 0);

          exibe_a_string();

          al_flip_display();

        }//while

        if (nivel_de_dificuldade == 3) {//CARREGA O RANKING DIFICIL
          if ((salvar = fopen("rankingdificil.txt", "ab")) != NULL){// Abre o arquivo e verifica se não ocorreu um erro.
            continua++;
          }
        }else if(nivel_de_dificuldade == 2){//CARREGA O RANKING MEDIO
          if ((salvar = fopen("rankingmedio.txt", "ab")) != NULL){// Abre o arquivo e verifica se não ocorreu um erro.
            continua++;
          }
        }else if(nivel_de_dificuldade == 1){// CARREGA O RANKING FACIL
          if ((salvar = fopen("rankingfacil.txt", "ab")) != NULL){// Abre o arquivo e verifica se não ocorreu um erro.
            continua++;
          }
        }
          criptografa(str);
          strcpy(jogador[4].nome, str);
          jogador[4].pontuacao = score;

        for (int i = 0; i < 5; i++) {
            fwrite(jogador[i].nome, sizeof(char), sizeof(jogador[i].nome), salvar);
            fwrite(&jogador[i].pontuacao, sizeof(int), 1, salvar);
        }

      fclose(salvar);
      }//if vida






//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
if(pausa == 1){
  menu_pause = al_load_bitmap("img/pause3.png");
  al_draw_bitmap(menu_pause,0,0,0);
  al_flip_display();
}
  while(pausa == 1 || vida <= 0) {
    ALLEGRO_EVENT evento;
    al_wait_for_event(fila_eventos, &evento);

    //SE O EVENTO FOR PRESSIONAR A TECLA DO TECLADO
    if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
      //DEFININDO AS AÇÕES PARA AS TECLAS APERTADAS
      switch(evento.keyboard.keycode){
        //ENCERRA A PAUSA
        case ALLEGRO_KEY_ESCAPE:
          if (vida > 0) {
            pausa = 0;
          }
          break;

        //REINICIA O PROGRESSO
        case ALLEGRO_KEY_BACKSPACE:

        if (nivel_de_dificuldade == 3) {//REINICIA O PROGRESSO DO SAVE DIFICIL
          if ((salvar = fopen("savegamedificil.txt", "wb")) == NULL){// Abre o arquivo e verifica se não ocorreu um erro.
            printf("Erro ao executar o arquivo!");
            exit(1);
          }
        }else if (nivel_de_dificuldade == 2) {// REINICIA O PROGRESSO DO SAVE MEDIO
          if ((salvar = fopen("savegamemedio.txt", "wb")) == NULL){// Abre o arquivo e verifica se não ocorreu um erro.
            printf("Erro ao executar o arquivo!");
            exit(1);
          }
        }else if (nivel_de_dificuldade == 1) {// REINICIA O PROGRESSO DO SAVE FACIL
          if ((salvar = fopen("savegamefacil.txt", "wb")) == NULL){// Abre o arquivo e verifica se não ocorreu um erro.
            printf("Erro ao executar o arquivo!");
            exit(1);
          }
        }
            salvo[0].pos_x_save = 0;
            salvo[0].pos_y_save = 0;
            salvo[0].pont_atual = 0;
            salvo[0].vida = 50;
            salvo[0].numero_de_vidas =numero_de_vidas;
            fwrite(&salvo[0].pos_x_save, sizeof(int), 1, salvar);
            fwrite(&salvo[0].pos_y_save, sizeof(int), 1, salvar);
            fwrite(&salvo[0].pont_atual, sizeof(int), 1, salvar);
            fwrite(&salvo[0].vida, sizeof(float), 1, salvar);
            fwrite(&salvo[0].numero_de_vidas,sizeof(int),1,salvar);
          for (int i = 0; i < quantidade_de_zombies; i++) {
            salvo[i].pos_x_savez = 0;
            salvo[i].pos_y_savez = 0;
            fwrite(&salvo[i].pos_x_savez, sizeof(int), 1, salvar);
            fwrite(&salvo[i].pos_y_savez, sizeof(int), 1, salvar);
          }
          fclose(salvar);

        iniciar();
        break;

        //VOLTA AO MENU

        case ALLEGRO_KEY_ENTER:

        //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
          //SALVA A POSIÇÃO, PONTUAÇÃO
          if (nivel_de_dificuldade == 3) {// SALVA O PROGRESSO DO MODO DIFICIL
            if ((salvar = fopen("savegamedificil.txt", "wb")) == NULL){// Abre o arquivo e verifica se não ocorreu um erro.
              printf("Erro ao executar o arquivo!");
              exit(1);
            }
          }else if (nivel_de_dificuldade == 2) {// SALVA O PROGRESSO DO MODO MEDIO
            if ((salvar = fopen("savegamemedio.txt", "wb")) == NULL){// Abre o arquivo e verifica se não ocorreu um erro.
              printf("Erro ao executar o arquivo!");
              exit(1);
            }
          }else if (nivel_de_dificuldade == 1) {// SALVA O PROGRESSO DO MODO FACIL
            if ((salvar = fopen("savegamefacil.txt", "wb")) == NULL){// Abre o arquivo e verifica se não ocorreu um erro.
              printf("Erro ao executar o arquivo!");
              exit(1);
            }//IF
          }
          if (vida > 0) {//SE AINDA ESTIVER "VIVO"
            salvo[0].pos_x_save = personagem.pos_x_sprite;
            salvo[0].pos_y_save = personagem.pos_y_sprite;
            salvo[0].pont_atual = score;
            salvo[0].vida = vida;
            salvo[0].numero_de_vidas = numero_de_vidas;
          fprintf(salvar,"a ");
              fwrite(&salvo[0].pos_x_save, sizeof(int), 1, salvar);
              fwrite(&salvo[0].pos_y_save, sizeof(int), 1, salvar);
              fwrite(&salvo[0].pont_atual, sizeof(int), 1, salvar);
              fwrite(&salvo[0].vida, sizeof(float), 1, salvar);
              fwrite(&salvo[0].numero_de_vidas,sizeof(int),1,salvar);
            for (int i = 0; i < quantidade_de_zombies; i++) {
              salvo[i].pos_x_savez = zombies[i].pos_x_sprite;
              salvo[i].pos_y_savez = zombies[i].pos_y_sprite;
              fwrite(&salvo[i].pos_x_savez, sizeof(int), 1, salvar);
              fwrite(&salvo[i].pos_y_savez, sizeof(int), 1, salvar);
              continua ++;
            }//FOR
          fclose(salvar);
        }//IF

            if (vida <= 0) {// SE "MORREU"

                salvo[0].pos_x_save = 0;
                salvo[0].pos_y_save = 0;
                salvo[0].pont_atual = 0;
                salvo[0].vida = 50;
                salvo[0].numero_de_vidas= 5;
                fprintf(salvar,"a ");
              fwrite(&salvo[0].pos_x_save, sizeof(int), 1, salvar);
              fwrite(&salvo[0].pos_y_save, sizeof(int), 1, salvar);
              fwrite(&salvo[0].pont_atual, sizeof(int), 1, salvar);
              fwrite(&salvo[0].vida, sizeof(float), 1, salvar);
              fwrite(&salvo[0].numero_de_vidas,sizeof(int),1,salvar);
                for (int i = 0; i < quantidade_de_zombies; i++) {
                    salvo[i].pos_x_savez = 0;
                    salvo[i].pos_y_savez = 0;
                  fwrite(&salvo[i].pos_x_savez, sizeof(int), 1, salvar);
                  fwrite(&salvo[i].pos_y_savez, sizeof(int), 1, salvar);
                  continua++;
                }//FOR
                fclose(salvar);
          }//IF

        estado = ESTADO_MENU;

        //LAÇO DE REPETIÇÃO PARA FUNCIONAMENTO DO JOGO, CASO =ESTADO FIM - FECHA O JOGO//
          while((estado != ESTADO_FIM)){

        //CASO ESTADO = ESTADO_MENU, CHAMA A FUNÇÃO MENU
          if(estado == ESTADO_MENU){
            menu();
          }//IF FUNÇÃO MENU

        //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
          //CASO ESTADO - ESTADO_CREDITO, CHAMA A FUNÇÃO CRÉDITOS
            if(estado == ESTADO_CREDITO){
              creditos();
            }//IF FUNÇÃO CRÉDITOS

        //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
          //CASO ESTADO = ESTADO_PONTUAÇÃO, CHAMA A FUNÇÃO PONTOS
            if (estado == ESTADO_PONTUACAO){
              pontos();
            }//IF FUNÇÃO PONTOS

        //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
          //CASO ESTADO = ESTADO_TUTORIAL, CHAMA A FUNÇÃO TUTORIAL
            if(estado==ESTADO_TUTORIAL){//estado igual menu chama a função menu tutorial
              tutorial();
            }//IF FUNÇÃO TUTORIAL

            if(estado==ESTADO_DIFICULDADE){
              dificuldade();
            }

        //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
          //CASO ESTADO = ESTADO_INICIA_O_JOGO, CHAMA A FUNÇÃO INICIAR
            if(estado == ESTADO_INICIA_O_JOGO){
              iniciar();
            }//IF FUNÇÃO INICIAR
          }//FIM DO LAÇO DE REPETIÇÃO PRINCIPAL

        //DESTROI TUDO CASO O JOGO FECHE
          destruir();

        break;
      }
    }
  }
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

         //SE O EVENTO FOR PRESSIONAR A TECLA DO TECLADO
              if(evento.type == ALLEGRO_EVENT_KEY_DOWN){

      	 //DEFININDO AS AÇÕES PARA AS TECLAS APERTADAS
              switch(evento.keyboard.keycode){
          //PAUSA O JOGO
         case ALLEGRO_KEY_ESCAPE:
            pausa = 1;
            for (int i = 0; i < 4; i++){
              pressed[i] = 0;
            }
          	 break;

        //CASO PRESSIONE SETA PARA A ESQUERDA
	       case ALLEGRO_KEY_LEFT:
         case ALLEGRO_KEY_A:
            pressed[0] = 1;
          	break;

        //CASO PRESSIONE SETA PARA A DIREITA
          case ALLEGRO_KEY_RIGHT:
          case ALLEGRO_KEY_D:
            pressed[1] = 1;
          	break;

        //CASO PRESSIONE SETA PARA CIMA
          case ALLEGRO_KEY_UP:
          case ALLEGRO_KEY_SPACE:
              if(personagem.tempo_salto == -1){
                 personagem.tempo_salto = 0;
                  pressed[2] = 1;
                  printf("%i\n",pressed[2]);
                  al_play_sample(pulo, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);

                }//IF CASO SALTO FOR -1
                  break;

          //CASO PRESSIONE A TECLA E - ATAQUE
            case ALLEGRO_KEY_E:
              pressed[4] = 1;
              movi_perso = ATAQUE;
              break;

        //CASO NENHUMA TECLA FOR PRESSIONADA
            default:
            movi_perso = PARADO;
            break;
    }//FECHAMENTO SWITCH DAS AÇOES DAS TECLAS

      //CASO SOLTE A TECLA O BONECO PARA DE ANDAR
      	}else if(evento.type == ALLEGRO_EVENT_KEY_UP){

          //DEFININDO AS AÇÕES CASO SOLTE A TECLA
          switch(evento.keyboard.keycode) {

          //CASO SOLTE A SETA ESQUERDA
	         case ALLEGRO_KEY_LEFT:
           case ALLEGRO_KEY_A:
             pressed[0] = 0;
             movi_perso = PARADO;
          	 break;

          //CASO SOLTE A SETA DIREITA
           case ALLEGRO_KEY_RIGHT:
           case ALLEGRO_KEY_D:
              pressed[1] = 0;
              movi_perso = PARADO;
          	  break;

          //CASO SOLTE A SETA PARA CIMA - PULO
           case ALLEGRO_KEY_UP:
           case ALLEGRO_KEY_SPACE:
              pressed[2] = 0;
              break;

          //CASO SOLTE A TECLA E - ATAQUE
          case ALLEGRO_KEY_E:
          pressed[4] = 0;
          break;
        }//SWITCH CASO SOLTE AS TECLAS
    }//IF CASO SOLTE A TECLA


    //CASO MOVI_PERSO = PARADO
    if(movi_perso == PARADO){
      desenha = 1;

      personagem.cont_frames++;
      //DEFININDO LINHA DA FOLHA ONDE FICA O SPRITE PARADO
      personagem.linha_atual = 0;
    }

      //SE ALCANÇOU A QUANTIDADE DE FRAMES PARA MUDAR DE SPRITE
      if (personagem.cont_frames >=personagem.frames_sprite){
       //SE MUDAR DE SPRITE, RESETA A CONTAGEM DE FRAMES
          personagem.cont_frames=0;

       //INCREMENTA A COLUNA ATUAL PARA MOSTRAR PROXIMO SPRITE
          personagem.coluna_atual = (personagem.coluna_atual+1)%personagem.colunas_folhas;

       //CALCULA A REGIÃO X DA FOLHA(SPRITE) QUE SERÁ EXIBIDA
          personagem.regiao_x_folha =personagem.coluna_atual *personagem.largura_sprite;
          personagem.regiao_y_folha =personagem.linha_atual *personagem.altura_sprite;
        }//IF CONT FRAMES


      //CASO SETA ESQUERDA FOR PRESSIONADA MOVIMENTA PARA A ESQUERDA
        if(pressed[0] == 1){
          movi_perso = MOVIMETO_PARA_ESQUERDA;
          desenha = 1;

        //CARREGANDO SPRITE PARA A ESQUERDA
          personagem.cont_frames++;
          personagem.linha_atual = 1;
          //CASO SETA DIREITA FOR PRESSIONADA MOVIMENTA PARA A DIREITA
    }else if(pressed[1] == 1){
        movi_perso = MOVIMENTO_PARA_DIREITA;
        desenha = 1;
        personagem.linha_atual = 1;



      //DEFININDO O INTERVALO QUE O PERSONAGEM SE MOVA, APÓS PASSAR O X ESTABELECIDO, O CHÃO QUE SE MOVE
        if(personagem.pos_x_sprite>520){
          movi_perso = MOVIMENTO_PARA_DIREITA_FUNDO;
        }//IF INTERVALO DE MOVIMENTAÇÃO

        //CARREGANDO SPRITE PARA A DIREITA
          personagem.cont_frames++;
        }



        //SPRITE SE O PERSONAGEM PRESSIONAR A TECLA PARA ATACAR
         if(movi_perso == ATAQUE){

           if(personagem.linha_atual!= 2){
             personagem.coluna_atual = 0;
           }//IF PARA DEFINIR COLUNA ATUAL =0 PARA COMEÇAR A ANIMAÇÃO DO ATAQUE NA COLUNA 0;
          desenha = 1;
          personagem.linha_atual = 2;//DEFININDO LINHA ONDE ESTA O SPRITE DE ATAQUE
          personagem.cont_frames++;//INCREMENTANDO FRAMES
          if(personagem.cont_frames >= personagem.frames_sprite){
            personagem.cont_frames = 0;//RESENTANDO FRAMES
            personagem.coluna_atual++;//PASSANDO DE COLUNA
            //CASO AS COLUNAS ACABEM, PERSONAGEM VOLTA A FICAR PARADO
            if(personagem.coluna_atual >= personagem.colunas_folhas){
              personagem.coluna_atual = 0;
              movi_perso = PARADO;
            }
            //DESENHA PERSONAGEM
            personagem.regiao_x_folha =personagem.coluna_atual *personagem.largura_sprite;
            personagem.regiao_y_folha =personagem.linha_atual *personagem.altura_sprite;

          }//IF CONT FRAMES
        }//IF SPRITE ATAQUE


      //DEFININDO O DESENHO DO SPRITE DO ZUMBI
      for (int i = 0; i < quantidade_de_zombies; i++) {
        zombies[i].cont_frames++;

        if(zombies[i].cont_frames>zombies[i].frames_sprite){
          zombies[i].cont_frames = 0;
          zombies[i].coluna_atual = (zombies[i].coluna_atual+1)%zombies[i].colunas_folhas;
          zombies[i].regiao_x_folha = zombies[i].coluna_atual*zombies[i].largura_sprite;
          zombies[i].pos_x_sprite -= zombies[i].vel_x_sprite;
        }
      }

       //ATUALIZAÇÕES DE ACORDO COM COMANDOS E EVENTOS//
       if((desenha || personagem.pulo != -1)&& al_is_event_queue_empty(fila_eventos)) {


           //DESENHA O FUNDO DO JOGO
            al_draw_bitmap_region(fundo_do_jogo,0,0,LARGURA,ALTURA,0,0,0);

           //PARAMETROS DO PULO DO PERSONAGEM
           if(personagem.tempo_salto != -1){
              personagem.tempo_salto ++;


          //DEFININDO A ALTURA DO PULO E O TEMPO DE QUEDA DO PERSONAGEM
              personagem.pulo = 28*personagem.tempo_salto + (-1*personagem.tempo_salto*personagem.tempo_salto)/2;
           }//IF PARAMETROS PULO DO PERSONAGEM


          //DEFININDO QUE O PERSONAGEM NÃO ESTÁ PULANDO
            if (personagem.pulo<0){
              personagem.pulo = 0;
              personagem.tempo_salto = -1;
        }//DEF QUE O PERSONAGEM NÃO ESTÁ PULANDO


        //MOVIMENTANDO O CHÃO CASO O PERSONAGEM ANDE PARA A DIREITA
          if (desenha != 0) {

            //DEFININDO QUANDO O CHÃO SE MOVE
            if(movi_perso==MOVIMENTO_PARA_DIREITA){
                personagem.pos_x_sprite += personagem.vel_x_sprite;
            }else if(movi_perso==MOVIMENTO_PARA_DIREITA_FUNDO){
              for (int i = 0; i < quantidade_de_zombies; i++) {
                zombies[i].pos_x_sprite -= personagem.vel_x_sprite;
              }

            //VELOCIDADE CHÃO = (desloca_chao + *VELOCIDADE*)
              desloca_chao = (desloca_chao+4)%250;
              desloca_fundoa = (desloca_fundoa+2)%1280;
              desloca_fundob = (desloca_fundob+1)%1280;
              desloca_arvore = (desloca_arvore+4)%800;

            //CASO O PERSONAGEM SE MOVA PARA A ESQUERDA, O CHÃO FICA PARADO
           }else if(movi_perso==MOVIMETO_PARA_ESQUERDA){
             if(personagem.pos_x_sprite > 0){
             personagem.pos_x_sprite -= personagem.vel_x_sprite;
          }//IF PERSONAGEM MOVENDO PARA ESQUERDA, CHÃO PARADO
         }//IF PERSONAGEM MOVENDO PARA ESQUERDA, CHÃO PARADO
       }//IF MOVIMENTANDO O CHÃO
       for (int i = 0; i < numero_de_vidas; i++) {
         al_draw_bitmap(vida_do_personagem,0+(i*50),0,0);
       }
       for (int i = 0; i <= 1; i++){
         al_draw_bitmap(fundo_novob,(i*1280)-desloca_fundob,0,0);
       }//FOR CLONAGEM DO CHÃO
       for (int i = 0; i <= 1; i++){
         al_draw_bitmap(fundo_novo,(i*1280)-desloca_fundoa,0,0);
       }//FOR CLONAGEM DO CHÃO
       for (int i = 0; i < (LARGURA/800)+2;i++){
         al_draw_bitmap(arvore,(i*800)-desloca_arvore,ALTURA-(65+447),0);

       }//FOR CLONAGEM DO CHÃO


        //CLONANDO CHÃO A PARTIR DO DESLOCAMENTO DO PERSONAGEM
           for (int i = 0; i < (LARGURA/300)+2; i++){
             al_draw_bitmap(chao,(i*50)-desloca_chao,ALTURA-100,0);
           }//FOR CLONAGEM DO CHÃO



         //MOVIMENTO PARA A DIREITA (VELOCIDADE POSITIVA)
           if (movi_perso != MOVIMETO_PARA_ESQUERDA){

            //DESENHA SPRITE PARA A DIREITA
               al_draw_bitmap_region(folha_sprite,
                  personagem.regiao_x_folha,personagem.regiao_y_folha,
                  personagem.largura_sprite,personagem.altura_sprite,
                  personagem.pos_x_sprite,personagem.pos_y_sprite - personagem.pulo,0);
           }else{

           //DESENHA SPRITE PARA A ESQUERDA
               al_draw_scaled_bitmap(folha_sprite,
                  personagem.regiao_x_folha,personagem.regiao_y_folha,
                  personagem.largura_sprite,personagem.altura_sprite,
                  personagem.pos_x_sprite+personagem.largura_sprite,personagem.pos_y_sprite-personagem.pulo,
                  -personagem.largura_sprite,personagem.altura_sprite,0);
                }

                //DESENHA O ZUMBI
                for (int i = 0; i < quantidade_de_zombies; i++) {
                  al_draw_bitmap_region(folha_sprite_zombie,
                    zombies[i].regiao_x_folha,zombies[i].regiao_y_folha,
                    zombies[i].largura_sprite,zombies[i].altura_sprite,
                    zombies[i].pos_x_sprite,zombies[i].pos_y_sprite - zombies[i].pulo,zombies[i].inverte_sprite);
                }
                al_draw_textf(fonte,al_map_rgb(150,255,143),LARGURA-150,20,ALLEGRO_ALIGN_CENTRE,"%4d",score);
          //ATUALIZANDO TELA
            al_flip_display();

          desenha = 0;

       }//IF DESENHO DOS SPRITES DE ACORDO COM OS COMANDOS E O MOVIMENTO DO PERSONAGEM}//IF DESENHO DOS SPRITES DE ACORDO COM OS COMANDOS E O MOVIMENTO DO PERSONAGEM
       if(inicia_tutorial==1){
       for (int i = 0; i < quantidade_de_zombies; i++) {
         if(zombies[i].pos_x_sprite<900){
           menu_pause = al_load_bitmap("img/1imagem.png");
           al_draw_bitmap(menu_pause,0,0,0);
           al_flip_display();
           while (!sair) {
           ALLEGRO_EVENT evento;
           al_wait_for_event(fila_eventos, &evento);

           //SE O EVENTO FOR PRESSIONAR A TECLA DO TECLADO
           if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
             //DEFININDO AS AÇÕES PARA AS TECLAS APERTADAS
             switch(evento.keyboard.keycode){
               //ENCERRA A PAUSA
               case ALLEGRO_KEY_SPACE:
                 sair++;
                 inicia_tutorial++;
                 for (int i = 0; i < 4; i++) {
                   pressed[i]=0;
                 }

                 break;
               }
             }
         }
         sair--;
         }
       }
       }
       for (int i = 0; i < quantidade_de_zombies; i++) {
         if(zombies[i].pos_x_sprite<0){
           vida=vida-(0.1+aumenta_dano+score*0.00001);
         }

       if(personagem.pos_x_sprite>(zombies[i].pos_x_sprite)&&
       personagem.pos_x_sprite< (zombies[i].pos_x_sprite + zombies[i].largura_sprite) &&
       personagem.pos_y_sprite-personagem.pulo>(zombies[i].pos_y_sprite)&&
       personagem.pos_y_sprite-personagem.pulo<zombies[i].pos_y_sprite + zombies[i].altura_sprite){
       }

       //HITBOX DO PERSONAGEM COM O ZUMBI CASO O PERSONAGEM ESTEJA ATACANDO, O ZUMBI SOME
       if(personagem.pos_x_sprite + personagem.largura_sprite-30 > zombies[i].pos_x_sprite &&
       personagem.pos_x_sprite < zombies[i].pos_x_sprite + zombies[i].largura_sprite-45 &&
       personagem.pos_y_sprite + personagem.altura_sprite - personagem.pulo-50 > zombies[i].pos_y_sprite &&
       personagem.pos_y_sprite - personagem.pulo < zombies[i].pos_y_sprite + zombies[i].altura_sprite-50 && movi_perso == ATAQUE && personagem.coluna_atual == 5){
         zombies[i].pos_x_sprite = 1280 + rand()%100;//"MORTE DO ZUMBI"//
         score = score+10;
         for (int  i = 0; i < quantidade_de_zombies; i++) {
           zombies[i].vel_x_sprite= zombies[i].vel_x_sprite +(score*0.001);
         }
         if(inicia_tutorial==2){
           inicia_tutorial++;
         }
       }

       if (zombies[i].pos_x_sprite + zombies[i].largura_sprite > 1280 - -130 || zombies[i].pos_x_sprite< -130){
         zombies[i].pos_x_sprite = 1280 + (rand()%100)+rand()%100;
       }

       //CASO O PERSONAGEM NÃO ESTEJA ATACANDO, O PERSONAGEM MORRE
       else if(personagem.pos_x_sprite + personagem.largura_sprite-50 > zombies[i].pos_x_sprite &&
       personagem.pos_x_sprite < zombies[i].pos_x_sprite + zombies[i].largura_sprite-50 &&
       personagem.pos_y_sprite + personagem.altura_sprite - personagem.pulo-50 > zombies[i].pos_y_sprite &&
       personagem.pos_y_sprite - personagem.pulo < zombies[i].pos_y_sprite + zombies[i].altura_sprite-50 && movi_perso != ATAQUE){
         //aqui vai oq acontece quando o zumbi mata o personagem//
         vida=vida-(0.1+aumenta_dano+score*0.00001);
         numero_de_vidas = (vida /10)+0.9;

         }
       if(inicia_tutorial==3){
         if(zombies[i].pos_x_sprite<900){
           menu_pause = al_load_bitmap("img/telinhacerta2.png");
           al_draw_bitmap(menu_pause,0,0,0);
           al_flip_display();
           while (!sair) {
           ALLEGRO_EVENT evento;
           al_wait_for_event(fila_eventos, &evento);

           //SE O EVENTO FOR PRESSIONAR A TECLA DO TECLADO
           if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
             //DEFININDO AS AÇÕES PARA AS TECLAS APERTADAS
             switch(evento.keyboard.keycode){
               //ENCERRA A PAUSA
               case ALLEGRO_KEY_SPACE:
                 sair++;
                 inicia_tutorial=0;
                 estado=ESTADO_MENU;
                 for (int i = 0; i < 4; i++) {
                   pressed[i]=0;
                 }

                 break;
               }
             }
         }
       }
     }
     }
   }//FIM DO LAÇO DE REPETIÇÃO PARA FUNCIONAMENTO DA GAMEPLAY

 }//FUNÇÃO INICIAR
 void configuracao_do_jogo(){
   config = al_load_bitmap("img/Config.png");
   al_draw_bitmap(config, 0, 0, 0);

   fonte = al_load_font("stranger.ttf", 85, 0);//FONTE

 //ESCREVE BOTÕES NA TELA SCORE

   al_draw_text(fonte, al_map_rgb(255, 255, 255), 300, 450, ALLEGRO_ALIGN_CENTRE, "Música:");
   al_draw_text(fonte, al_map_rgb(255, 255, 255), 500, 450, ALLEGRO_ALIGN_CENTRE, "Ativado");
   al_draw_text(fonte, al_map_rgb(255, 255, 255), 700, 450, ALLEGRO_ALIGN_CENTRE, "Desativado");
   al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 2, 600, ALLEGRO_ALIGN_CENTRE, "Voltar");

   al_flip_display();
   voltar_menu = al_create_bitmap(130 ,70);

   while(!sair){

     //ALOCA EVENTO NA  FILA DE EVENTOS DO MOUSE
       while (!al_is_event_queue_empty(fila_eventos_mouse)){
         ALLEGRO_EVENT evento;
         al_wait_for_event(fila_eventos_mouse, &evento);

     //CASO O EVENTO FOR O CLICK DO MOUSE
         if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

     //DEFININDO HITBOX DO BOTÃO VOLTAR DA TELA SCORE
     if (evento.mouse.x >= 460 - al_get_bitmap_width(voltar_menu) / 2 &&
     evento.mouse.x <= 480 + al_get_bitmap_width(voltar_menu) / 2 &&
     evento.mouse.y >= 520 - al_get_bitmap_height(voltar_menu) / 2 &&
     evento.mouse.y <= 490 + al_get_bitmap_height(voltar_menu) / 2){
       sair = 1;
       som =0;
       estado = ESTADO_MENU;
     }//IF DA AÇÃO CASO CLICAR NO BOTÃO VOLTAR
   }//IF DA HITBOX DO BOTÃO VOLTAR
         if (evento.mouse.x >= 700 - al_get_bitmap_width(voltar_menu) / 2 &&
               evento.mouse.x <= 710 + al_get_bitmap_width(voltar_menu) / 2 &&
               evento.mouse.y >= 520 - al_get_bitmap_height(voltar_menu) / 2 &&
               evento.mouse.y <= 490 + al_get_bitmap_height(voltar_menu) / 2){
                     sair = 1;
                     som =2;
                     printf("%i\n",som );
                     estado = ESTADO_MENU;
         }//IF DA AÇÃO CASO CLICAR NO BOTÃO VOLTAR
       }//IF DA HITBOX DO BOTÃO VOLTAR
   }
   sair--;

 }

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//**** FUNÇÃO MAIN ****//
int main(void){
srand(time(NULL));
//CHAMANDO FUNÇÃO INICIALIZAR//
  inicializar();

//LAÇO DE REPETIÇÃO PARA FUNCIONAMENTO DO JOGO, CASO =ESTADO FIM - FECHA O JOGO//
  while((estado != ESTADO_FIM)){

//CASO ESTADO = ESTADO_MENU, CHAMA A FUNÇÃO MENU
  if(estado == ESTADO_MENU){
    menu();
  }//IF FUNÇÃO MENU

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //CASO ESTADO - ESTADO_CREDITO, CHAMA A FUNÇÃO CRÉDITOS
    if(estado == ESTADO_CREDITO){
      creditos();
    }//IF FUNÇÃO CRÉDITOS

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //CASO ESTADO = ESTADO_PONTUAÇÃO, CHAMA A FUNÇÃO PONTOS
    if (estado == ESTADO_PONTUACAO){
      pontos();
    }//IF FUNÇÃO PONTOS

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //CASO ESTADO = ESTADO_TUTORIAL, CHAMA A FUNÇÃO TUTORIAL
    if(estado==ESTADO_TUTORIAL){//estado igual menu chama a função menu tutorial
      tutorial();
    }//IF FUNÇÃO TUTORIAL

    if(estado==ESTADO_DIFICULDADE){
      dificuldade();
    }
    if(estado==ESTADO_CONFIGURACAO){
      configuracao_do_jogo();
    }
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //CASO ESTADO = ESTADO_INICIA_O_JOGO, CHAMA A FUNÇÃO INICIAR
    if(estado == ESTADO_INICIA_O_JOGO){
      iniciar();
    }//IF FUNÇÃO INICIAR
  }//FIM DO LAÇO DE REPETIÇÃO PRINCIPAL

//DESTROI TUDO CASO O JOGO FECHE
  destruir();

  return 0;
}//FIM DA MAIN
