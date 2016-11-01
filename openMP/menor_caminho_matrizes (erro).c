/* Menor caminho de Grafo pelo método da multiplicação de matrizes, utilizando openMP
   Desenvolvido por: Leonardo Pinheiro do Nascimento

   Entrada de dados: Matriz adjacente do grafo, gerado aleatóriamente pelo método inicializarMatrizAdjacente
   Saída de dados: Matriz contendo as menores distâncias entre todos os vértices do grafo 

   Algoritmo possui funcionamento semelhante ao algoritmo de Floyd-Warshall, 
   onde a partir da entrada da matriz adjacente é possível calcular uma matriz possuindo 
   todas as distâncias do grafo.

*/

#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <ctype.h>
 
#define TAM 4

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
    //Bloco executado em paralelo
    #pragma omp parallel for private(k,i,j)
    for (i = 0; i < TAM; i ++)
       for(j = 0; j < TAM; j ++)
            for(k = 0; k < TAM; k ++)
                mResult[i][j] += m1[i][k] * m2[k][j];
}

//Atualiza a matriz de distâncias, de acordo com o grau fornecido
//O Grau será o número da iteração do for dentro de calculaDistancias
void atualizaMatrizDistancias(int** mDistancias, int** mNovasDistancias, int** mCaminhos, int grau){
    int i, j;
    for (i = 0; i < TAM; i ++){
       for(j = 0; j < TAM; j ++){
            //Registra um novo caminho, caso haja
            if(mNovasDistancias[i][j] != 0 && mDistancias[i][j] == 0){
                mDistancias[i][j] = grau;
                mCaminhos[i][j] = mCaminhos[i][grau-1];  
            }
       } 
    }
 
}

int ** inicializaMatrizCaminhos(int** mAdjacente){
    int i,j;
    int** mCaminhos = getNovaMatriz();
    for (i = 0; i < TAM; i++){
        for (j = 0; j < TAM; j++){
            if(mAdjacente[i][j] == 1)
                mCaminhos[i][j] = j;
            else
                mCaminhos[i][j] = -1;
        }  
    }
    return mCaminhos;
}


int** calculaDistancias(int** mAdjacente, int** mDistancias){
    int h;
    int** mAuxiliar = getNovaMatriz(); 
    int** mResult = getNovaMatriz();
    int ** mCaminhos = inicializaMatrizCaminhos(mAdjacente);
    zeraMatriz(mResult);
    copiaMatriz(mAdjacente, mAuxiliar);
    copiaMatriz(mAdjacente, mDistancias); //Primeira matriz de distâncias é a matriz adjacente
    //Multiplica a matriz TAM - 1 vezes
    for(h = 2; h <= TAM; h ++){
        multiplicarMatrizes(mAdjacente, mAuxiliar, mResult);
        //Faz a atualização das novas distâncias
        //A matriz contendo as novas distâncias eh a matriz resultante da multiplicação
        atualizaMatrizDistancias(mDistancias, mResult, mCaminhos, h); 
        copiaMatriz(mResult, mAuxiliar);
        zeraMatriz(mResult);
    }
    return mCaminhos;

}

//Algoritmo guloso para encontrar menor caminho
void printCaminho(int ** matrizDistancias, int vInicial, int vFinal){
    int i;
    if (vInicial != vFinal){
        for(i = 0; i < TAM ; i ++){
            if(i != vFinal && matrizDistancias[vFinal][i] == 1){
                printf(" %c ", i + 65);
                printCaminho(matrizDistancias, vInicial, i);
                break;
            }
        }
        
    }else
        printf(" %c ", vFinal + 65);
}

main(){
    double start,stop;
    int** matrizAdjacente  = getNovaMatriz();
    int** matrizDistancias = getNovaMatriz();
   
    int indiceVerticeInicial, indiceVerticeFinal; //Indices utilizados na impressao do caminho
    char verticeInicial, verticeFinal; //Letras referentes aos caminhos
    
    inicializaMatrizAdjacente(matrizAdjacente);
    zeraMatriz(matrizDistancias);
    imprimirMatriz(matrizAdjacente);   
    start = omp_get_wtime();
    int** matrizCaminhos = calculaDistancias(matrizAdjacente, matrizDistancias);
    imprimirMatriz(matrizDistancias);
    imprimirMatriz(matrizCaminhos);
    stop = omp_get_wtime();
    printf("tempo %f\n",stop-start);
   
    while(1){
        
        printf("\nDigite o vértice inicial\n");
        verticeInicial = getc(stdin);
        __fpurge(stdin);
        printf("Digite o vértice final\n");
        verticeFinal = getc(stdin);
        __fpurge(stdin);

        //Transformo para maiúsculo
        verticeInicial = toupper(verticeInicial);
        verticeFinal = toupper(verticeFinal);

        //Para descobrir o valor do índice, preciso subtrair 65 do valor na tabela ASCII referente à letra
        indiceVerticeInicial = verticeInicial- 65;
        indiceVerticeFinal   = verticeFinal - 65;

        printf("Caminho de %c para %c: ",verticeInicial, verticeFinal);
        //printf(" %c ", verticeInicial);
        //printCaminho(matrizDistancias, indiceVerticeInicial, indiceVerticeFinal);
        //printf(" %c ", verticeFinal);


    }

}
