/*
// Biblioteca de barreiras alocadas dinamicamente, versao 1
// Sistemas Operativos
*/

#ifndef BARRIER
#define BARRIER
#include "matrix2d.h"

typedef struct 
{
    pthread_cond_t waitTasks;
    pthread_mutex_t mutex;	
}barrier;

int initBarrier(barrier *bar);
int waitBarrier(void *args,int iter, double difmax); 
void destroyBarrier(barrier *bar); 

typedef struct {
    int id;
    int N;
    int iter;
    int trab;
    int tam_fatia;
    int flag;
    double maxD;
    barrier *bar;
    int *contadores;
    DoubleMatrix2D *matrix, *matrix_aux;
}thread_info;

#endif
