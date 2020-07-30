/*
Universidade de Brasilia
Instituto de Ciencias Exatas
Departamento de Ciencia da Computacao

Algoritmos e Programação de Computadores – 2/2017

Aluno: Matheus Nicollas de Souza Mota
Matricula: 17/0111199
Turma: A
Versão do compilador: 5.4.0 20160609
Descrição: O programa se resume em um jogo interativo, cujo objetivo eh capturar todos os pontos no mapa
usando o minimo de movimentos possiveis, fugindo de inimigos e obstaculos no mapa.  
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define VERMELHO    "\x1b[31m"
#define VERDE   "\x1b[32m"
#define AMARELO  "\x1b[33m"
#define ROSA "\x1b[35m"			/* Macros para definir cores dos personagens */
#define CIANO    "\x1b[36m"
#define BRANCO   "\x1b[0m"

#define MAX_JOGADAS 400 	/*Macro de definicao de jogadas*/

#ifdef _WIN32
	#define LIMPA "cls"
#else						/* Macro para limpar tela em ambos sistemas, Windows e Linux */
	#define LIMPA "clear"
#endif

typedef struct {
	char nick[11];
	int score;
	int jogadas;
} Player;

char arena[20][20];	/* Matriz que da arena em geral */
int cx;				/*variavel que define a posicao do personagem C em todo eixo X da matriz*/
int cy;				/*variavel que define a posicao do personagem C em todo eixo Y da matriz*/
int pontos = 0;		
int xX[3];			/* vetor que define a posicao dos 3 personagens X em todo eixo X da matriz*/
int yX[3];			/* vetor que define a posicao dos 3 personagens X em todo eixo Y da matriz*/
int bx[3][5];		/* matriz que define a posicao dos 3 personagens B em todo eixo X da matriz, tendo 5 posicoes para eles se moverem*/
int by[3][5];		/* matriz que define a posicao dos 3 personagens B em todo eixo Y da matriz, tendo 5 posicoes para eles se moverem*/
int xQ;				/*variavel que define a posicao do personagem Q em todo eixo X da matriz*/
int yQ;				/*variavel que define a posicao do personagem Q em todo eixo Y da matriz*/
int cont_bomba = 0;	/* conta os intervalos de jogadas que a bomba deve explodir*/
int cont_aleat_x = -1;	/* conta os intervalos de jogadas que o X deve se movimentar aleatoriamente*/	
int dead = 0;			/* variavel que informa se o C esta morto ou nao*/
int flag_mov = 0;	/* variavel que informa se alguma tecla invalida foi digitada*/
int i_geral;		/* contador de para for no switch case*/
int jogadas_restantes; 
int jogadas_utilizadas;
int j_geral;		/* contador de para for no switch case*/
Player player[11];

void InicializaRanking(){
	int i;

	for (i = 0; i < 11; i++){
		player[i].score = -1;
	}
}

void cria_jogador(){
	cx = rand() % 20;		/* atribui valores aleatorios para o spawn do C*/
	cy = rand() % 20;
	while(arena[cx][cy] != '.'){
		cx = rand() % 20;				/* verifica se já tem outro personagem na mesma posicao*/
		cy = rand() % 20;		
	}		
	arena[cx][cy] = 'C'; 
}

void cria_X(){
	int i;
	for (i = 0; i < 3; i++){
		xX[i] = rand() % 20;	/* atribui valores aleatorios para o spawn dos 3 X*/
		yX[i] = rand() % 20;
		while(arena[xX[i]][yX[i]] != '.'){
			xX[i] = rand() % 20;			/* verifica se ja tem outro personagem na mesma posicao*/
			yX[i] = rand() % 20;		
		}
		arena[xX[i]][yX[i]] = 'X';
	}	
}

void cria_O(){

	int ox = rand() % 20;		/* atribui valores aleatorios para o spawn do O*/
	int oy = rand() % 20;

	while(arena[ox][oy] != '.'){
		ox = rand() % 20;			/* verifica se ja tem outro personagem na mesma posição*/
		oy= rand() % 20; 			
	}
	arena[ox][oy] = 'O';
}

void cria_arena(){
	int i, j;
	for (i = 0; i < 20; i++){	
		for (j = 0; j < 20; j++){	/* Atribui todas as posicoes da arena para '.'*/
			arena[i][j] = '.';	
		}			
	}
}

