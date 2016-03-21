#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* =========================================================
 * Definicao das Format Strings pra printf e etc
 *
 *      jogador[n] := "mao[n]+(ult_jogada[n])+ncartas[n]"
 *
 *      "jogador[0]_jogador[1]_jogador[2]_jogador[3]_(ult_jogador)_(sel_cartas)"
 *
 * =========================================================
 */

/* URL da CGI */
#define SCRIPT          "http://127.0.0.1/cgi-bin/cartas"

/* URL da pasta com as cartas */
#define BARALHO         "http://127.0.0.1/cards"

/* Ordem dos naipes */
#define NAIPES          "DCHS"

/* Ordem das cartas */
#define VALORES         "3456789TJQKA2"

/* valores usados pela funcao imprime */
#define COR_TABULEIRO 116611    /* RGB em HEX */
#define XC_INIT 10              /* x inicial pra cartas */
#define YC_INIT 10              /* y inicial pra cartas */
#define XC_STEP 20              /* salto do x pra cartas */
#define YC_STEP 150             /* salto do y pra cartas */
#define YC_SEL_STEP 10          /* salto de cartas seleccionadas */
#define YJ_INIT 0               /* y inicial pra jogador */
#define YJ_STEP 150             /* salto do y pra jogador */

/* definicoes do botao jogar */
#define SVG_WIDTH 150
#define SVG_HEIGHT 200
#define COR_R 200
#define COR_G 150
#define COR_B 100
#define RECT_X 50
#define RECT_Y 50
#define RECT_WIDTH 100
#define RECT_HEIGHT 50
#define TXT_X 100
#define TXT_Y 80

/* comprimento max de strings */
#define MAXLEN 10240
#define ULLI unsigned long long int

/**
Estado inicial com todas as 52 cartas do baralho
Cada carta é representada por um bit que está
a 1 caso ela pertença à mão ou 0 caso contrário
*/
const ULLI ESTADO_INICIAL = 0xfffffffffffff;

/** \brief Devolve o numero de bits 1

@param n        Numero do qual queremos saber quantos bits 1 tem
@return         O numero de bits a 1
*/
unsigned int bitsUm (ULLI n)
{
    unsigned int res;
    for (res = 0; n > 0; n >>= 1)
        res += n % 2;
    return res;
}

/** \brief Devolve o índice da carta

@param naipe    O naipe da carta (inteiro entre 0 e 3)
@param valor    O valor da carta (inteiro entre 0 e 12)
@return         O índice correspondente à carta
*/
int indice (const int naipe, const int valor)
{
    return naipe * 13 + valor;
}

/** \brief Adiciona uma carta ao estado

@param ESTADO   O estado atual
@param naipe    O naipe da carta (inteiro entre 0 e 3)
@param valor    O valor da carta (inteiro entre 0 e 12)
@return         O novo estado
*/
ULLI add_carta (const ULLI ESTADO, const int naipe, const int valor)
{
    int idx = indice(naipe, valor);
    return ESTADO | ((ULLI) 1 << idx);
}

/** \brief Remove uma carta do estado

@param ESTADO   O estado atual
@param naipe    O naipe da carta (inteiro entre 0 e 3)
@param valor    O valor da carta (inteiro entre 0 e 12)
@return         O novo estado
*/
ULLI rem_carta (const ULLI ESTADO, const int naipe, const int valor)
{
    int idx = indice(naipe, valor);
    return ESTADO & ~((ULLI) 1 << idx);
}

/** \brief Remove as cartas seleccionadas do estado

@param ESTADO           O estado actual
@param sel_cartas       As cartas seleccionadas
@return                 O novo estado 
*/
ULLI rem_sel_cartas (const ULLI ESTADO, const ULLI sel_cartas)
{
    return (ESTADO ^ sel_cartas);
}

/** \brief Verifica se uma carta pertence ao estado

@param ESTADO   O estado atual
@param naipe    O naipe da carta (inteiro entre 0 e 3)
@param valor    O valor da carta (inteiro entre 0 e 12)
@return         1 se a carta existe e 0 caso contrário
*/
int carta_existe (const ULLI ESTADO, const int naipe, const int valor)
{
    int idx = indice(naipe, valor);
    ULLI estado = ESTADO;
    return (estado >> idx) & 1;
}

