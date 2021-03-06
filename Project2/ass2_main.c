#include "stdio.h"
#include "stdlib.h"
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "ass2_lib.h"
#include <omp.h>

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


double ** dmalloc_2d_opt(int m, int n) {
	if (m <= 0 || n <= 0) return NULL;
	double **A = (double **)malloc(m * sizeof(double *));
	if (A == NULL) return NULL;
	//A[0] = (double *)malloc(m*n*sizeof(double));	
	#pragma omp parallel default(none) shared (A, m, n)
	{
		int i;
		#pragma omp for	private(i) 
		for (i = 0; i < m; i++)
			A[i] = (double *)malloc(n*sizeof(double));
	}
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

double getMatSum(double ** A, int N){
	int i,j;
	double sum = 0;
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			sum += A[i][j];
		}	
	}
	return sum;
}


double fnorm_squared(double ** u, double ** uo, int N){
	int i,j;
	double sum = 0;
	#pragma omp for	private(i,j)  reduction(+:sum)
	for(i = 1; i <N-1; i++){
		for(j = 1; j<N-1; j++){
			sum += (u[i][j]-uo[i][j])*(u[i][j]-uo[i][j]);
		}
	}
	return sum / (N*N);
}

void initialize_matrices(double ** u, double ** uo, double ** f, int N, double Nt){

	
	// init loop variables
	int i, j;

	// define uo as zeros\
	uo[x][0] = 0 i.e. outer wall
	#pragma omp for private (i, j)
	for(i = 1; i < N; i++){
		for(j = 1; j < N-1; j++){
			uo[i][j] = 0;
		}
	}
	
	// Defining the boundaries to 20
	#pragma omp master
	{
		for(j = 0; j < N; j++) uo[0][j] = 20;
		for(i = 0; i < N; i++) uo[i][0] = 20;
		for(i = 0; i < N; i++) uo[i][N-1] = 20;
	}
	#pragma omp barrier
		
	//setting u = uo;
	#pragma omp for private (i, j)
	for(i = 0; i<N; i++){
		for(j = 0; j<N; j++){
			u[i][j] = uo[i][j];
		}
	}
	//printMat(uo,N);

	// defining the f matrix
	#pragma omp for private (i, j)
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
}

