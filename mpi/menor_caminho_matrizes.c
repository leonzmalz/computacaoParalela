/* Menor caminho de Grafo pelo método da multiplicação de matrizes, utilizando MPI
   Desenvolvido por: Leonardo Pinheiro do Nascimento

   Entrada de dados: Matriz adjacente do grafo, gerado aleatóriamente pelo método inicializarMatrizAdjacente
   Saída de dados: Matriz contendo as menores distâncias entre todos os vértices do grafo 

*/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define TAM 500   
#define MASTER 0           
#define FROM_MASTER 1          
#define FROM_WORKER 2         

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

//Inicializa a matriz Adjacente com números randômicos entre 0 e 1
void inicializaMatrizAdjacente(int  mAdjacente[TAM][TAM]){
    int i, j;
    srand( (unsigned)time(NULL));
    for (i = 0; i < TAM; i ++){
        for (j = 0; j < TAM; j ++){
            if(i == j)
              mAdjacente[i][j] = 0;
            else
              mAdjacente[i][j] = rand() % 2;
        }
    }
}

void copiaMatriz (int mOrigem[TAM][TAM], int mDestino[TAM][TAM]){
    int i,j;
    for(i = 0; i < TAM; i ++)
        for(j = 0; j < TAM; j ++)
            mDestino[i][j] = mOrigem[i][j]; 
}

void atualizaMatrizDistancias(int mDistancias[TAM][TAM], int mNovasDistancias[TAM][TAM], int grau){
    int i, j;
    for (i = 0; i < TAM; i ++)
       for(j = 0; j < TAM; j ++)
            //Registra um novo caminho, caso haja
            if(mDistancias[i][j] == 0 && mNovasDistancias[i][j] != 0)
                mDistancias[i][j] = grau;
    
}

//Realiza a multiplicação de duas matrizes
void multiplicarMatrizes(int m1[TAM][TAM], int m2[TAM][TAM], int mResult[TAM][TAM], int rows){
    int i, j, k;
    for (k=0; k<TAM; k++)
        for (i=0; i<rows; i++){
            mResult[i][k] = 0;
            for (j=0; j<TAM; j++)
               mResult[i][k] += m1[i][j] * m2[j][k]; 
        }
}

int main (int argc, char *argv[]){
    int totalTask, taskid, origem, dest, totalThreads, rows, lastRow, grau;         
    int mAdjacente[TAM][TAM], mAuxiliar[TAM][TAM], mResult[TAM][TAM], mDistancias[TAM][TAM];
    MPI_Status status; //Estrutura possuindo detalhes sobre a transmissão
    clock_t start, stop;

    start = clock();
    //Inicializa o ambiente de execução 
    MPI_Init(&argc,&argv);
     //Determina a variavel que irá guardar o rank do processo
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    //Determina o numero de processos com valor padrão
    MPI_Comm_size(MPI_COMM_WORLD,&totalTask);


    //Inicializacao das matrizes
    //Caso seja o processo principal
    if(taskid  == MASTER){
        totalThreads = totalTask - 1; //Ignoro a master
        printf("Matriz Adjacente Gerada aleatoriamente...\n");
        inicializaMatrizAdjacente(mAdjacente);
        imprimirMatriz(mAdjacente);
        copiaMatriz(mAdjacente, mAuxiliar);
        copiaMatriz(mAdjacente, mDistancias);//Primeira matriz de distâncias é a matriz adjacente 
    }
    //Multiplica a matriz TAM - 1 vezes
    for(grau = 2; grau <= TAM; grau++){
       if (taskid == MASTER){
          printf("-----------------------------------------------------\n");
          printf("Iteracao %d ...\n", grau);  
          lastRow = 0;
          
          //Aqui irei dividir a matriz e distribuir pelas threads
          for (dest=1; dest<=totalThreads; dest++){
             if(dest <= TAM%totalThreads)
                rows = (TAM/totalThreads) + 1;
             else
                rows = (TAM/totalThreads);
             //Envia as mensagens contendo a matriz adjacente, auxiliar,
             //ultima linha da submatriz e total de linhas
             MPI_Send(&lastRow, 1, MPI_INT, dest, FROM_MASTER, MPI_COMM_WORLD);
             MPI_Send(&rows, 1, MPI_INT, dest, FROM_MASTER, MPI_COMM_WORLD);
             MPI_Send(&mAdjacente[lastRow][0], rows*TAM, MPI_INT, dest, FROM_MASTER, MPI_COMM_WORLD);
             MPI_Send(&mAuxiliar, TAM*TAM, MPI_INT, dest, FROM_MASTER, MPI_COMM_WORLD);
             lastRow += rows;
          }
          
          //Recebo a matriz resultante das multiplicações
          for (origem=1; origem<= totalThreads; origem++){
             MPI_Recv(&lastRow, 1, MPI_INT, origem, FROM_WORKER, MPI_COMM_WORLD, &status);
             MPI_Recv(&rows, 1, MPI_INT, origem, FROM_WORKER, MPI_COMM_WORLD, &status);
             MPI_Recv(&mResult[lastRow][0], rows*TAM, MPI_INT, origem, FROM_WORKER, MPI_COMM_WORLD, &status); 
          }
          
          atualizaMatrizDistancias(mDistancias, mResult, grau);
          copiaMatriz(mResult, mAuxiliar);
          printf("Matriz de distâncias atual:\n");
          imprimirMatriz(mDistancias);

          if (grau == TAM){
            stop = clock();
            printf("tempo %lf ms\n",(double)(stop-start)/ CLOCKS_PER_SEC);
          }
       }
       //Caso não seja o processo principal
       if (taskid > MASTER){
          //Recebe a submatriz enviada pela MASTER
          MPI_Recv(&lastRow, 1, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
          MPI_Recv(&rows, 1, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
          MPI_Recv(&mAdjacente, rows*TAM, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
          MPI_Recv(&mAuxiliar, TAM*TAM, MPI_INT, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
          
          multiplicarMatrizes(mAdjacente, mAuxiliar, mResult, rows);

          //Envia a submatriz multiplicada para o MASTER
          MPI_Send(&lastRow, 1, MPI_INT, MASTER, FROM_WORKER, MPI_COMM_WORLD);
          MPI_Send(&rows, 1, MPI_INT, MASTER, FROM_WORKER, MPI_COMM_WORLD);
          MPI_Send(&mResult, rows*TAM, MPI_INT, MASTER, FROM_WORKER, MPI_COMM_WORLD);
       }
      
    }
    MPI_Finalize();

    
}