/** \brief Imprime o botao Jogar

@param mao[]            O array com as maos de todos os jogadores
@param ncartas[]        O array com o numero de cartas que cada jogador tem
@param ult_jogador      O jogador anterior
@param ult_jogada[]     Array com a ultima jogada de cada jogador
@param sel_cartas       As cartas seleccionadas pelo jogador
*/
void imprime_botao_jogar (const ULLI mao[], const unsigned int ncartas[], const int ult_jogador, const ULLI ult_jogada[], const ULLI sel_cartas)
{
    char link[MAXLEN];

    /*
    if (jogada_valida(sel_cartas)) {
    */
    sprintf(link, "%s?q=%llu+%llu+%u_%llu+%llu+%u_%llu+%llu+%u_%llu+%llu+%u_%d_%llu", SCRIPT, rem_sel_cartas(mao[0], sel_cartas), sel_cartas, (ncartas[0]-bitsUm(sel_cartas)), mao[1], ult_jogada[1], ncartas[1], mao[2], ult_jogada[2], ncartas[2], mao[3], ult_jogada[3], ncartas[3], (ult_jogador+1), (ULLI) 0);

    printf("<svg width=%d height=%d>\n", SVG_WIDTH, SVG_HEIGHT);
    printf("<a xlink:href = \"%s\">\n", link);
    printf("<rect x=%d y=%d width=%d height=%d ry=5 fill=\"rgb(%d, %d, %d)\" />\n", RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_R, COR_G, COR_B);
    printf("<text x=%d y=%d text-anchor = \"midle\" text-alignt = \"center\" font-family = \"serif\" font-weight = \"bold\">Jogar</text></a></svg>\n", TXT_X, TXT_Y);
    /*
    } else {
    }
    */
}

/** \brief Imprime o botao Limpar

@param mao[]            O array com as maos de todos os jogadores
@param ncartas[]        O array com o numero de cartas que cada jogador tem
@param ult_jogador      O jogador anterior
@param ult_jogada[]     Array com a ultima jogada de cada jogador
*/
void imprime_botao_limpar (const ULLI mao[], const unsigned int ncartas[], const int ult_jogador, const ULLI ult_jogada[])
{
    char link[MAXLEN];
    sprintf(link, "%s?q=%llu+%llu+%u_%llu+%llu+%u_%llu+%llu+%u_%llu+%llu+%u_%d_%llu", SCRIPT, mao[0], ult_jogada[0], ncartas[0], mao[1], ult_jogada[1], ncartas[1], mao[2], ult_jogada[2], ncartas[2], mao[3], ult_jogada[3], ncartas[3], (ult_jogador+1), (ULLI) 0);

    printf("<svg width=%d height=%d>\n", SVG_WIDTH, SVG_HEIGHT);
    printf("<a xlink:href = \"%s\">\n", link);
    printf("<rect x=%d y=%d width=%d height=%d ry=5 fill=\"rgb(%d, %d, %d)\" />\n", RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_R, COR_G, COR_B);
    printf("<text x=%d y=%d text-anchor = \"midle\" text-alignt = \"center\" font-family = \"serif\" font-weight = \"bold\">Limpar</text></a></svg>\n", TXT_X, TXT_Y);
}

/** \brief Imprime o html correspondente a uma carta

@param path             O URL correspondente à pasta que contém todas as cartas
@param x                A coordenada x da carta
@param y                A coordenada y da carta
@param mao[]            O estado atual
@param naipe            O naipe da carta (inteiro entre 0 e 3)
@param valor            O valor da carta (inteiro entre 0 e 12)
@param ult_jogada[]     Ultima jogada de cada jogador
@param ncartas[]        Numero de cartas de cada jogador
@param ult_jogador      O jogador anterior
@param sel_cartas       As cartas seleccionadas pelo jogador
*/
void imprime_carta (const char *path, const int x, int y, const ULLI mao[], const int naipe, const int valor, const ULLI ult_jogada[], const unsigned int ncartas[], const int ult_jogador, const ULLI sel_cartas)
{
    char script[MAXLEN];
    if (carta_existe(sel_cartas, naipe, valor)) y += YC_SEL_STEP;
    sprintf(script, "%s?q=%llu+%llu+%u_%llu+%llu+%u_%llu+%llu+%u_%llu+%llu+%u_%d_%llu", SCRIPT, mao[0], ult_jogada[0], ncartas[0], mao[1], ult_jogada[1], ncartas[1], mao[2], ult_jogada[2], ncartas[2], mao[3], ult_jogada[3], ncartas[3], ult_jogador, add_carta(sel_cartas, naipe, valor));
    printf("<a xlink:href=\"%s\"><image x=\"%d\" y=\"%d\" height=\"110\" width=\"80\" xlink:href=\"%s/%c%c.svg\"/></a>\n", script, x, y, path, VALORES[valor], NAIPES[naipe]);
}

