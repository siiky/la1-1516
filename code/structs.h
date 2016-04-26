/* =========================================================
 * Definição das Format Strings para printf e etc
 *
 *      jogador[n] := "mao[n]+(ult_jogada[n])+ncartas[n]+pontos[n]"
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
 *  Imaginando que o tipo `MAO` e uma matriz e seja `MAO e;`
 *  O valor de uma posicao `(n,v)` na matriz `e` e dado por
 *
 *      e[n][v]         =>      (e & ((MAO) 1 << INDICE(N, V)))
 *
 *  Pra escrever numa posicao `(n,v)` da matriz `e`
 *
 *      e[n][v] = 0     =>      (e & ~((MAO) 1 << INDICE(N, V)))
 *      e[n][v] = 1     =>      (e |  ((MAO) 1 << INDICE(N, V)))
 * =========================================================
 *          | 0123456789ABC | DEF
 *       ---|---------------|-----|----------
 *        0 | ............. | ... | Diamonds
 *        1 | ............. | ... | Clubs
 *        2 | ............. | ... | Heart
 *        3 | ............. | ... | Spades
 *       ---|---------------|-----|----------
 *          | 34567891JQKA2 |
 *                   0
 */

/* comprimento máximo das strings */
#define MAXLEN          10240
#define TERNOS          ((MAO) 1 << 3)

#define PROX_JOG(J)             ((J + 1) % 4)
#define JOG_ANT(J)              ((J + 3) % 4)
#define INDICE_NAIPE(N, V)      (V + (N * 13))      /* ordenado por naipe (do stor) */
#define INDICE(N, V)            (N + (V * 4))       /* ordenado por figuras (nossa) */
#define REM_SELECCAO(E, S)      (E & ~S)            /* remove a seleccao de cartas de um dado estado */

/* Escreve N (0 ou 1) na posicao/indice I da matriz/estado E */
#define ESCMX(E,I,N)    (N == 0) ? \
                        (rem_carta(E, I)) : \
                        (add_carta(E, I));

typedef struct {
    unsigned int naipe, valor;
} Card;

typedef unsigned long long int MAO;
typedef struct {
    MAO mao[4];
    MAO ult_jogada[4];
    MAO seleccao;
    unsigned int ncartas[4];
    unsigned int jogador;
    unsigned int ult_jogador_valido;
    unsigned int pontos[4];
} State;

/*==================================================================*/
MAO             add_carta       (const MAO *e, const unsigned int idx);
MAO             rem_carta       (const MAO *e, const unsigned int idx);
int             carta_existe    (MAO e, const unsigned int idx);
Card            mao2carta       (MAO carta);
Card*           jogada2cartas   (const MAO jogada);
State           str2estado      (const char *str);
char*           estado2str      (const State *e);
void            baralhar        (State *e);
void            initEstado      (State *e);
unsigned int    trailingZ       (MAO n);
unsigned int    bitsUm          (MAO n);
int             test_play1      (const MAO m, const MAO ult);
int             test_play2      (const MAO m, const MAO ult);
int             test_play3      (const MAO m, const MAO ult);

/*==================================================================*/
int test_play1 (const MAO m, const MAO ult)
{
    int res;
    if (ult == 0)
        res = 1;
    else
        res = m > ult;
    return res;
}

/*==================================================================*/
int test_play2 (const MAO m, const MAO ult)
{
    int res;
    Card *jogada = jogada2cartas(m);
    Card *ult_jogada = jogada2cartas(ult);

    if (jogada[0].valor != jogada[1].valor)
        res = 0;
    else if (ult == 0 || jogada[0].valor > ult_jogada[0].valor)
        res = 1;
    else if (jogada[0].valor == ult_jogada[0].valor)
        res = (jogada[1].naipe > ult_jogada[1].naipe);
    else
        res = 0;

    return res;
}

/*==================================================================*/
int test_play3 (const MAO m, const MAO ult)
{
    int res;
    Card *jogada = jogada2cartas(m);
    Card *ult_jogada = jogada2cartas(ult);

    if (jogada[0].valor != jogada[1].valor || jogada[1].valor != jogada[2].valor)
        res = 0;
    else if (ult == 0 || jogada[0].valor > ult_jogada[0].valor)
        res = 1;
    else
        res = 0;

    return res;
}

/*==================================================================*/
/*
int test_play5 (const MAO m, const MAO ult)
{
}
*/

/*==================================================================*/
unsigned int trailingZ (MAO n)
{
    unsigned int count;
    for (count = 0; n > 0 && (n ^ 1); n >>= 1, count++);
    return count;
}