void printa_arena(){
	int i, j;

	for (i = 0; i < 20; i++){
		for (j = 0; j < 20; j++){
			if (arena[i][j] == 'B'){
				printf("%s%c%s ", CIANO, arena[i][j], BRANCO);
			}else if(arena[i][j] == 'X'){
				printf("%s%c%s ", VERMELHO, arena[i][j], BRANCO);
			}else if((arena[i][j] == 'Q') || (arena[i][j] == '#')){ /* Atribui cores para diferentes personagens*/
				printf("%s%c%s ", ROSA, arena[i][j], BRANCO);
			}else if((arena[i][j] == 'O') || (arena[i][j] == '#')){
				printf("%s%c%s ", VERDE, arena[i][j], BRANCO);
			}else{
				printf("%c ", arena[i][j]);	 /* printa toda a arena*/
			}
		}			
		printf("\n");
	}
}

void cria_B(){
	int i, j;

	for(j = 0; j < 3; j++){
		for (i = 0; i < 5; i++){
			by[j][i] = -1;			/* atribui para todas posições de B valores nulos (inacessiveis)*/
			bx[j][i] = -1;				
		}
		by[j][0] = rand() % 20;		/* atribui valores aleatorios para o spawn dos 3 B na primeira posicao (cabeca) de cada um*/
		bx[j][0] = rand() % 20;
		while(arena[bx[j][0]][by[j][0]] != '.'){
			by[j][0] = rand() % 20;					/* verifica se ja tem outro personagem na mesma posição*/
			bx[j][0] = rand() % 20;
		}
		arena[bx[j][0]][by[j][0]] = 'B';
	}		
}

void cria_Q(){
	xQ = rand() % 20;		/* atribui valores aleatorios para o spawn do Q*/
	yQ = rand() % 20;
	while(arena[xQ][yQ] != '.'){
		xQ = rand() % 20;			/* verifica se ja tem outro personagem na mesma posição*/
		yQ = rand() % 20;	
	}
	arena[xQ][yQ] = 'Q';
}

void limpa_bomba(){
	int i, j;
	for (i = 0; i < 20; i++){
		for (j = 0; j < 20; j++){
			if (arena[i][j] == '#')	
				arena[i][j] = '.';	
		}			
	}
}

void explode_Q(){
	int i, f1 = 0, f2 = 0, f3 = 0, f4 = 0; /* flags barram a explosao ao encontrarem algum personagem diferente de C*/

	if (cont_bomba == 5){  /* condicao que explode a bomba depois de 6 jogadas*/
		for (i = 0; i < 8; ++i){	/* faz com que a explosao tenha um raio de 8 grids*/
			if (((arena[xQ+i][yQ] == '.') || (arena[xQ+i][yQ] == 'C')) && f1 == 0){
				arena[xQ][yQ] = '#';
				if (xQ+i <= 19){	/*	condicao para a explosao nao sair da arena*/
					if (arena[xQ+i][yQ] == 'C'){
						dead = 3; 	/* o valor 3 indica que o C foi morto pela bomba*/
						arena[xQ+i][yQ] = '*'; /* '*' indica que o C esta morto */
					}else{
						arena[xQ+i][yQ] = '#';
					}
				}	
			}else if((arena[xQ+i][yQ] == 'X') || (arena[xQ+i][yQ] == 'O') || (arena[xQ+i][yQ] == 'B')){
				f1 = 1;		/* condição para barrar a explosão*/
			}
			/* faz as mesmas verificacoes da ultima condicao so que em direcoes diferentes*/
			if (((arena[xQ-i][yQ] == '.') || (arena[xQ-i][yQ] == 'C')) && f2 == 0){
				arena[xQ][yQ] = '#';
				if (xQ-i >= 0){
					if (arena[xQ-i][yQ] == 'C'){
						dead = 3;
						arena[xQ-i][yQ] = '*';
					}else{
						arena[xQ-i][yQ] = '#';
					}
				}	
			}else if((arena[xQ-i][yQ] == 'X') || (arena[xQ-i][yQ] == 'O') || (arena[xQ-i][yQ] == 'B')){
				f2 = 1;
			}
			if (((arena[xQ][yQ+i] == '.') || (arena[xQ][yQ+i] == 'C')) && f3 == 0){
				arena[xQ][yQ] = '#';
				if (yQ+i <= 19){
					if (arena[xQ][yQ+i] == 'C'){
						dead = 3;
						arena[xQ][yQ+i] = '*';
					}else{
						arena[xQ][yQ+i] = '#';
					}
				}
			}else if((arena[xQ][yQ+i] == 'X') || (arena[xQ][yQ+i] == 'O') || (arena[xQ][yQ+i] == 'B')){
				f3 = 1;
			}
			if (((arena[xQ][yQ-i] == '.') || (arena[xQ][yQ-i] == 'C')) && f4 == 0){
				arena[xQ][yQ] = '#';
				if (yQ-i >= 0){
					if (arena[xQ][yQ-i] == 'C'){
						dead = 3;
						arena[xQ][yQ-i] = '*';
					}else{
						arena[xQ][yQ-i] = '#';
					}
				}
			}else if((arena[xQ][yQ-i] == 'X') || (arena[xQ][yQ-i] == 'O') || (arena[xQ][yQ-i] == 'B')){
				f4 = 1;
			}
		}
		cont_bomba = 0; /* zera o contador para fazer a contagem de jogadas novamente*/
		cria_Q();
	}else{
		limpa_bomba();
		cont_bomba++;
	}
}

