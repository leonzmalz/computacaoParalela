#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#define TAM 4
#define MASTER 0               /* taskid of first task */
#define FROM_MASTER 1          /* setting a message type */
#define FROM_WORKER 2          /* setting a message type */

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

int main (int argc, char *argv[]){
int totalTask,              /* number of tasks in partition */
  taskid,                /* a task identifier */
  numworkers,            /* number of worker tasks */
  source,                /* task id of message source */
  dest,                  /* task id of message destination */
  mtype,                 /* message type */
  rows,                  /* rows of matrix A sent to each worker */
  averow, extra, offset, /* used to determine rows sent to each worker */
  h,i, j, k, rc;           /* misc */
int a[TAM][TAM], b[TAM][TAM],c[TAM][TAM],d[TAM][TAM];

MPI_Status status;

MPI_Init(&argc,&argv);
MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
MPI_Comm_size(MPI_COMM_WORLD,&totalTask);

//Inicializacao das matrizes
if(taskid  == MASTER){
    numworkers = totalTask-1; //Ignoro a master
    inicializaMatrizAdjacente(a);
    printf("Matriz Adjacente Gerada aleatoriamente...\n");
    imprimirMatriz(a);
    copiaMatriz(a, b);
    copiaMatriz(a, d);//Primeira matriz de distâncias é a matriz adjacente
}
//Multiplica a matriz TAM - 1 vezes
for(h = 2; h <= TAM; h ++){
 /**************************** master task ************************************/
   if (taskid == MASTER)
   {
      printf("Iteracao %d ...\n", h);  
      //Aqui irei dividir a matriz e distribuir pelas threads
      averow = TAM/numworkers;
      extra = TAM%numworkers;
      offset = 0;
      mtype = FROM_MASTER;
      for (dest=1; dest<=numworkers; dest++)
      {
         rows = (dest <= extra) ? averow+1 : averow;    
         printf("Sending %d rows to task %d offset=%d\n",rows,dest,offset);
         MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
         MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
         MPI_Send(&a[offset][0], rows*TAM, MPI_INT, dest, mtype,
                   MPI_COMM_WORLD);
         MPI_Send(&b, TAM*TAM, MPI_INT, dest, mtype, MPI_COMM_WORLD);
         offset = offset + rows;
      }

      /* Receive results from worker tasks */
      mtype = FROM_WORKER;
      for (i=1; i<=numworkers; i++)
      {
         source = i;
         MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
         MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
         MPI_Recv(&c[offset][0], rows*TAM, MPI_INT, source, mtype, 
                  MPI_COMM_WORLD, &status);
         printf("Received results from task %d\n",source);
      }
      atualizaMatrizDistancias(d, c, h);
      copiaMatriz(c, b);
      /* Print results */
      printf("******************************************************\n");
      printf("Matrix de distâncias atual:\n");
      imprimirMatriz(d);
      printf("\n******************************************************\n");
   }

/**************************** worker task ************************************/
   if (taskid > MASTER)
   {
      mtype = FROM_MASTER;
      MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
      MPI_Recv(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
      MPI_Recv(&a, rows*TAM, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
      MPI_Recv(&b, TAM*TAM, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);

      for (k=0; k<TAM; k++)
         for (i=0; i<rows; i++)
         {
            c[i][k] = 0.0;
            for (j=0; j<TAM; j++)
               c[i][k] = c[i][k] + a[i][j] * b[j][k];
         }
      mtype = FROM_WORKER;
      MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
      MPI_Send(&c, rows*TAM, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
   }
  
}
MPI_Finalize();

}