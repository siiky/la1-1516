/* =========================================================
 * Definição das Format Strings para printf e etc
 *
 *      jogador[n] := "mao[n]+(ult_jogada[n])+ncartas[n]"
 *
 *      "jogador[0]_jogador[1]_jogador[2]_jogador[3]_(jogador)_(seleccao)_(ult_jogador_valido)"
 *
 * =========================================================
 * Última jogada:
 *
 *   == 0 -> PASSOU
 *   != 0 -> JOGOU
 *
 * =========================================================
 */

typedef struct card {
    unsigned int naipe, valor;
} CARTA;

typedef unsigned long long int MAO;
typedef struct state {
    MAO mao[4];
    MAO ult_jogada[4];
    MAO seleccao;
    unsigned int ncartas[4];
    unsigned int jogador;
    unsigned int ult_jogador_valido;
} ESTADO;

/*============================================================================*/
void bot_joga (ESTADO *e);
void parse (char *query);
CARTA mao2carta (MAO carta);
CARTA* jogada2cartas (MAO jogada);
int valores_iguais (CARTA cartas[]);
unsigned int trailingZ (MAO n);
unsigned int bitsUm (MAO n);
int jogada_valida (const MAO jogada, const MAO ult_jogada);
MAO add_carta (const MAO e, const int naipe, const int valor);
MAO rem_carta (const MAO e, const int naipe, const int valor);
int carta_existe (const MAO e, const int naipe, const int valor);
void imprime_bjogar (ESTADO e);
void imprime_blimpar (ESTADO e);
void imprime_carta (const char *path, const int x, int y, ESTADO e, const CARTA c);
void imprime (const char *path, const ESTADO *e);
/*============================================================================*/

/*----------------------------------------------------------------------------*/
ESTADO str2estado (const char *str)
{
    ESTADO e;
    sscanf(str, "q="
        "%llu+%llu+%u_"
        "%llu+%llu+%u_"
        "%llu+%llu+%u_"
        "%llu+%llu+%u_"
        "%u_%llu_%u",
        &e.mao[0], &e.ult_jogada[0], &e.ncartas[0],
        &e.mao[1], &e.ult_jogada[1], &e.ncartas[1],
        &e.mao[2], &e.ult_jogada[2], &e.ncartas[2],
        &e.mao[3], &e.ult_jogada[3], &e.ncartas[3],
        &e.jogador, &e.seleccao, &e.ult_jogador_valido
    );
    return e;
}

/*----------------------------------------------------------------------------*/
char* estado2str (const ESTADO *e)
{
    static char str[MAXLEN];
    sprintf(str,
        "%llu+%llu+%u_"
        "%llu+%llu+%u_"
        "%llu+%llu+%u_"
        "%llu+%llu+%u_"
        "%u_%llu_%u",
        e->mao[0], e->ult_jogada[0], e->ncartas[0],
        e->mao[1], e->ult_jogada[1], e->ncartas[1],
        e->mao[2], e->ult_jogada[2], e->ncartas[2],
        e->mao[3], e->ult_jogada[3], e->ncartas[3],
        e->jogador, e->seleccao, e->ult_jogador_valido
    );
    return str;
}

/*----------------------------------------------------------------------------*/
/** \brief Dá as cartas a cada jogador no início do jogo

@param e        O estado actual do jogo
@return e       O novo estado do jogo
*/
void baralhar (ESTADO *e)
{
    int n;      /* naipe */
    int v;      /* valor */
    int j;      /* jogador */

    for (n = 0; n < 4; n++)
        for (v = 0; v < 13; v++) {
            do j = random() % 4; while (e->ncartas[j] >= 13);
            e->mao[j] = add_carta(e->mao[j], n, v);
            e->ncartas[j]++;
        }
    for (j = 0; j < 4 && (e->mao[j] % 2 != 1); j++);
    e->jogador = j;
}

/*----------------------------------------------------------------------------*/
void initEstado (ESTADO *e)
{
    int i;
    e->seleccao = 0;                     /* cartas selecionadas pelo jogador */
    e->jogador = 0;                      /* jogador actual */
    e->ult_jogador_valido = -1;          /* último jogador a jogar uma jogada valida */
    for (i = 0; i < 4; i++) {
        e->mao[i] = 0;                   /* começam todas vazias */
        e->ult_jogada[i] = 0;            /* começam todas vazias */
        e->ncartas[i] = 0;               /* jogadores começam com 0 cartas */
    }
    baralhar(e);
}