void movimento_aleatorio_x(int i){
				
	int aleatorio;
	
	aleatorio = rand() % 4; /* gera valores de 0 a 3*/
	
	if (aleatorio == 0){
		if ((arena[xX[i]+1][yX[i]] == '.') && (xX[i] < 19))	/* cada valor gerado tem uma condicao de direcao diferente*/
			xX[i]++;
	}else if(aleatorio == 1){
		if ((arena[xX[i]-1][yX[i]]) == '.' && (xX[i] > 0))
			xX[i]--;											
	}else if(aleatorio == 2){
		if ((arena[xX[i]][yX[i]+1]) == '.' && (yX[i] < 19))
			yX[i]++;
	}else{
		if ((arena[xX[i]][yX[i]-1]) == '.' && (yX[i] > 0))
			yX[i]--;
	}
	arena[xX[i]][yX[i]] = 'X';
	cont_aleat_x = 0;	/* zera o contador responsavel por chamar essa função a cada 3 rodadas*/
}


void movimenta_x(){
	/* O X de maneira geral ira seguir o C priorizando a perseguicao pelo eixo X, buscando ficar na mesma linha que o C*/
	/* depois que isso acontecer ele irá o perseguir pelo eixo Y*/
	int i;
	for (i = 0; ((i < 3) && (dead == 0)); i++){		/* move os 3 X de uma vez*/
		if (xX[i] > cx){	/* verifica se o C esta acima de X*/							
			if (cont_aleat_x == 3){		
				arena[xX[i]][yX[i]] = '.';
				movimento_aleatorio_x(i);	
				cont_aleat_x = 0;
			}else{
				if ((arena[xX[i]-1][yX[i]]) == '.'){ /* Verifica se ha algum personagem que nao seja '.'*/
					arena[xX[i]][yX[i]] = '.';
					xX[i]--;
					if (arena[xX[i]][yX[i]] == 'C'){	/* verifica se a posicao a qual ele foi eh a mesma de C*/
						dead = 2;		/* O valor 2 indica que o C foi morto pelo X*/
						arena[cx][cy] = '*'; 
					}else{
						arena[xX[i]][yX[i]] = 'X';
						cont_aleat_x++;
					}
				}
			}	
		}else if(xX[i] < cx){ /* verifica se o C esta abaixo de X*/ 
			/* faz as mesmas verificacoes da ultima condição so que em direcoes diferentes*/
			if (cont_aleat_x == 3){
				arena[xX[i]][yX[i]] = '.';
				movimento_aleatorio_x(i);
				cont_aleat_x = 0;
			}else{
				if ((arena[xX[i]+1][yX[i]]) == '.'){
					arena[xX[i]][yX[i]] = '.';
					xX[i]++;
					if (arena[xX[i]][yX[i]] == 'C'){
						dead = 2;
						arena[cx][cy] = '*';
					}else{
						arena[xX[i]][yX[i]] = 'X';
						cont_aleat_x++;
					}
				}
			}	
		}else if(xX[i] == cx){ /* verifica se o C esta na mesma linha de X*/ 
			if((arena[xX[i]][yX[i]+1] == 'C') || (arena[xX[i]][yX[i]-1] == 'C')){
				dead = 2;
				arena[cx][cy] = '*';
			}

			if (yX[i] < cy){ /* verifica se o C esta a direita de X*/ 
				if (cont_aleat_x == 3){
					arena[xX[i]][yX[i]] = '.';
					movimento_aleatorio_x(i);
					cont_aleat_x = 0;
				}else{
					if ((arena[xX[i]][yX[i]+1]) == '.'){
						arena[xX[i]][yX[i]] = '.';
						yX[i]++;
						if (arena[xX[i]][yX[i]] == 'C'){
							dead = 2;
							arena[cx][cy] = '*';
						}else{
							arena[xX[i]][yX[i]] = 'X';
							cont_aleat_x++;
						}	
					}	
				}	
			}else if(yX[i] > cy){ /* verifica se o C esta a esquerda de X*/ 
				if (cont_aleat_x == 3){
					arena[xX[i]][yX[i]] = '.';
					movimento_aleatorio_x(i);
					cont_aleat_x = 0;
				}else{
					if ((arena[xX[i]][yX[i]-1]) == '.'){
						arena[xX[i]][yX[i]] = '.';
						yX[i]--;
						if (arena[xX[i]][yX[i]] == 'C'){
							dead = 2;
							arena[cx][cy] = '*';
						}else{
							arena[xX[i]][yX[i]] = 'X';
							cont_aleat_x++;	
						}	
					}
				}
			}
		}	
	}
}
void movimenta_B(int j){ /* recebe o contador do for de onde esta sendo chamado para mover os 3 B*/ 
	int aleatorio, auxbx, auxby, k;

	aleatorio = rand() % 4; /* determina valores de 0 a 3*/ 

	auxbx = bx[j][0]; /* recebe os valores das posicao do eixo x dos 3 B*/ 
	auxby = by[j][0]; /* recebe os valores das posicao do eixo Y dos 3 B*/

	/* Verifica o valor da direcao e impede que o B saia do mapa ou coma algum personagem*/
	if ((aleatorio == 0) && (auxbx+1 <= 19) && (arena[bx[j][0]+1][by[j][0]] == '.')){
		auxbx++; /* faz a cabeca de B andar uma posicao baixo*/
		arena[auxbx][auxby] = 'B';

		if ((bx[j][4] != -1) && by[j][4] != -1){ /* Verifica se a ultima posição de B ja foi preenchida*/
			arena[bx[j][4]][by[j][4]] = '.';	/* Apaga o rastro da útima podicao*/
		}

		for (k = 4; k >= 1; k--){	/* for responsavel pelas trocas de posicoes de cada movimento*/
			bx[j][k] = bx[j][k-1];	/* As posicoes de B sao trocadas dentro do vetor para cada um do indice J (3)*/		
			by[j][k] = by[j][k-1];			
		}				
	/* faz as mesmas verificacoes da ultima condicao so que em direcoes diferentes*/					
	}else if((aleatorio == 1) && (auxbx-1 >= 0) && (arena[bx[j][0]-1][by[j][0]] == '.')){
		auxbx--;
		arena[auxbx][auxby] = 'B';

		if ((bx[j][4] != -1) && by[j][4] != -1){
			arena[bx[j][4]][by[j][4]] = '.';
		}

		for (k = 4; k >= 1; k--){
			bx[j][k] = bx[j][k-1];			
			by[j][k] = by[j][k-1];			
		}							
	}else if((aleatorio == 2) && (auxby+1 <= 19) && (arena[bx[j][0]][by[j][0]+1] == '.')){				
		auxby++;
		arena[auxbx][auxby] = 'B';

		if ((bx[j][4] != -1) && by[j][4] != -1){
			arena[bx[j][4]][by[j][4]] = '.';
		}

		for (k = 4; k >= 1; k--){
			bx[j][k] = bx[j][k-1];			
			by[j][k] = by[j][k-1];			
		}								
	}else if((aleatorio == 3) && (auxby-1 >= 0) && (arena[bx[j][0]][by[j][0]-1] == '.')){
		auxby--;
		arena[auxbx][auxby] = 'B';

		if ((bx[j][4] != -1) && by[j][4] != -1){
			arena[bx[j][4]][by[j][4]] = '.';
		}

		for (k = 4; k >= 1; k--){
			bx[j][k] = bx[j][k-1];			
			by[j][k] = by[j][k-1];			
		}							
			
	}
	/* Impede que o B se enrole e saia da arena, caso fique na beira da arena ele pode percorrer entre si mesmo, ate que possa ter uma posicao livre novamente*/
	if ((arena[bx[j][0]+1][by[j][0]] != '.') && (arena[bx[j][0]-1][by[j][0]] != '.') && (arena[bx[j][0]][by[j][0]-1] != '.') && (arena[bx[j][0]][by[j][0]+1] != '.')){
	
			if ((bx[j][4] != -1) && by[j][4] != -1){
				arena[bx[j][4]][by[j][4]] = '.';
			}

			for (k = 4; k >= 1; k--){
				bx[j][k] = bx[j][k-1];			
				by[j][k] = by[j][k-1];			
			}
			
	}

	/* os auxiliares atribuem o valor que adicionaram aos eixos para a cabeca novamente*/
	bx[j][0] = auxbx;
	by[j][0] = auxby;		
}

