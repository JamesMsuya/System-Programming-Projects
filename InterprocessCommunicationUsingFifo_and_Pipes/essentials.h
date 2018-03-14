
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



void printMatrix(double ** matrix,int n);

double ** inverse(double **matrix1, int n);

double **inverseFull(double **matrix,int dimension);

void copy( double ** matrixSource, double ** matrixDest, int rowStart,int rowEnd,int colStart, int colEnd);

void printMatrixToFile(double ** matrix,int dimension,FILE *out);

double ** convolution(double **matrix, int dimension);

int applyPartialPivoting(double ** matrix,int rows,int columns,int pivoteIndex);

double max(double *matrix,int rows);

double ** solvedByGESP(double ** augmentedMatrix,int dimension);

void copyToSource( double ** matrixSource, double ** matrixDest, int rowStart,int rowEnd,int colStart, int colEnd);

void freeMatrix(double ** memory,int size);

double ** memAllocator(int dimension);

double ** memAllocator1(int rows,int columns);

double determinant(double ** matrix,int dimension);

void copyMatrix( double ** matrixSource, double ** matrixDest, int dimension);


#endif