/*==================================================================*/
/** \brief Devolve o número de bits a 1

@param n        Número a calcular
@return         O número de bits 1
*/
unsigned int bitsUm (MAO n)
{
    unsigned int count;
    for (count = 0; n > 0; count += n % 2, n >>= 1);
    return count;
}

/*==================================================================*/
/** \brief Adiciona uma carta ao estado

@param e        Uma mão
@param idx      O indice da carta (inteiro entre 0 e 51)
@return         A nova mão
*/
MAO add_carta (const MAO *e, const unsigned int idx)
{
    return (*e | ((MAO) 1 << idx));
}

/*==================================================================*/
/** \brief Remove uma carta do estado

@param e        Uma mão
@param idx      O indice da carta (inteiro entre 0 e 51)
@return         A nova mão
*/
MAO rem_carta (const MAO *e, const unsigned int idx)
{
    return (*e & ~((MAO) 1 << idx));
}

/*==================================================================*/
/** \brief Verifica se uma carta pertence ao estado

@param e        Uma mão
@param idx      O indice da carta (inteiro entre 0 e 51)
@return         1 se a carta existe, 0 caso contrário
*/
int carta_existe (MAO e, const unsigned int idx)
{
    return ((e >> idx) & ((MAO) 1));
}

/*==================================================================*/
Card mao2carta (MAO carta)
{
    Card c;
    for (c.valor = 0; carta > TERNOS; c.valor++)
        carta >>= 4;
    for (c.naipe = 0; !(carta_existe(carta, 0)); carta >>= 1)
        c.naipe++;
    return c;
}

/*==================================================================*/
/** \brief Devolve uma lista de cartas (pares naipes/figuras)

@param jogada   A jogada a converter
@return         Os pares naipe/figura ordenados por figuras
*/
Card* jogada2cartas (const MAO jogada)
{
    unsigned int i, w;
    unsigned int b = bitsUm(jogada);
    static Card cartas[13];
    for (i = w = 0; i < 52 && w < b; i++)
        if (carta_existe(jogada, i))
            cartas[w++] = mao2carta((MAO) 1 << i);

    cartas[w].naipe = 20;
    cartas[w].valor = 20;
    return cartas;
}

/*==================================================================*/
State str2estado (const char *str)
{
    State e;
    sscanf(str, "q="
        "%llu+%llu+%u+%u_"
        "%llu+%llu+%u+%u_"
        "%llu+%llu+%u+%u_"
        "%llu+%llu+%u+%u_"
        "%u_%llu_%u",
        &e.mao[0], &e.ult_jogada[0], &e.ncartas[0], &e.pontos[0],
        &e.mao[1], &e.ult_jogada[1], &e.ncartas[1], &e.pontos[1],
        &e.mao[2], &e.ult_jogada[2], &e.ncartas[2], &e.pontos[2],
        &e.mao[3], &e.ult_jogada[3], &e.ncartas[3], &e.pontos[3],
        &e.jogador, &e.seleccao, &e.ult_jogador_valido
    );
    return e;
}

/*==================================================================*/
char* estado2str (const State *e)
{
    static char str[MAXLEN];
    sprintf(str,
        "%llu+%llu+%u+%u_"
        "%llu+%llu+%u+%u_"
        "%llu+%llu+%u+%u_"
        "%llu+%llu+%u+%u_"
        "%u_%llu_%u",
        e->mao[0], e->ult_jogada[0], e->ncartas[0], e->pontos[0],
        e->mao[1], e->ult_jogada[1], e->ncartas[1], e->pontos[1],
        e->mao[2], e->ult_jogada[2], e->ncartas[2], e->pontos[2],
        e->mao[3], e->ult_jogada[3], e->ncartas[3], e->pontos[3],
        e->jogador, e->seleccao, e->ult_jogador_valido
    );
    return str;
}

/*==================================================================*/
/** \brief Dá as cartas a cada jogador no início do jogo

@param e        O estado actual do jogo
@return e       O novo estado do jogo
*/
void baralhar (State *e)
{
    int j;      /* jogador */
    int i;

    for (i = 0; i < 52; i++) {
        do j = random() % 4; while (e->ncartas[j] >= 13);
        e->mao[j] = add_carta(&e->mao[j], i);
        e->ncartas[j]++;
    }
}

/*==================================================================*/
void initEstado (State *e)
{
    int i;
    e->seleccao = (MAO) 0;
    for (i = 0; i < 4; i++) {
        e->mao[i] = (MAO) 0;
        e->ult_jogada[i] = (MAO) 0;
        e->ncartas[i] = 0;
        e->pontos[i] = 0;
    }
    baralhar(e);
    for (i = 0; i < 4 && (e->mao[i] % 2 == 0); i++);
    e->ult_jogador_valido = e->jogador = i;
}