void movimenta_jogador(){
	char mov;

	scanf(" %c", &mov);	/* recebe o valor do comando dado*/
	arena[cx][cy] = '.';	/* desfaz o rastro*/

	if ((mov == 'w') || (mov == 'W')){
		if (cx > 0){ /* evita que C saia da arena*/
			if (arena[cx-1][cy] == 'X'){
				dead = 2; 	/* Indica que o C foi morto pelo X*/
				arena[cx][cy] = '*';
			}else if(arena[cx-1][cy] == 'O'){
				pontos += 10;	/* adiciona mais 10 pontos*/
				cx--;
			}else if(arena[cx-1][cy] == '.'){
				cx--;	/* Se move normalmente*/
			}	
		}
	/* faz as mesmas verificacoes da ultima condicao so que em direcoes diferentes*/		
	}else if ((mov == 'a') || (mov == 'A')){
		if (cy > 0){
			if (arena[cx][cy-1] == 'X'){
				dead = 2;
				arena[cx][cy] = '*';
			}else if(arena[cx][cy-1] == 'O'){
				pontos += 10;
				cy--;
			}else if(arena[cx][cy-1] == '.'){
				cy--;
			}	
		}	
	}else if((mov == 's') || (mov == 'S')){
		if (cx < 19){
			if (arena[cx+1][cy] == 'X'){
				dead = 2;
				arena[cx][cy] = '*';
			}else if(arena[cx+1][cy] == 'O'){
				pontos += 10;
				cx++;
			}else if(arena[cx+1][cy] == '.'){
				cx++;
			}	
		}	
	}else if((mov == 'd') || (mov == 'D')){
		if (cy < 19){
			if (arena[cx][cy+1] == 'X'){
				dead = 2;
				arena[cx][cy] = '*';
			}else if(arena[cx][cy+1] == 'O'){
				pontos += 10;
				cy++;
			}else if(arena[cx][cy+1] == '.'){
				cy++;
			}
		}	
	}else{
		flag_mov = 1;	/* Ativa a flag de aviso que foi digitado uma tecla errada*/	
		arena[cx][cy] = 'C';	
	}

	if ((flag_mov != 1) && (dead == 0)){
		arena[cx][cy] = 'C';	
	}else{
		arena[cx][cy] = '*';	
	}
}

