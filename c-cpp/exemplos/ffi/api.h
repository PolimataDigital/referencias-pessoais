#ifndef REF_API_H
#define REF_API_H
#ifdef __cplusplus
extern "C" {
#endif

/* 1 no sucesso; 0 para ponteiro nulo ou overflow. Saida preservada no erro. */
int ref_somar(int a, int b, int *saida);

#ifdef __cplusplus
}
#endif
#endif
