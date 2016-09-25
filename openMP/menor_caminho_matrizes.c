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
void zeraMatriz(int m[TAM][TAM]){
    int i, j;
    #pragma omp parallel for private(i,j)
    for (i = 0; i < TAM; i ++){
        for (j = 0; j < TAM; j ++){
            m[i][j] = 0;
        }       
    }
}
void copiaMatriz (int mOrigem[TAM][TAM], int mDestino[TAM][TAM]){
    int i,j;
    #pragma omp parallel for private(i,j)
    for(i = 0; i < TAM; i ++)
        for(j = 0; j < TAM; j ++)
            mDestino[i][j] = mOrigem[i][j]; 
}
void multiplicarMatrizes(int m1[TAM][TAM], int m2[TAM][TAM], int mResult[TAM][TAM]){
    int i, j, k;
    int count = TAM;
    #pragma omp parallel private(i, j, k)
    #pragma omp for schedule(static) 
    for (i = 0; i < count; i ++)
       for(j = 0; j < count; j ++)
            for(k = 0; k < count; k ++)
                mResult[i][j] += m1[i][k] * m2[k][j];
}

//Grau será o número da matriz de distâncias
void atualizaMatrizDistancias(int mDistancias[TAM][TAM], int mNovasDistancias[TAM][TAM], int grau){
    int i, j;
    #pragma omp parallel private(i, j)
    for (i = 0; i < TAM; i ++)
       for(j = 0; j < TAM; j ++)
            if(mDistancias[i][j] == 0 && mNovasDistancias[i][j] != 0)
                mDistancias[i][j] = grau;
    
}

void calculaDistancias(int mAdjacente[TAM][TAM], int mDistancias[TAM][TAM]){
    int h;
    int mAuxiliar[TAM][TAM];
    int mResult[TAM][TAM];
    zeraMatriz(mResult);
    copiaMatriz(mAdjacente, mAuxiliar);
    copiaMatriz(mAdjacente, mDistancias); //Primeira matriz de distâncias é a matriz adjacente
    for(h = 2; h <= TAM; h ++){
        multiplicarMatrizes(mAdjacente, mAuxiliar, mResult);
        //imprimirMatriz(mResult);
        atualizaMatrizDistancias(mDistancias, mResult, h);
        copiaMatriz(mResult, mAuxiliar);
        zeraMatriz(mResult);
    }

}

main(){
    int matrizAdjacente[TAM][TAM] = {{0,0,1,0,0},{1,0,0,0,0},{0,1,0,0,1},{0,1,0,0,0},{0,0,0,1,0}}; 
    int matrizDistancias[TAM][TAM];

    zeraMatriz(matrizDistancias);
    imprimirMatriz(matrizAdjacente);   
    calculaDistancias(matrizAdjacente, matrizDistancias);
    imprimirMatriz(matrizDistancias);
}
