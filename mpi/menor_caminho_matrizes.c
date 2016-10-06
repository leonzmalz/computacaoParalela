#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#define TAM 4   
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
    int totalTask,           /* number of tasks in partition */
      taskid,                /* a task identifier */
      numworkers,            /* number of worker tasks */
      source,                /* task id of message source */
      dest,                  /* task id of message destination */
      mtype,                 /* message type */
      rows,                  /* rows of matrix A sent to each worker */
      averow, extra, offset, /* used to determine rows sent to each worker */
      h,i, j, k, rc;           /* misc */
    int mAdjacente[TAM][TAM];
    int mAuxiliar[TAM][TAM];
    int mResult[TAM][TAM];
    int mDistancias[TAM][TAM];

    MPI_Status status;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    MPI_Comm_size(MPI_COMM_WORLD,&totalTask);

    //Inicializacao das matrizes
    if(taskid  == 0){
        numworkers = totalTask-1; //Ignoro a master
        printf("Matriz Adjacente Gerada aleatoriamente...\n");
        inicializaMatrizAdjacente(mAdjacente);
        imprimirMatriz(mAdjacente);
        copiaMatriz(mAdjacente, mAuxiliar);
        copiaMatriz(mAdjacente, mDistancias);//Primeira matriz de distâncias é a matriz adjacente 
    }
    //Multiplica a matriz TAM - 1 vezes
    for(h = 2; h <= TAM; h ++){
       if (taskid == MASTER)
       {
          printf("Iteracao %d ...\n", h);  
          averow = TAM/numworkers;
          extra = TAM%numworkers;
          offset = 0;
          
          //Aqui irei dividir a matriz e distribuir pelas threads
          for (dest=1; dest<=numworkers; dest++)
          {
             rows = (dest <= extra) ? averow+1 : averow;    
             printf("Enviando %d linhas para a thread %d \n",rows,dest);
             MPI_Send(&offset, 1, MPI_INT, dest, FROM_MASTER, MPI_COMM_WORLD);
             MPI_Send(&rows, 1, MPI_INT, dest, FROM_MASTER, MPI_COMM_WORLD);
             MPI_Send(&mAdjacente[offset][0], rows*TAM, MPI_INT, dest, FROM_MASTER, MPI_COMM_WORLD);
             MPI_Send(&mAuxiliar, TAM*TAM, MPI_INT, dest, FROM_MASTER, MPI_COMM_WORLD);
             offset = offset + rows;
          }
          //Recebo a matriz resultante da multiplicação
          for (i=1; i<=numworkers; i++)
          {
             source = i;
             MPI_Recv(&offset, 1, MPI_INT, source, FROM_WORKER, MPI_COMM_WORLD, &status);
             MPI_Recv(&rows, 1, MPI_INT, source, FROM_WORKER, MPI_COMM_WORLD, &status);
             MPI_Recv(&mResult[offset][0], rows*TAM, MPI_INT, source, FROM_WORKER, MPI_COMM_WORLD, &status); 
             printf("Resultados recebidos da thread %d\n",source);
          }
          atualizaMatrizDistancias(mDistancias, mResult, h);
          copiaMatriz(mResult, mAuxiliar);
          printf("******************************************************\n");
          printf("Matriz de distâncias atual:\n");
          imprimirMatriz(mDistancias);
          printf("\n******************************************************\n");
       }

       if (taskid > MASTER)
       {
          mtype = FROM_MASTER;
          MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
          MPI_Recv(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
          MPI_Recv(&mAdjacente, rows*TAM, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
          MPI_Recv(&mAuxiliar, TAM*TAM, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
          
          multiplicarMatrizes(mAdjacente, mAuxiliar, mResult, rows);
          
          mtype = FROM_WORKER;
          MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
          MPI_Send(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
          MPI_Send(&mResult, rows*TAM, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
       }
      
    }
    MPI_Finalize();

}