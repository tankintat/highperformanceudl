#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mpi.h"


int main(int argc, char **argv){
    int rank, size, namelen;
    char host[50];
	int message[10], returned[1];
	int rc, count;
	
    FILE *test_file = NULL;
	int *val, *wt;   // width and cost values
    long int Nitems; // Number of items
    long int Width;  // Max. load to carry
    long int cont;    // counter
    double tpivot1=0,tpivot2=0,tpivot3=0; //time counting
    struct timeval tim;
	
	if (argc!=2) {
	  printf("\n\nError, mising parameters:\n");
	  printf("format: test_file \n");
	  return 1;
	}
	
	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(host,&namelen);
    
	MPI_Status status;
	MPI_Request req;
	fflush(stdout);
	
	/* Read the file */
	gettimeofday(&tim, NULL);
	tpivot1 = tim.tv_sec+(tim.tv_usec/1000000.0);
	
	if (!(test_file=fopen(argv[1],"r"))) {
	  printf("Error opening Value file: %s\n",argv[1]);
	  return 1;
	}

	/*Reading number of items and Maximum width*/
	fscanf(test_file,"%ld %ld\n",&Nitems, &Width);
	/*printf("%ld\n",Nitems*sizeof(int));*/

	
	val = (int *)malloc(Nitems*sizeof(int)); /*values for each element*/
	wt = (int *)malloc(Nitems*sizeof(int)); /*width  for each element*/
	
	/*Reading value and width for each element*/
	for (cont=0;cont<Nitems;cont++){
	  fscanf(test_file,"%d,%d\n",&val[cont],&wt[cont]);
	}
	
	gettimeofday(&tim, NULL);
	tpivot2 = (tim.tv_sec+(tim.tv_usec/1000000.0));
	
	/*for (cont=0;cont<Nitems;cont++){
	  printf("Indice %i, Valor: %i, Peso %i\n", cont, val[cont], wt[cont]);
	}*/
	
	/* Inicio implementar */
	int **K;
	long int i, w;
	
	K = (int**) malloc((Width+1)*sizeof(int*));
	for (i=0;i<=Width;i++)
		K[i] = (int*)malloc((Nitems+1)*sizeof(int));
	
	
	for (w = 0; w <= Width; w++)
			K[0][w] = 0;
		
	for (i = 0; i <= Nitems; i++)
			K[i][0] = 0;
	
	int cluster = 0;
	
	int quantitycluster = 4;
	
    if (rank==0){
		printf("Valor de Width e NItems: %i %i\n", Width, Nitems);
		for (i = 1; i <= Nitems; i++){
			for (w = 1; w <= Width; w++){

				message[0] = wt[i-1];
				message[1] = w;
				message[2] = val[i-1];
				if(w>=wt[i-1])
					message[3] = K[i-1][w-wt[i-1]];
				else
					message[3] = 0;
				message[4] = K[i-1][w];
				message[5] = i;
				
				cluster = (cluster % quantitycluster) + 1;
				printf("%i Cluster: %i, Mensagem enviada: %i, %i, %i, %i, %i\n", i, cluster, message[0], message[1], message[2], message[3], message[4]);
				rc = MPI_Isend(message, 10, MPI_INT, cluster, 1, MPI_COMM_WORLD, &req);
				MPI_WAIT(&req, &status);
				rc = MPI_Irecv(returned, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &req);
				MPI_WAIT(&req, &status);
				rc = MPI_Get_count(&status, MPI_DOUBLE, &count);
				
				K[i][w] = returned[0];
				fflush(stdout);
				/*MPI_WAIT(&req, &status);*/
			}
			MPI_WAIT(&req, &status);
		}
		
		message[0] = -1;
		for(i=1;i<=quantitycluster;i++)
			rc = MPI_Send(message, 10, MPI_INT, i, 1, MPI_COMM_WORLD);
		
		
		for (i = 0; i <= Nitems; i++){
			for (w = 0; w <= Width; w++){
				printf("%i ", K[i][w]);
			}
			printf("\n\n\n");
		}
		printf("Resultado final: %i\n", K[Nitems][Width]);
		
		/* Fin implementar */
		gettimeofday(&tim, NULL);
		tpivot3 = (tim.tv_sec+(tim.tv_usec/1000000.0));
		printf(":%.6lf:%.6lf\n", tpivot3-tpivot2,tpivot3-tpivot1);
		
		free(val);
		free(wt);
		fclose(test_file);
    }
    else {
		MPI_Status status;
	
		rc = MPI_Irecv(message, 10, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &req);
		while(message[0] != -1){
			if (message[0] <= message[1]){
				if((message[2] + message[3]) && (message[2] + message[3])  >= message[4])
					returned[0] = (message[2] + message[3]);
				else
					returned[0] = message[4];
			}
			else
				returned[0] = message[4];
			
			/*K[message[5]][message[1]] = returned[0];*/
			
			fflush(stdout);
			rc = MPI_Isend(returned, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &req);
			MPI_WAIT(&req, &status);
			
			rc = MPI_Get_count(&status, MPI_DOUBLE, &count);
			rc = MPI_Irecv(message, 10, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &req);
			MPI_WAIT(&req, &status);
		}
    }
  	
    MPI_Finalize();
    return 0;
}