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

void copiaMatriz (int mOrigem[TAM][TAM], int mDestino[TAM][TAM]){
    int i,j;
    #pragma omp parallel for private(i,j)
    for(i = 0; i < TAM; i ++)
        for(j = 0; j < TAM; j ++)
            mDestino[i][j] = mOrigem[i][j]; 
}

void calculaMatrizPesos(int mAdjacente[TAM][TAM], int mPesos[TAM][TAM]){
    int h, i, j, k;
    int count = TAM;
    int mAuxiliar[TAM][TAM];
    copiaMatriz(mAdjacente, mAuxiliar);
    int novoValor = 0;
    #pragma omp parallel private(h, i, j, k, novoValor)
    #pragma omp for schedule(static) 
    for(h = 0; h < count; h ++){
        for (i = 0; i < count; i ++){
           for(j = 0; j < count; j ++){
                novoValor = 0;
                for(k = 0; k < count; k ++)
                    novoValor += mAdjacente[i][k] * mAuxiliar[k][j];
                mAuxiliar[i][j] = novoValor;
                if(mPesos[i][j] == 0)
                    mPesos[i][j] = novoValor;
           }
        }
    }

}

main(){
    int matrizAdjacente[TAM][TAM] = {{0,0,1,0,0},{1,0,0,0,0},{0,1,0,0,1},{0,1,0,0,0},{0,0,0,1,0}}; 
    int matrizPesos[TAM][TAM] = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}}; ; //Matriz resultante dos pesos
   
    imprimirMatriz(matrizAdjacente);   
    calculaMatrizPesos(matrizAdjacente, matrizPesos);
    imprimirMatriz(matrizPesos);
}