void reseta(){
	cria_arena();	
	cria_jogador();
	cria_O();			
	cria_O();
	cria_O();	
	cria_X();
	cria_B();
	cria_Q();		
	cont_bomba = 0;	
	cont_aleat_x = -1;		
	dead = 0;			
	flag_mov = 0;
	pontos = 0;
}

void registerMatch(){
	
	int i;

	FILE *fp;

	fp = fopen("ranking.bin","r+b");

	if (fp == NULL){
		fp = fopen("ranking.bin", "wb");
		InicializaRanking();
	}

	printf("Digite o seu Nick (Min 1 e Max 10 caracteres): \n");
	scanf("%s", player[10].nick);

	while((strlen(player[10].nick) > 10)){
		printf("%sDigite um nick com menos de 10 caracteres:%s\n", VERMELHO, BRANCO);
		scanf("%s", player[10].nick);
	}

	player[10].score = pontos;
	player[10].jogadas = jogadas_utilizadas;


	for(i = 0; i < 11; i++){
		fwrite(&player[i], sizeof(Player), 1, fp);
	}

	fclose(fp);
	system(LIMPA);
}

void MostraRanking(){

	int i, j;
	Player aux;

	system(LIMPA);

	FILE *fp;

	fp = fopen("ranking.bin", "rb");

	if (fp == NULL){
		fp = fopen("ranking.bin", "a+b");
	}

	for (i = 0; i < 11; i++){

		fread(&player[i],sizeof(Player),1,fp);
	}	

	for (i = 0; i < 11; i++){
		for (j = i; j < 11; j++){
			if (player[i].score <= player[j].score){
				if(player[i].score == player[j].score){
					if((player[i].jogadas > player[j].jogadas)){
						aux = player[i];
						player[i] = player[j];
						player[j] = aux;
					}
				}else{
					aux = player[i];
					player[i] = player[j];
					player[j] = aux;
				}
			}
		}
	}

	printf("---------------- %sRANKING - TOP 10%s ----------------\n\n", VERDE, BRANCO);
	for(i = 0; i < 10; i++){
		if(player[i].score != -1){
			printf("%dº - %s     ", i+1, player[i].nick);
			printf("%d Pontos   ", player[i].score);
			printf("%d Jogadas Efetuadas\n", player[i].jogadas);
		}
	}	

	fclose(fp);
	printf("\nPressione ENTER para voltar ao menu\n");
}

