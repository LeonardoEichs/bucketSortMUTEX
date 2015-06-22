#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// Mutex para LOCK e UNLOCK
pthread_mutex_t global_mutex;

// Definição dos dados de entrada
#define tamanho  30
#define n_threads 5
#define n_buckets 5
//

// Criação da estrutura BUCKET
typedef struct {
  int tam;  // Quantos elementos estão dentro do BUCKET
  int min_value; // Menor valor armazenado no BUCKET
  int max_value; // Maior valor armazenado no BUCKET
  int *values; // Valores contidos no BUCKET
}bucket;
//

// Definição de variáveis globais
bucket vetorBuckets[n_buckets]; // Vetor que contém os BUCKETS
int proximoBucket = 0; // Variável usada no método *thread
//

// Método BUBBLE_SORT
void bubble_sort(int *v, int tam){
  int i,j,temp,trocou;
  for(j=0; j<tam-1; j++){
	  trocou = 0;
	  for(i = 0; i < tam -1; i++){
	    if(v[i+1] < v[i]){
		    temp = v[i];
		    v[i] = v[i + 1];
		    v[i + 1] = temp;
		    trocou = 1;
	    }
	  }
	   if(!trocou) break;
  }
}
//

// Método encarregado de ordenar os BUCKETS
void *thread(void *arg) {

  int val = *((int *) arg);

  while (true) {

    pthread_mutex_lock(&global_mutex); // LOCK

    if (proximoBucket == n_buckets) {
      pthread_mutex_unlock(&global_mutex); // UNLOCK
      break;
    }

    int bucketAtual = proximoBucket;
    proximoBucket++;

    pthread_mutex_unlock(&global_mutex); // UNLOCK

    printf("Thread %d ordena bucket %d\n", val, bucketAtual);
    bubble_sort(vetorBuckets[bucketAtual].values, vetorBuckets[bucketAtual].tam); // Chama BUBBLE_SORT
  }

  pthread_exit(NULL);

}
//

// Método MAIN
int main(int argc, char **argv) {

  int i; // Variável usada como contador no for
  int k; // Variável usada como contador no for

  // Checar
  if (n_threads < 1 || n_buckets > tamanho)
    printf("ERRO\n");

  else{

  // Cria vetor original
  int vetor[tamanho];
  srand((unsigned)time(NULL));
  for (i = 0; i < tamanho; i++) {
    vetor[i] = rand() % (tamanho - 1);
    printf("%d ", vetor[i]);
  }
  printf("\n");
  //

  // Criar buckets
	int resto = tamanho%n_buckets;
	int qtdBucket = tamanho/n_buckets;

	if (resto != 0) {
		vetorBuckets[0].min_value = qtdBucket * 0;
		vetorBuckets[0].max_value = qtdBucket;
		for (i = 1; i < resto; i++) {
				vetorBuckets[i].min_value = (qtdBucket * i)+(i * 1);
				vetorBuckets[i].max_value = vetorBuckets[i].min_value + qtdBucket;
		}
		for (i = resto; i < n_buckets; i++) {
			vetorBuckets[i].min_value = (qtdBucket * i)+(i * 1);
			vetorBuckets[i].max_value = (vetorBuckets[i].min_value + qtdBucket) - 1;
		}

	} else {
		for (i = 0; i < n_buckets; i++) {
			vetorBuckets[i].min_value = qtdBucket * i;
			vetorBuckets[i].max_value = (vetorBuckets[i].min_value + qtdBucket) - 1;
		}
	}
  //

  // Conta quantos elementos tem dentro de cada bucket
  int contador = 0;
  for(i = 0; i < n_buckets; i++) {
    for(k = 0; k < tamanho; k++) {
      if (vetor[k] >= vetorBuckets[i].min_value && vetor[k] <= vetorBuckets[i].max_value) {
        contador++;
      }
    }
    vetorBuckets[i].values = new int[contador]; // Inicializa values com quantidade de elementos que vão nesse BUCKET
    vetorBuckets[i].tam = contador;
    contador = 0;
  }
  //

  // Coloca os elementos dentro dos BUCKETS
  contador = 0;
  for(i = 0; i < n_buckets; i++) {
    for(k = 0; k < tamanho; k++) {
      if (vetor[k] >= vetorBuckets[i].min_value && vetor[k] <= vetorBuckets[i].max_value) {
        vetorBuckets[i].values[contador] = vetor[k];
        contador++;
      }
    }
    contador = 0;
  }
  //

  //Cria threads
  pthread_t threads[n_threads];

  int arg[n_threads];
  int g = 0;
  for (g = 0; g < n_threads; g++) {
    arg[g] = g;
    pthread_create(&threads[g], NULL, thread, (void*)&arg[g]);
  }
  //

  // Join de THREADS
  for(i = 0; i < n_threads; i++)
    pthread_join(threads[i], NULL);
  //

  // Junta os valores ordenados dentro dos BUCKETS e imprime
  int cont = 0;
  for (i = 0; i < n_buckets; i++) {
    for (g = 0; g < vetorBuckets[i].tam; g++) {
      vetor[cont] = vetorBuckets[i].values[g];
      printf(" %d, ", vetor[cont]);
      cont++;
    }
}
printf("\n");
}
pthread_mutex_destroy(&global_mutex);
return 0;

}
//
