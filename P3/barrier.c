/*
// Biblioteca de barreiras alocadas dinamicamente, versao 1
// Sistemas Operativos
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "barrier.h"
#include "matrix2d.h"

/*--------------------------------------------------------------------
| Global external variables
---------------------------------------------------------------------*/
extern int flag;
extern double diftotal;
extern int count_iter;

/*--------------------------------------------------------------------
| Function: initBarrier
---------------------------------------------------------------------*/

int initBarrier(barrier *bar) {

    if (pthread_mutex_init(&(bar->mutex),NULL) !=0){
	fprintf(stderr, "\nErro ao inicializar mutex\n");
	exit(1);
    }

    if(pthread_cond_init(&(bar->waitTasks), NULL) != 0) {
	fprintf(stderr, "\nErro ao inicializar variável de condição\n");
	exit(1);
    }
    return 0;
}

/*--------------------------------------------------------------------
| Function: waitBarrier
---------------------------------------------------------------------*/

int waitBarrier(void * args, int iter ,double difmax) {

	thread_info *tinfo = (thread_info *) args;

    if(pthread_mutex_lock(&(tinfo->bar->mutex)) != 0) {
	fprintf(stderr, "\nErro ao bloquear mutex\n");
	exit(1);
    }
    count_iter++;
    if (difmax > diftotal)
    	diftotal = difmax ;

    /*Verificar quando passa a ultima tarefa*/
    if (tinfo->contadores[iter] == tinfo->trab) {
    	if (diftotal < tinfo->maxD)
    		flag = 1;

		if (pthread_cond_broadcast(&(tinfo->bar->waitTasks)) != 0) {
	    	fprintf(stderr, "\nErro ao desbloquear variável de condição\n");
	    	exit(1);
		}
	}
	
	else

		/*Enquanto nao e a ultima tarefa,espera.*/ 
		while (tinfo->contadores[iter] != tinfo->trab){
	    	tinfo->contadores[iter]++; 
	    	if(pthread_cond_wait(&(tinfo->bar->waitTasks), &(tinfo->bar->mutex)) != 0) {  
				fprintf(stderr, "\nErro ao esperar pela variável de condição\n");
				exit(1);
	    	}
		}

    if(pthread_mutex_unlock(&(tinfo->bar->mutex)) != 0) {
	fprintf(stderr, "\nErro ao desbloquear mutex\n");
	exit(1);
    }
    return 0;
}

/*--------------------------------------------------------------------
| Function: destroyBarrier
---------------------------------------------------------------------*/

void destroyBarrier(barrier *bar) {

    if(pthread_mutex_destroy(&(bar->mutex)) != 0) {
	fprintf(stderr, "\nErro ao destruir mutex\n");
	exit(1);
    }

    if(pthread_cond_destroy(&(bar->waitTasks)) != 0) {
	fprintf(stderr, "\nErro ao destruir variável de condição\n");
	exit(1);
    }    
}


















