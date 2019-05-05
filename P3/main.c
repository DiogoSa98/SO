/*
// Projeto SO - exercicio 3, version 01 
// Sistemas Operativos, Grupo 56
// Diogo Sa 87652
// Antonio Machado Santos 87632
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#include "matrix2d.h"
#include "barrier.h"

/*--------------------------------------------------------------------
| Global variables
---------------------------------------------------------------------*/
double diftotal = 0;
int flag = 0;
int count_iter = 0;
/*--------------------------------------------------------------------
| Function: parse_integer_or_exit
---------------------------------------------------------------------*/

int parse_integer_or_exit(char const *str, char const *name)
{
    int value;

    if(sscanf(str, "%d", &value) != 1) {
	fprintf(stderr, "\nErro no argumento \"%s\".\n\n", name);
	exit(1);
    }
    return value;

}

/*--------------------------------------------------------------------
| Function: parse_double_or_exit
---------------------------------------------------------------------*/

double parse_double_or_exit(char const *str, char const *name)
{
    double value;

    if(sscanf(str, "%lf", &value) != 1) {
	fprintf(stderr, "\nErro no argumento \"%s\".\n\n", name);
	exit(1);
    }
    return value;
}

/*--------------------------------------------------------------------
| Function: destroymutex
| Description: 
| Destroi o mutex recebido.
---------------------------------------------------------------------*/

void destroymutex(pthread_mutex_t mutex) {
	   if(pthread_mutex_destroy(&mutex) != 0) {
	fprintf(stderr, "\nErro ao destruir mutex!!!\n");
	exit(1);
    }
}


/*--------------------------------------------------------------------
| Function: tarefa_trabalhadora
| Description: Função executada por cada tarefa trabalhadora.
|              Recebe como argumento uma estrutura do tipo thread_info.
---------------------------------------------------------------------*/

void *tarefa_trabalhadora(void* args){
    thread_info *tinfo = (thread_info *) args;
    int iter;
    int j, i, linhai, linhaf;
    double difmax;
    double val , dif;
    DoubleMatrix2D *m, *aux, *tmp;
    pthread_mutex_t mutex ;
     
    /* Define primeira linha da fatia */
    if(tinfo->id != 0)
		linhai = tinfo->id*tinfo->tam_fatia;
    else
		linhai = 0;
        
    /* Calcula ultima linha da fatia */
	linhaf = (1 + tinfo->id) * tinfo->tam_fatia;

    m = tinfo->matrix;
  	aux = tinfo->matrix_aux;

  	if (pthread_mutex_init(&mutex,NULL) !=0){
		fprintf(stderr, "\nErro ao inicializar mutex\n");
		exit(1);
    }

    /* Ciclo Iterativo */
    for (iter = 0; iter < tinfo->iter;iter++) {
   	if(pthread_mutex_lock(&mutex) != 0) {
		fprintf(stderr, "\nErro ao bloquear mutex\n");
		exit(1);
	}
    	difmax = 0;

	/* Calcular Pontos Internos */
	for (i = linhai; i < linhaf; i++) 
	    for (j = 0; j < tinfo->N; j++) {
		  val = (dm2dGetEntry(m, i, j+1) +
			dm2dGetEntry(m, i+2, j+1) +  
			dm2dGetEntry(m, i+1, j) +    
			dm2dGetEntry(m, i+1, j+2))/4;
		  dm2dSetEntry(aux, i+1, j+1, val);
        dif = val - dm2dGetEntry(m, i+1, j+1);

        if (dif > difmax)
        	difmax = dif;
		}
	tmp = aux;
    aux = m;
    m = tmp;

    if(pthread_mutex_unlock(&mutex) != 0) {
		fprintf(stderr, "\nErro ao bloquear mutex\n");
		exit(1);
	}

	waitBarrier(tinfo, iter, difmax);

    if (flag == 1){
   		break;
   	}
   		
   	diftotal = 0;

    }

    destroymutex(mutex);
    pthread_exit(NULL);
}

/*--------------------------------------------------------------------
| Function: main
---------------------------------------------------------------------*/

