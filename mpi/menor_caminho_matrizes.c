/* Menor caminho de Grafo pelo método da multiplicação de matrizes, utilizando MPI
   Desenvolvido por: Leonardo Pinheiro do Nascimento

   Entrada de dados: Matriz adjacente do grafo, gerado aleatóriamente pelo método inicializarMatrizAdjacente
   Saída de dados: Matriz contendo as menores distâncias entre todos os vértices do grafo 

*/

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
 
#define TAM 10

void imprimirMatriz(int** m) {
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

void zeraMatriz(int** m){
    int i, j;
    for (i = 0; i < TAM; i ++)
        for (j = 0; j < TAM; j ++)
            m[i][j] = 0;
}

//Alocação dinâmica de matrizes
int** getNovaMatriz(){
    int i;
    int** m = (int**)malloc(TAM * sizeof(int*));
    for (i = 0; i < TAM; i++) 
           m[i] = (int*) malloc(TAM * sizeof(int)); 
    return m; 
}

//Inicializa a matriz Adjacente com números randômicos entre 0 e 1
void inicializaMatrizAdjacente(int** mAdjacente){
    int i, j;
    srand( (unsigned)time(NULL));
    for (i = 0; i < TAM; i ++){
        for (j = 0; j < TAM; j ++){
            mAdjacente[i][j] = rand() % 2;
        }
    }
}

void copiaMatriz (int** mOrigem, int** mDestino){
    int i,j;
    for(i = 0; i < TAM; i ++)
        for(j = 0; j < TAM; j ++)
            mDestino[i][j] = mOrigem[i][j]; 
}
//Realiza a multiplicação de duas matrizes
void multiplicarMatrizes(int** m1, int** m2, int** mResult){
    int i, j, k;
    MPI_Bcast (m2, TAM*TAM, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter (m1, TAM*TAM, MPI_INT, m1[0], TAM*TAM, MPI_INT, 0, MPI_COMM_WORLD);
    for (i = 0; i < TAM; i ++)
       for(j = 0; j < TAM; j ++)
            for(k = 0; k < TAM; k ++)
                mResult[i][j] += m1[i][k] * m2[k][j];
    MPI_Gather (mResult[0], TAM*TAM, MPI_INT, mResult, TAM*TAM, MPI_INT, 0, MPI_COMM_WORLD);
       
}

//Atualiza a matriz de distâncias, de acordo com o grau fornecido
//O Grau será o número da iteração do for dentro de calculaDistancias
void atualizaMatrizDistancias(int** mDistancias, int** mNovasDistancias, int grau){
    int i, j;
    //#pragma omp parallel private(i, j)
    for (i = 0; i < TAM; i ++)
       for(j = 0; j < TAM; j ++)
            //Registra um novo caminho, caso haja
            if(mDistancias[i][j] == 0 && mNovasDistancias[i][j] != 0)
                mDistancias[i][j] = grau;
    
}

void calculaDistancias(int** mAdjacente, int** mDistancias){
    int h;
    int** mAuxiliar = getNovaMatriz(); //Matriz que vai 
    int** mResult = getNovaMatriz();
    zeraMatriz(mResult);
    copiaMatriz(mAdjacente, mAuxiliar);
    copiaMatriz(mAdjacente, mDistancias); //Primeira matriz de distâncias é a matriz adjacente
    //Multiplica a matriz TAM - 1 vezes
    for(h = 2; h <= TAM; h ++){
        multiplicarMatrizes(mAdjacente, mAuxiliar, mResult);
        //Faz a atualização das novas distâncias
        //A matriz contendo as novas distâncias eh a matriz resultante da multiplicação
        atualizaMatrizDistancias(mDistancias, mResult, h); 
        copiaMatriz(mResult, mAuxiliar);
        zeraMatriz(mResult);
    }

}

int main(int argc, char * argv[]){
    double start,stop;
    int** matrizAdjacente  = getNovaMatriz();
    int** matrizDistancias = getNovaMatriz();
    int p, myrank;
    MPI_Init (&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    printf("%d\n",p);
    printf("%d\n",myrank);
    inicializaMatrizAdjacente(matrizAdjacente);
    zeraMatriz(matrizDistancias);
    imprimirMatriz(matrizAdjacente);   
    start = clock();
    calculaDistancias(matrizAdjacente, matrizDistancias);
    imprimirMatriz(matrizDistancias);
    stop = clock();
    printf("tempo %f\n",stop-start);
}
