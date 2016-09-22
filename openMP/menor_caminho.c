#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
 
#define TAM 10 

int matrizPesos[TAM][TAM]; 
 
void imprimirMatriz(int m[TAM][TAM]) {
    int i, j;
    printf("    ");
    for (i = 0; i < TAM; ++i)
        printf("%4c", 'A' + i);
    printf("\n");
    for (i = 0; i < TAM; ++i) {
        printf("%4c", 'A' + i);
        for (j = 0; j < TAM; ++j)
            printf("%4d", m[i][j]);
        printf("\n");
    }
    printf("\n");
}

int menorPeso(int pesoAtual, int pesoUm, int pesoDois){
	if(pesoAtual == 0 || pesoAtual > pesoUm + pesoDois)
		return pesoUm + pesoDois;
	return pesoAtual;
}

void floyd_warshall(int m[TAM][TAM]) {
    int i, j, k;
    #pragma omp parallel for private(k,i,j)
    for (k = 0; k < TAM; k ++)
    	for(i = 0; i < TAM; i ++)
    		for(j = 0; j < TAM; j ++)
    			if(i != j)
    				m[i][j] = menorPeso(m[i][j],m[i][k],m[k][j]);
}
 
main() {
    double start,stop;
    int i, j;
    for (i = 0; i < TAM; ++i)
        for (j = 0; j < TAM; ++j)
            if (i==j)
                matrizPesos[i][j]=0;
            else
                matrizPesos[i][j]= (int)( 11.0 * rand() / ( RAND_MAX + 1.0 ) );//number creator 0-10
 
    imprimirMatriz(matrizPesos);// printout the new array
 
    start = omp_get_wtime();
    floyd_warshall(matrizPesos);
    stop = omp_get_wtime();
 
    imprimirMatriz(matrizPesos);// printout again the new array with shortest paths
 
    printf("time %f\n",stop-start);
}