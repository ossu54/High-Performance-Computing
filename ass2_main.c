#include "stdio.h"
#include "stdlib.h"

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

void printMat(double ** A, int N){
int i,j;
for(i = 0; i < N; i++){
	for(j = 0; j < N; j++){
		printf("%7.2f ",A[i][j]);
	}	
	printf("\n");
}
}


int main(){

// init loop variables
int i, j;

// Size of square PHYSICAL domain \
find a way to input this from command line like in assignment 1
int NN = 5000;

// Size of square NUMERICAL domain
int N = NN + 2;
double delta = 2/N; // distance between points
double delta2 = delta*delta; // delta squared
double Nt = N/6.0; // number of points corresponding to a third in physical units

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

/*
The real code should be here. While loop that checks if change from uo to u is small enough to be accepted (solution has converged). Jacobi should be implemented as a sub-routine in a separate function
*/

}