/** \brief Imprime o estado do jogo

Esta função está a imprimir o estado em quatro linhas: uma para cada jogador
@param *path            O URL correspondente à pasta que contém todas as cartas
@param mao[]            O estado atual
@param ult_jogada       A ultima jogada de cada jogador
@param ult_jogador      O jogador anterior
@param ncartas[]        O numero de cartas de cada jogador
@param sel_cartas       As cartas seleccionadas pelo jogador
*/
void imprime (const char *path, const ULLI mao[], const ULLI ult_jogada[], const int ult_jogador, const unsigned int ncartas[], const ULLI sel_cartas)
{
    int n;                      /* naipe */
    int v;                      /* valor */
    /* int j; */                /* jogador */
    int xc = XC_INIT;           /* x inicial */
    int yc = YC_INIT;           /* y inicial */
    int yj = 0;                 /* tabuleiros dos jogadores */

    printf("<svg height = \"200\" width = \"400\">\n");
    /* for (j = 0; j < 4; yj += YC_STEP, yc += YC_STEP, j++) { */
    /* printf("Jogador %d\n", j); */
    printf("<rect x=\"0\" y=\"%d\" height=\"130\" width=\"400\" style=\"fill:#%d\"/>\n", yj, COR_TABULEIRO);
    printf("<svg height = \"800\" width = \"800\">\n");
    for (xc = 10, n = 0; n < 4; n++) {
        for (v = 0; v < 13; v++) {
            if (carta_existe(mao[0], n, v)) {
                    xc += XC_STEP;
                    imprime_carta(path, xc, yc, mao, n, v, ult_jogada, ncartas, ult_jogador, sel_cartas);
            }
        }
    }
    printf("</svg>\n");
    /* } */
    printf("</svg>\n");
    imprime_botao_jogar(mao, ncartas, ult_jogador, ult_jogada, sel_cartas);
    imprime_botao_limpar(mao, ncartas, ult_jogador, ult_jogada);
}

/** \brief Da as cartas a cada jogador no inicio do jogo

@param mao[]            As maos de cada jogador
@param ncartas[]        O numero de cartas de cada jogador
*/
void baralhar (ULLI mao[], unsigned int ncartas[])
{
    int n;      /* naipe */
    int v;      /* valor */
    int j;      /* jogador */

    for (n = 0; n < 4; n++)
        for (v = 0; v < 13; v++) {
            do {
                j = random() % 4;
            } while (ncartas[j] >= 13);
            mao[j] = add_carta(mao[j], n, v);
            ncartas[j]++;
        }
}

/** \brief Trata os argumentos da CGI

Esta função recebe a query que é passada à cgi-bin e trata-a.
Neste momento, a query contém o estado que é um inteiro que representa um conjunto de cartas.
Cada carta corresponde a um bit que está a 1 se essa carta está no conjunto e a 0 caso contrário.
Caso não seja passado nada à cgi-bin, ela assume que todas as cartas estão presentes.
@param query    A query que é passada à cgi-bin
*/
void parse (char *query)
{
    ULLI mao[4] = {0};                /* comecam todas vazias */
    ULLI ult_jogada[4] = {0};         /* comecam todas vazias */
    ULLI sel_cartas = 0;              /* cartas seleccionadas pelo jogador */
    unsigned int ncartas[4] = {0};    /* jogadores comecam com 0 cartas */
    int ult_jogador = -1;             /* ultimo jogador */

    if (sscanf(query, "q=%llu+%llu+%u_%llu+%llu+%u_%llu+%llu+%u_%llu+%llu+%u_%d_%llu", &mao[0], &ult_jogada[0], &ncartas[0], &mao[1], &ult_jogada[1], &ncartas[1], &mao[2], &ult_jogada[2], &ncartas[2], &mao[3], &ult_jogada[3], &ncartas[3], &ult_jogador, &sel_cartas) == 1) {
        imprime(BARALHO, mao, ult_jogada, ult_jogador, ncartas, sel_cartas);
    } else {
        baralhar(mao, ncartas);
        imprime(BARALHO, mao, ult_jogada, ult_jogador, ncartas, sel_cartas);
    }
}

/** \brief Função principal

Função principal do programa que imprime os cabeçalhos necessários e depois disso invoca
a função que vai imprimir o código html para desenhar as cartas
*/
int main (void)
{
    srandom(time(NULL));
    /* Cabeçalhos necessários numa CGI */
    printf("Content-Type: text/html; charset=utf-8\n\n");
    printf("<header><title>Big Two</title></header>\n");
    printf("<body>\n");
    printf("<h1>Big Two</h1>\n");
    /* Ler os valores passados à cgi que estão na variável ambiente e passá-los ao programa */
    parse(getenv("QUERY_STRING"));
    printf("</body>\n");
    return 0;
}
