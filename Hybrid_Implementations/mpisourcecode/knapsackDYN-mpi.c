#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include "mpi.h"

int main(int argc, char **argv){
	/* ..:: Rank = number o id of the routine ::.. */
	/* ..:: Size = number of processors ::... */
	/* ..:: Namelen = length of the name of the host::.. */
	int rank, size, namelen;
	/* ..:: Host = vector which putted the name of the processor */
	char host[50];
	/* ..:: Message = Message which traffic in the middle of the communication ::.. */
	int message[10];
	/* ..:: Test file ::.. */
	FILE *test_file = NULL;
	
	
	/* Initialize MPI program */
	/* MPI_COMM_WORLD is a variable to control all the processors. */
	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(host, &namelen);

	
	
	/* Validation when forgot the input parameter */
	if (argc!=2) { printf("\nError, mising input parameters.\n"); return 1; }
	/* Open the input file */
	if (!(test_file=fopen(argv[1],"r"))) { printf("Error opening Value file: %s\n",argv[1]); return 1; }
	
	
	/* ..:: Struct to receive the time ::.. */
	struct timeval tim;
	MPI_Status status;
	MPI_Request req;
	
	/* Counters */
	double tstarted  = 0;
	double tfinalize = 0;
	double treadFile = 0;
	
	/* tstarted = Time when started the program. */
	gettimeofday(&tim, NULL);
	tstarted = tim.tv_sec+(tim.tv_usec/1000000.0);

	
	
	/* Pointer the receive the file */
	long int numberItems, numberWeight;
	/* Vector of int to storage the inputs elements file */
	int *vectorNumberItems, *vectorNumberWeight;
	/* Variables */
	int cont=0;
	
	/* ------------- READ FILE ------------- */
	/* Reading number of items and Maximum width - First line the input file. */
	fscanf(test_file,"%ld %ld\n",&numberItems, &numberWeight);
	
	
	/* Master */
    if (rank==0){
		
		/* Storage the input */
		vectorNumberItems = (int *)malloc(numberItems*sizeof(int)); /*values for each element*/
		vectorNumberWeight = (int *)malloc(numberItems*sizeof(int)); /*width  for each element*/
		/*Reading value and width for each element*/
		for (cont=0;cont < numberItems; cont++){
			fscanf(test_file,"%d,%d\n", &vectorNumberItems[cont], &vectorNumberWeight[cont]);
		}
		/* ------------- END  FILE ------------- */
		
		
		gettimeofday(&tim, NULL);
		treadFile = (tim.tv_sec+(tim.tv_usec/1000000.0));
		
		/* Mostrando resultados */
		/*printf("Resultado: \n");
 * 		for(cont=0; cont < numberItems; cont++){
 * 					printf("%i %i\n", vectorNumberItems[cont], vectorNumberWeight[cont]);
 * 							}*/
		
		
		int **K;
		long int i, w;
		
		K = (int**) malloc((numberItems+1)*sizeof(int*));
		for (i=0;i<=numberItems;i++)
			K[i] = (int*)malloc((numberWeight+1)*sizeof(int));
		

		MPI_Request req;
		int message[10];
		int p,k;
		
		printf("NumberItems %i, NumberWeight %i.", numberItems, numberWeight);
		
		for (p = 0; p <= numberItems; p++)
				K[p][0] = 0;
		for (p = 0; p <= numberWeight; p++)
				K[0][p] = 0;

		
		int childrenClusters = size-1;
		int calculeQuantityChildren = ceil(numberWeight*1.0/childrenClusters*1.0);
		
		/*printf("Size: %i\n", size);
 * 		printf("\nClusters: %i, QuantitySize: %i\n", childrenClusters, calculeQuantityChildren);*/
		
		/*int u = 0;
 * 		for(p=1;p <= numberItems;p++){
 * 					for(k=1;k <= numberWeight;k++){
 * 									K[p][k]=u;
 * 													u++;
 * 																}
 * 																		}*/
		
			
		int cluster = 0;
		for(p=1;p <= numberItems;p++){
			for(k=1;k <size;k++){
				int begin, end;
				if(k<(size-1)){
					begin = (k-1) * calculeQuantityChildren;
					end = (k * calculeQuantityChildren)-1;
				}else{
					begin = (k-1) * calculeQuantityChildren;
					end = numberWeight;
				}
				int message[numberWeight+4];
				message[0] = begin;
				message[1] = end;
				message[2] = vectorNumberItems[p-1];
				message[3] = vectorNumberWeight[p-1];
				
				/*printf("Populando valor de p %i\n", p);*/
				
				
				
				/*int e,r;
 * 				printf("\n\n");
 * 								for(e=0;e<=numberItems;e++){
 * 													for(r=0;r<=numberWeight;r++){
 * 																			printf("%i ", K[e][r]);
 * 																								}
 * 																													printf("\n");
 * 																																	}
 * 																																					printf("\n\n");*/
				
				for(i=4;i<=numberWeight+4;i++){
					/*printf("%i ", K[p-1][i-4]);*/
					message[i] = K[p-1][i-4];
				}
					
				
				/* Show */
				/*printf("\nPai enviada\n");
 * 				for(i=0;i<=numberWeight+4;i++)
 * 									printf("%i ", message[i]);
 * 													printf("\n");*/
				
				
				/*printf("Send, enviou para %i\n", k);*/
				MPI_Send(message, numberWeight+5, MPI_INT, k, MPI_ANY_TAG, MPI_COMM_WORLD);
				
				
				/*int returned[(end-begin+1)+2];
 * 				
 * 								printf("Esperando, receber de %i, size: %i, begin %i, end %i, recv: %i\n", k, size, begin, end, (begin-end+1)+3);
 * 												MPI_Recv(returned, (end-begin+1)+2, MPI_INT, k, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
 * 																
 * 																				printf("O que recebi?\n");
 * 																								for(i=0;i<=(end-begin+2);i++)
 * 																													printf("%i ", returned[i]);
 * 																																	printf("\n");
 * 																																					
 * 																																									printf("Saiu aqui\n");
 * 																																													for(i=begin;i<=end;i++)
 * 																																																		K[p][i] = returned[i-begin+2];*/
			}
			
			
			for(k=1;k < size;k++){
				int begin, end;
				if(k<(size-1)){
					begin = (k-1) * calculeQuantityChildren;
					end = (k * calculeQuantityChildren)-1;
				}else{
					begin = (k-1) * calculeQuantityChildren;
					end = numberWeight;
				}
				int returned[(end-begin+1)+2];
				
				/*printf("Esperando, receber de %i, size: %i, begin %i, end %i, recv: %i\n", k, size, begin, end, (begin-end+1)+3);*/
				MPI_Recv(returned, (end-begin+1)+2, MPI_INT, k, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				
				/*printf("O que recebi?\n");
 * 				for(i=0;i<=(end-begin+2);i++)
 * 									printf("%i ", returned[i]);
 * 													printf("\n");
 * 																	
 * 																					printf("Saiu aqui\n");*/
				for(i=begin;i<=end;i++)
					K[p][i] = returned[i-begin+2];
			}
			
			/*printf("hey1\n");
 * 			int e,r;
 * 						for(e=0;e<=numberItems;e++){
 * 										for(r=0;r<=numberWeight;r++){
 * 															printf("%i ", K[e][r]);
 * 																			}
 * 																							printf("\n");
 * 																										}
 * 																													printf("hey2\n");*/
		}
		
		for(p=1;p<size;p++){
			message[0] = -1;	
			MPI_Send(message, 1, MPI_INT, p, 1, MPI_COMM_WORLD);
		}
		
		printf("\nResults: ");
		printf("\n%i.", K[numberItems][numberWeight]);
		
		gettimeofday(&tim, NULL);
		tfinalize = tim.tv_sec+(tim.tv_usec/1000000.0);
	
		printf("\nTimes: ");
		printf("\nTFinalize-TRead -- %.6lf\nTFinalize-TStarted -- %.6lf\n", tfinalize-treadFile, tfinalize-tstarted);
		
		
		/*printf("hey1\n");
 * 		for(p=0;p<=numberItems;p++){
 * 					for(k=0;k<=numberWeight;k++){
 * 									printf("%i ", K[p][k]);
 * 												}
 * 															printf("\n");
 * 																	}
 * 																			printf("hey2\n");*/
    } /* Nodes */
    else {
		MPI_Status status;
		int message[numberWeight+5];
		
		MPI_Recv(message, numberWeight+5, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		
		/*printf("\nMensagem recebida\n");
 * 		int i;
 * 				for(i=0;i<numberWeight+5;i++)
 * 							printf("%i ", message[i]);
 * 									printf("\n\n");*/
		
		while(message[0] != -1){
			int returned[message[1]-message[0]+3];
			
			returned[0] = message[0];
			returned[1] = message[1];
			int w;
			
			for (w = message[0]; w <= message[1]; w++){
				if (message[3] <= w){
						int max;	
						if(message[2] + message[w-message[3]+4] > message[w+4])
							max = message[2] + message[w-message[3]+4];
						else
							max = message[w+4];
						
						returned[w-message[0]+2] = max;
				}
				else
						returned[w-message[0]+2] = message[w+4];
			}
			
			/*printf("\nFilha mensagem enviando de volta");
 * 			for(w=0;w<=message[1]-message[0]+2;w++)
 * 							printf("%i ", returned[w]);
 * 										printf("\n");*/
			
			MPI_Send(returned, (message[1]-message[0]+1)+2, MPI_INT, 0, 1, MPI_COMM_WORLD);
			
			MPI_Recv(message, numberWeight+5, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			
			
			/*printf("\nMensagem recebida\n");
 * 			int i;
 * 						for(i=0;i<numberWeight+5;i++)
 * 										printf("%i ", message[i]);
 * 													printf("\n\n");*/
		}
    }
	
	
	
	/* Finalize MPI program */
    MPI_Finalize();
	
    return 0;
}
