#define PROX_JOG(J)             ((J + 1) % 4)
#define JOG_ANT(J)              ((J + 3) % 4)

void    bot_joga        (State *e);
int     bot_play1       (State *e);
int     bot_play23      (State *e, unsigned int N);
int     escolhe_jogada  (State *e);

/*==================================================================*/
void bot_joga (State *e)
{
    unsigned int nb;
    int r = 0;

    if (e->ult_jogador_valido == e->jogador) { /* pode jogar qq coisa */
        r = escolhe_jogada(e);
    } else {
        nb = bitsUm(e->ult_jogada[e->ult_jogador_valido]);
        switch (nb) {
            case PLAY_FIVE:
                printf("<P>fuck you creator, you havent taught me how to play that!</P>\n");
                break;
            case PLAY_TRIPLE:
            case PLAY_PAIR:
                r = bot_play23(e, nb);
                break;
            case PLAY_SINGLE:
                r = bot_play1(e);
                break;
        }
    }

    if (r != 0) { /* jogou */
        e->ult_jogador_valido = e->jogador;
        e->ncartas[e->jogador] = update_ncartas(e->ncartas[e->jogador], e->ult_jogada[e->jogador]);
    } else { /* passou */
        e->ult_jogada[e->jogador] = (MAO) 0;
    }
    e->jogador = PROX_JOG(e->jogador);
}

/*==================================================================*/
int bot_play1 (State *e)
{
    MAO a_jogar = 0;
    unsigned int idx, i;
    int r = 0;

    if (e->jogador == e->ult_jogador_valido) {
        idx = trailingZ(e->mao[e->jogador]);
        a_jogar = (MAO) 1 << idx;
    } else {
        idx = trailingZ(e->ult_jogada[e->ult_jogador_valido]);
        for (i = idx; i < 52 && !(carta_existe(e->mao[e->jogador], i)); i++);
        a_jogar = ((carta_existe(e->mao[e->jogador], i)) ?
                  (MAO) 1 << i :
                  (MAO) 0);
    }

    if (a_jogar != (MAO) 0) {
        e->mao[e->jogador] = REM_SELECCAO(e->mao[e->jogador], a_jogar);
        r = 1;
    }
    e->ult_jogada[e->jogador] = a_jogar;

    return r;
}

/*==================================================================*/
int bot_play23 (State *e, unsigned int N)
{
    unsigned int i, n;
    unsigned int idx = 0;
    int r = 0;
    MAO a_jogar = 0;
    MAO mao = e->mao[e->jogador];
    CardsCount c;

    if (e->jogador != e->ult_jogador_valido)
        idx = trailingZ(e->ult_jogada[e->ult_jogador_valido]);

    for (i = 0; i < idx; i++)
        mao = rem_carta(&mao, i);

    conta_cartas(&c, mao);
    for (i = 0; i < 13 && c.valores[i] < N; i++);
    if (c.valores[i] >= N)
        for (n = 0; n < 4 && (N > 0); n++) {
            idx = INDICE(n, i);
            if (carta_existe(mao, idx)) {
                a_jogar = add_carta(&mao, idx);
                N--;
            }
        }

    if (a_jogar > (MAO) 0) {
        e->mao[e->jogador] = REM_SELECCAO(e->mao[e->jogador], a_jogar);
        r = 1;
    }
    e->ult_jogada[e->jogador] = a_jogar;

    return r;
}

/*==================================================================*/
int escolhe_jogada (State *e)
{
    int r, i;
    for (i = 3, r = 0; i > 0 && r == 0; i--)
        switch (i) {
            case 1:
                r = bot_play1(e);
                break;
            case 2:
                r = bot_play23(e, 2);
                break;
            case 3:
                r = bot_play23(e, 3);
                break;
            /*case 4:
                r = bot_play5(e);
                break;*/
        }
    return r;
}