void instrucoes(){
	system(LIMPA);
	printf("----------------------- INSTRUCOES ----------------------- \n\n");
	printf("%sOBJETIVO:%s Capturar todos os pontos indicados pelo simbolo 'O' no mapa usando o mínimo de movimentos possiveis, fugindo de inimigos e obstaculos no mapa.\n", VERDE, BRANCO);
	printf("%sPERSONAGEM%s Você controlara o personagem indicado pelo simbolo 'C' colorido de branco, no qual eh livre para percorrer toda a arena preenchida por pontos '.', nao podendo sair da arena. Se o personagem se tranformar em '*' indica que ele esta morto.\n", AMARELO, BRANCO);
	printf("%sINIMIGO AGRESSIVO:%s Indicado pelo simbolo 'x' colorido de vermelho, esse inimigo ira te perseguir dutante o jogo, voce nao pode entrar em contato com ele, caso isso aconteca você morrera.\n", VERMELHO, BRANCO);
	printf("%sINIMIGO PASSIVO:%s Indicado pelo simbolo 'B' colorido de azul, esse inimigo fara movimentos aleatorios dutante o jogo, deixando um rastro de no maximo 5 posicoes, esse inimigo nao tem capacidade de te matar, porem ele barra seus movimentos impedindo sua passagem.\n", CIANO, BRANCO);
	printf("%sINIMIGO EXPLOSIVO:%s Indicado pelo simbolo 'Q' colorido de rosa,aparecera a todo momento em um lugar aleatorio do mapa, causando uma explosao de um raio de 8 pontos no mapa, caso voce entre em contato com ela voce morrera.\n", ROSA, BRANCO);
	printf("%sCONTROLES:%s Voce pode controlar seu personagem com as telas 'w' (para mover para cima), 'a' (para mover para a esquerda), 's' (para mover para baixo) e 'd' (para mover baixo). TODOS OS MOVIMENTOS devem ser seguidos da tecla ENTER para que possam ser executados.\n\n\n", VERDE, BRANCO);

	printf("%sPressione ENTER para voltar ao menu%s\n", VERDE, BRANCO);
	scanf("%*c");
	getchar();
}

