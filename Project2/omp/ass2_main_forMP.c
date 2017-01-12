#include "stdio.h"
#include "stdlib.h"
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "ass2_lib.h"

double ** dmalloc_2d(int m, int n) {
	if (m <= 0 || n <= 0) return NULL;
	double **A = (double **)malloc(m * sizeof(double *));
	if (A == NULL) return NULL;
	A[0] = (double *)malloc(m*n*sizeof(double));
	if (A[0] == NULL) {
		free(A); 
		return NULL; 
	}
	int i;
	for (i = 1; i < m; i++)
		A[i] = A[0] + i * n;
	return A;
}

void dfree_2d(double **A) {
	free(A[0]);
	free(A);
}

void printMat(double ** A, int N){
	int i,j;
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			printf("%7.0f ",A[i][j]);
		}	
		printf("\n");
	}
}

double fnorm_squared(double ** u, double ** uo, int N){
	int i,j;
	double sum = 0;
	for(i = 1; i <N-1; i++){
		for(j = 1; j<N-1; j++){
			sum += (u[i][j]-uo[i][j])*(u[i][j]-uo[i][j]);
		}
	}
	return sum / (N*N);
}

int main(int argc, char **argv){
	// ./ass2_main <method type> <NN> <d> <kmax>
	
	int NN;
	double dd;
	int kmax;
	
	sscanf(argv[2] , "%d", &NN);
	sscanf(argv[3] , "%lf", &dd);
	sscanf(argv[4] , "%d", &kmax);

	// init loop variables
	int i, j;

	// Size of square PHYSICAL domain \
	find a way to input this from command line like in assignment 1
	

	// Size of square NUMERICAL domain
	int N = NN + 2;
	double delta = 2.0/N; // distance between points
	double delta2 = delta*delta; // delta squared
	double delta2inv = 1/delta2; // 1/delta2
	double Nt = N/6.0; // number of points corresponding to a third in physical units

	double d = dd*dd;
	double checksum = 1000;
	int k = 0;
	

	// init matrices, u is the newest version, uo i u old, and f is f
	double ** u, ** uo, ** f;

	u = dmalloc_2d(N,N);
	uo = dmalloc_2d(N,N);
	f = dmalloc_2d(N,N);

	// define uo as zeros\
	uo[x][0] = 0 i.e. outer wall
	for(i = 1; i < N; i++){
		for(j = 1; j < N-1; j++){
			uo[i][j] = 0;
		}
	}
	// Defining the boundaries to 20
	for(j = 0; j < N; j++) uo[0][j] = 20;
	for(i = 0; i < N; i++) uo[i][0] = 20;
	for(i = 0; i < N; i++) uo[i][N-1] = 20;

	//setting u = uo;
	for(i = 0; i<N; i++){
		for(j = 0; j<N; j++){
			u[i][j] = uo[i][j];
		}
	}
	//printMat(uo,N);

	// defining the f matrix
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			f[i][j] = 0;
		}
	}
	for(i = 4*Nt; i < 5*Nt; i++){
		for(j = 3*Nt; j < 4*Nt; j++){
			f[i][j] = 200;
		}
	}


	//printMat(f,N);

	if(strcmp(argv[1], "jacobi") == 0){
		//printf("DOING JACOBI\n");
		clock_t startc = clock();
		while(checksum > d && k < kmax){
			jacobi_seq(u,uo,f,N,delta2);
			checksum = fnorm_squared(u,uo,N);
			for(i = 0; i<N; i++){
				for(j = 0; j<N; j++){
					uo[i][j] = u[i][j];
				}
			}
			k++;
		}
		clock_t endc = clock();
		printf("%s, %f, %i, %.20f, %i, \n", "JAC", ((float)(endc - startc) / CLOCKS_PER_SEC), N, dd, k);
	}

	if(strcmp(argv[1], "gauss") == 0){
		//printf("DOING GAUSS-SEIDEL\n");
		clock_t startc = clock();
		while(checksum > d && k < kmax){
			gauss_seidel(u,f,N,delta2);
		
			checksum = fnorm_squared(u,uo,N);

			for(i = 0; i<N; i++){
				for(j = 0; j<N; j++){
					uo[i][j] = u[i][j];
				}
			}
			k++;
		}
		clock_t endc = clock();
		printf("%s, %f, %i, %.20f, %i, \n", "G-S", (float)(endc - startc) / CLOCKS_PER_SEC, N, dd, k);
		
	}

	if(strcmp(argv[1], "omp") == 0){
	#pragma omp parallel default(none) shared(u,uo,f,N,delta2,d,kmax,checksum) 
	{	
	  // int nthr = omp_get_num_threads;
	  //printf("Number of threads is %i \n",nthr);
	  printf("omp");
		while(checksum > d && k < kmax){
			jacobi_seq(u,uo,f,N,delta2);
			checksum = fnorm_squared(u,uo,N);
#pragma omp for private(i,j)
			for(i = 0; i<N; i++){
				for(j = 0; j<N; j++){
					uo[i][j] = u[i][j];
				}
			} // end of omp for
			k++;
		}
	} // end of omp parallel
	}

//	printMat(u,N);
	// Save the data

	/*
	The real code should be here. While loop that checks if change from uo to u is small enough to be accepted (solution has converged). Jacobi should be implemented as a sub-routine in a separate function
	*/

	//printf("k is: %i \n",k);

	dfree_2d(u);
	dfree_2d(uo);
	dfree_2d(f);
}