int main (int argc, char** argv) {

    int N;
    double tEsq, tSup, tDir, tInf;
    double maxD;
    int iter;
    int trab;
    int res, tam_fatia;
    int i, e;
    int *contadores;
    DoubleMatrix2D  *matrix, *matrix_aux;
    thread_info *tinfo;
    pthread_t *trabalhadoras; 
    barrier *bar;


    if(argc != 9) {
	fprintf(stderr, "\nNumero invalido de argumentos.\n");
	fprintf(stderr, "Uso: heatSim N tEsq tSup tDir tInf iteracoes trabalhadoras maxD\n\n");

	return 1;
    }

    /* argv[0] = program name */
    
    N = parse_integer_or_exit(argv[1], "N");
    tEsq = parse_double_or_exit(argv[2], "tEsq");
    tSup = parse_double_or_exit(argv[3], "tSup");
    tDir = parse_double_or_exit(argv[4], "tDir");
    tInf = parse_double_or_exit(argv[5], "tInf");
    iter = parse_integer_or_exit(argv[6], "iter");
    trab = parse_integer_or_exit(argv[7], "trab");
    maxD = parse_double_or_exit(argv[8], "maxD");

    /* Verificacoes de input*/
    fprintf(stderr, "\nArgumentos:\n"
	    " N=%d tEsq=%.1f tSup=%.1f tDir=%.1f tInf=%.1f iter=%d trab=%d maxD=%.1f\n" ,
	    N, tEsq, tSup, tDir, tInf, iter, trab , maxD);

    if(N < 1 || tEsq < 0 || tSup < 0 || tDir < 0 || tInf < 0 || iter < 1 || trab < 0 || maxD <= 0 ) {
	fprintf(stderr, "\nErro: Argumentos invalidos.\n"
		" Lembrar que N >= 1, temperaturas >= 0 , iteracoes >= 1 e MaxD > 0\n\n");
	return 1;
    }

    if (N % trab != 0) {
	fprintf(stderr, "\nErro: Argumento %s e %s invalidos\n"
		"%s deve ser multiplo de %s.", "N", "trab", "N", "trab");
	return -1;
    }

    tam_fatia = N / trab;

    matrix = dm2dNew(N+2, N+2);
    matrix_aux = dm2dNew(N+2, N+2);

    if (matrix == NULL || matrix_aux == NULL) {
	fprintf(stderr, "\nErro: Nao foi possivel alocar memoria para as matrizes.\n\n");
	return -1;
    }

    for(i=0; i<N+2; i++)
	dm2dSetLineTo(matrix, i, 0);

    dm2dSetLineTo (matrix, 0, tSup);
    dm2dSetLineTo (matrix, N+1, tInf);
    dm2dSetColumnTo (matrix, 0, tEsq);
    dm2dSetColumnTo (matrix, N+1, tDir);

    dm2dCopy (matrix_aux, matrix);

    /* Reservas de Memória  */
    tinfo = (thread_info *)malloc(trab * sizeof(thread_info));
    trabalhadoras = (pthread_t *)malloc(trab * sizeof(pthread_t));
    contadores = (int*) malloc(iter * sizeof(int));
    bar = (barrier*) malloc(sizeof(barrier));     

    /* Inicializa vetor de contadores */
    for(e = 0; e < iter; e++)
	   contadores[e] = 1;

    if (tinfo == NULL || trabalhadoras == NULL || contadores == NULL || bar == NULL) {
	fprintf(stderr, "\nErro ao alocar memória para trabalhadoras .\n");    
	return -1;
    }

    /* Iniciar Barreira */
    initBarrier(bar);

    /* Criar Trabalhadoras */
    for (i = 0; i < trab; i++) {
	tinfo[i].id = i;
	tinfo[i].N = N;
	tinfo[i].iter = iter;
	tinfo[i].trab = trab;
	tinfo[i].tam_fatia = tam_fatia;
	tinfo[i].matrix = matrix;
	tinfo[i].matrix_aux = matrix_aux;
	tinfo[i].bar = bar;
	tinfo[i].contadores = contadores;
    tinfo[i].maxD = maxD;

	res = pthread_create(&trabalhadoras[i], NULL, tarefa_trabalhadora, &tinfo[i]);

	if(res != 0) {
	    fprintf(stderr, "\nErro ao criar uma tarefa trabalhadora.\n");
	    return -1;
    	}
    } 
    
    /* Esperar que todas as tarefas acabem */
    for (i = 0; i < trab; i++) {
	res = pthread_join(trabalhadoras[i], NULL);

		if (res != 0) {
	    	fprintf(stderr, "\nErro ao esperar por uma tarefa trabalhadora.\n");    
	    	return -1;
		}  
    }

    /*Numero de itercoes*/
    count_iter = count_iter/trab;

    /*Imprimir a matriz correta*/
    if (count_iter % 2 != 0)
    dm2dPrint(matrix_aux);
	else
    dm2dPrint(matrix);

    destroyBarrier(bar);
    dm2dFree(matrix);
    dm2dFree(matrix_aux);
    free(tinfo);
    free(trabalhadoras);
    free(contadores);
    free(bar);

    return 0;
}
