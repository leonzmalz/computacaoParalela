#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
 
#define TAM 5

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

void calculaMatrizPesos(int mAdjacente[TAM][TAM], int mPesos[TAM][TAM]){
    int i, j, k;
    int mAuxiliar[TAM][TAM];
    copiaMatriz(mAdjacente, mAuxiliar);
    int novoValor = 0;
    //Multiplicarei as matrizes TAM vezes
    for (k = 0; k < TAM; k ++){
       for(i = 0; i < TAM; i ++){
            for(j = 0; j < TAM; j ++){
                novoValor += mAdjacente[k][j] * mAuxiliar[j][i];
            }
            mPesos[k][i] = novoValor;
       }
    }
}

void copiaMatriz (int mOrigem[TAM][TAM], int mDestino[TAM][TAM]){
    int i,j;
    for(i = 0; i < TAM; i ++)
        for(j = 0; j < TAM; j ++)
            mDestino[i][j] = mOrigem[i][j]; 
}

main(){
    int matrizAdjacente[TAM][TAM] = {{0,0,1,0,0},{1,0,0,0,0},{0,1,0,0,1},{0,1,0,0,0},{0,0,0,1,0}}; 
    int matrizPesos[TAM][TAM] = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}}; ; //Matriz resultante dos pesos
   
    imprimirMatriz(matrizAdjacente);   
    calculaMatrizPesos(matrizAdjacente, matrizPesos);
    imprimirMatriz(matrizPesos);
}