void menu(){

	int opcao;

	srand(time(0));

	system(LIMPA);
	printf("-------- Pac Point --------\n");
	printf("%s1 - Jogar%s\n", VERDE, BRANCO);
	printf("%s2 - Configuracoes%s\n", AMARELO, BRANCO);
	printf("%s3 - Ranking%s\n", ROSA, BRANCO);
	printf("%s4 - Instrucoes%s\n", CIANO, BRANCO);
	printf("%s5 - Sair%s\n\n", VERMELHO, BRANCO);

	printf("Escolha uma opcao:");
	scanf("%d", &opcao);

	/* Se for digitado uma opcao diferente das mostradas eh mostrado esse erro*/	
	while((opcao < 1) || (opcao > 6)){
		system(LIMPA);
		printf("-------- Pac Point --------\n\n");
		printf("COMANDO INVALIDO!\n\n");	
		printf("6 - Voltar\n\n");
		printf("Escolha uma opcao:");
		scanf("%d", &opcao);
		getchar();
	}

	switch (opcao) {
		case 1: 
			system(LIMPA);		
			reseta();
			for (i_geral = 0; ((i_geral < MAX_JOGADAS) && (dead == 0)); i_geral++){ /* enquanto jogadas nao se esgotarem e a flag dead = 0 o jogo continua*/
				printf("Pontos: %d     ", pontos);
				printf("Jogadas restantes: %d\n\n", MAX_JOGADAS - i_geral);
				if (flag_mov == 1){	/* Se a flag  for ativada mostra a mensagem*/
					printf("COMANDO NAO IDENTIFICADO, USE W,A,S OU D PARA SE MOVIMENTAR\n");
					flag_mov = 0;
				}
				printa_arena();								/* ----------------------------------*/
				movimenta_jogador();
				//movimenta_x();
				for (j_geral = 0; j_geral < 3; j_geral++){ 	/* chama as funcoes de movimentos dos personagens*/
					movimenta_B(j_geral);
				}
				explode_Q();
				if (arena[cx][cy] == 'X'){					/* ----------------------------------*/
					dead = 1;
					arena[cx][cy] = '*';
				}
				if (pontos == 30){	/* Encerra o jogo se todos os pontos forem pegos*/
					dead = 4;\
				}
				system(LIMPA);
			}

			if (i_geral == MAX_JOGADAS){ /* se as jogadas acabarem o jogo se encerra*/
				dead =1;	/* o valor 1 indica que as jogadas acabaram*/
			}	

			jogadas_restantes = MAX_JOGADAS - i_geral;
			jogadas_utilizadas = MAX_JOGADAS - jogadas_restantes;

			if (dead == 1){
				printa_arena();
				printf("%s GAME OVER\n\n", VERMELHO);
				printf("Seu numero de jogadas acabou\n\n");
				printf("%s Total de Pontos: %d\n\n", AMARELO, pontos);
				printf("%s Jogadas efetuadas: %d\n\n", AMARELO, jogadas_utilizadas);
			}else if(dead == 2){
				printa_arena();
				printf("%s GAME OVER\n\n", VERMELHO);
				printf("%s Voce morreu para o inimigo 'X'\n\n", VERMELHO);
				printf("%s Total de Pontos: %d\n\n", AMARELO, pontos);
				printf("%s Jogadas efetuadas: %d\n\n", AMARELO, jogadas_utilizadas);
			}else if(dead == 3){
				printa_arena();
				printf("%s GAME OVER\n\n", VERMELHO);
				printf("%s Voce foi explodido pelo inimigo 'Q'\n\n", ROSA);
				printf("%s Total de Pontos: %d\n\n", AMARELO, pontos);
				printf("%s Jogadas efetuadas: %d\n\n", AMARELO, jogadas_utilizadas);
			}else if(dead == 4){
				arena[cx][cy] = 'C';
				printa_arena();
				printf("%s GAME OVER\n\n", VERDE);
				printf("%s Parabens! Voce capturou todos os pontos\n\n", VERDE);
				printf("%s Total de Pontos: %d\n", AMARELO, pontos);
				printf("%s Jogadas efetuadas: %d\n\n", AMARELO, jogadas_utilizadas); /* faz o calculo de jogadas que foram efetuadas*/
			}

			printf("%s\n", BRANCO);
			scanf("%*c");
			registerMatch();
			getchar(); 

			reseta();
			menu();

			break;
		case 2: 
			
			break;
		case 3: 
			MostraRanking();
			getchar();
			getchar();
			menu();
			break;
		case 4: 
			instrucoes();
			menu();
			break;
		case 5:
			break;	
		case 6:
			menu();
			break;	
	default:
		break;
	}

}

void telaInicial(){

	system(LIMPA);
	printf("---------------------------\n");
	printf("---------------------------\n");
	printf("--------%s Pac Point %s--------\n", AMARELO, BRANCO);
	printf("---------------------------\n");
	printf("---------------------------\n\n");
	printf("%sPressione ENTER para iniciar %s\n", VERDE, BRANCO);

	scanf("%*c");
	menu();
}

int main(){

	telaInicial();	

	return 0;
}