int main(int argc, char **argv){
	// ./ass2_main <method type> <NN> <d> <kmax>
	
	int NN;
	double dd;
	int kmax;
	
	
	sscanf(argv[2] , "%d", &NN);
	sscanf(argv[3] , "%lf", &dd);
	sscanf(argv[4] , "%d", &kmax);

	double d = dd*dd;
	// Size of square NUMERICAL domain
	int N = NN + 2;
	double delta = 2.0/N; // distance between points
	double delta2 = delta*delta; // delta squared
	double delta2inv = 1/delta2; // 1/delta2
	double Nt = N/6.0; // number of points corresponding to a third in physical units	

	double checksum = 1000;
	int k = 0;

	double ** u, ** uo, ** f;
	int i1 = 0;
	#pragma omp parallel
	{
		int nthread = omp_get_num_threads();
		#pragma omp master
		{
			if(nthread > 10) i1 = 1;;
		}
	}
	if (strcmp(argv[1], "omp3") == 0 && i1 == 1){
		u = dmalloc_2d_opt(N,N);
		uo = dmalloc_2d_opt(N,N);
		f = dmalloc_2d_opt(N,N);
	} 
	else {
		u = dmalloc_2d(N,N);
		uo = dmalloc_2d(N,N);
		f = dmalloc_2d(N,N);
	}

	//initialize_matrices(u, uo, f, N, Nt);
	int i, j;
	struct timeval  tv1, tv2;
	double runtime;
	int nruns;

	if(strcmp(argv[1], "jacobi") == 0){

		runtime = 0.0;
		nruns = 0;
		
		while(runtime <= 3.0){
			k = 0;
			checksum = 1000;
			initialize_matrices(u, uo, f, N, Nt);			
			
			gettimeofday(&tv1, NULL);
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
			gettimeofday(&tv2, NULL);
			runtime += (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec);
			nruns++;
		}
		printf("%s, ", "JAC");
		printf("%f, ", (double) runtime / nruns);
		printf("%i, %.20f, %i, %i\n", N, dd, k, k*nruns);
		
	}

	if(strcmp(argv[1], "gauss") == 0){

		runtime = 0.0;
		nruns = 0;

		while(runtime <= 3.0){
			k = 0;
			checksum = 1000;
			initialize_matrices(u, uo, f, N, Nt);			
			
			gettimeofday(&tv1, NULL);
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
			gettimeofday(&tv2, NULL);
			runtime += (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec);
			nruns++;
		}
		
		printf("%s, ", "G-S");
		printf("%f, ", runtime);
		printf("%i, %.20f, %i, %i\n", N, dd, k, k*nruns);
		
	}
	
	if(strcmp(argv[1], "omp") == 0){
		runtime = 0.0;
	
		k = 0;
		checksum = 1000;
		initialize_matrices(u, uo, f, N, Nt);			
		
		double omp_s = omp_get_wtime();
		while(checksum > d && k < kmax){
			#pragma omp parallel default(none) shared(u,uo,f,N,delta2) private(i,j)
			{
				jacobi_seq(u,uo,f,N,delta2);
			} // end parallel
			checksum = fnorm_squared(u,uo,N);
			for(i = 0; i<N; i++){
				for(j = 0; j<N; j++){
					uo[i][j] = u[i][j];
				}
			} 
			
			k++;
		}
		double omp_time = omp_get_wtime() - omp_s;
		int thread = omp_get_max_threads();
		printf("%s, ", "OMP");
		printf("%f, ", omp_time);
		printf("%i, %.20f, %i, %i\n", N, dd, k, thread);

	}


	if(strcmp(argv[1], "omp2") == 0){
			runtime = 0.0;
	
		k = 0;
		checksum = 1000;
		initialize_matrices(u, uo, f, N, Nt);			
		
		double omp_s = omp_get_wtime();
			#pragma omp parallel default(none) shared	(u,uo,f,N,delta2, checksum, k, d, kmax) private(i,j)
				{
			while(checksum > d && k < kmax){
					jacobi_seq(u,uo,f,N,delta2);
					#pragma omp for	private(i,j)  reduction(+:checksum)
					for(i = 1; i <N-1; i++){
						for(j = 1; j<N-1; j++){
							checksum += (u[i][j]-uo[i][j])*(u[i][j]-uo[i][j]);
						}
					}
					#pragma omp for	private(i,j) 
					for(i = 0; i<N; i++){
						for(j = 0; j<N; j++){
							uo[i][j] = u[i][j];
						}
					} 
					#pragma omp master
					{
					k++;
					checksum=checksum/(N*N);		
					}
					#pragma omp barrier
				} // end while 
				
			} // end parallel
		double omp_time = omp_get_wtime() - omp_s;
		int thread = omp_get_max_threads();
		printf("%s, ", "OMP2");
		printf("%f, ", omp_time);
		printf("%i, %.20f, %i, %i\n", N, dd, k, thread);

	}

	if(strcmp(argv[1], "omp3") == 0){
		runtime = 0.0;
		k = 0;
		checksum = 1000;
		#pragma omp parallel default(none) shared(u, uo, f, N, Nt)
		{
			initialize_matrices(u, uo, f, N, Nt);			
		}
		double omp_s = omp_get_wtime();
			#pragma omp parallel default(none) shared	(u,uo,f,N,delta2, checksum, k, d, kmax) private(i,j)
			{
				while(k < kmax){
					jacobi_seq(u,uo,f,N,delta2);
					#pragma omp for	private(i,j) 
					for(i = 0; i<N; i++){
						for(j = 0; j<N; j++){
							uo[i][j] = u[i][j];
						}
					} 
					#pragma omp master
					{
					k++;
					checksum=checksum/(N*N);		
					}
					#pragma omp barrier
				} // end while 
			} // end parallel

	int thread = omp_get_max_threads();

	//printf("MATRIX U:\n");
	//printMat(u,N);
	//printf("\n");

	printf("%s, ", "OMP3");
	printf("%f, ", omp_get_wtime() - omp_s);
	printf("%i, %.20f, %.0f, %i, %i\n", N, dd, getMatSum(u, N), thread, k);

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
