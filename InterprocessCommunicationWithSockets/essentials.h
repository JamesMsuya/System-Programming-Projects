
#ifndef ESSENTIALS_H
#define ESSENTIALS_H 


#include <math.h> 
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>  
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <semaphore.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netdb.h>

#define PERMS (mode_t)(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define FLAGS (O_CREAT | O_EXCL)
  

typedef struct {
	int m, n;
	double ** v;
} mat_t, *mat;
 
int getnamed(char *name, sem_t **sem, int val);

mat matrix_new(int m, int n);

void matrix_show(mat m);

void matrix_delete(mat m);
 
void matrix_transpose(mat m);
 
mat matrix_copy(int n, double** a, int m);
 
mat matrix_mul(mat x, mat y);
 
mat matrix_minor(mat x, int d);
 
/* c = a + b * s */
double *vmadd(double a[], double b[], double s, double c[], int n);
/* m = I - v v^T */
mat vmul(double v[], int n);
/* ||x|| */
double vnorm(double x[], int n);

double* vdiv(double x[], double d, double y[], int n);

void householder(mat m, mat *R, mat *Q);

double* mcol(mat m, double *v, int c);
/*QR functions*/
/*SVD function*/
double PYTHAG(double a, double b);

double ** matrixMul2Dto2D(double**matrix,double **matrix1,int rows,int columns);

int dsvd(float **a, int m, int n, float *w, float **v);
/*SVD ENDS*/
int detachandremove(int shmid, void *shmaddr);

double **matrixGenerator(int rows,int columns);

double * qrDecomposition(double **matrix,double *matrixB,int rows,int columns);

double *matrixGeneratorSingle(int rows);

double ax_bNorm(double **matrixA,double *matrix,double *matrixB,int columns);

void printMatrix(double ** matrix,int rows,int columns);

void printMatrix1D(double * matrix,int columns);

double norm(double *matc,int columns);

double ** inverse(double **matrix1, int n);

double * multiply1Dto2D(double ** matrix, double * matrixMultiplier, int rows,int columns);

void matrixTranspouse(double **matrix,int rows,int columns);

double* backSub(double **a, int columns, double *indx);

void printMatrixToFile(double ** matrix,int row,int column,FILE *out);

void copyFrom1Dto2D( double * matrixSource, double ** matrixDest, int rows,int columns);

void copyFrom1Dto1D( double * matrixSource, double * matrixDest, int start,int end);

void freeMatrix(double ** memory,int size);

double ** memAllocator(int rows,int columns);

void copyMatrix( double ** matrixSource, double ** matrixDest, int rows,int columns);


#endif

