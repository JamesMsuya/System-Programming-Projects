#include "essentials.h"



int detachandremove(int shmid, void *shmaddr) {
	int error = 0;

	if (shmdt(shmaddr) == -1)
		error = errno;

	if ((shmctl(shmid, IPC_RMID, NULL) == -1) && !error)
		error = errno;

	if (!error)
		return 0;

	errno = error;
	return -1;
}

/*This function allocates memeory to a n*m matrix where n and m are not the same*/
double ** memAllocator(int rows,int columns){
	int i=0;
	double **matrix = (double**) malloc(sizeof(double*)*rows);
	if(matrix == NULL)
		return NULL;

	for(i = 0; i < rows; ++i){
		matrix[i] = (double*) malloc(sizeof(double)*columns);

		if(matrix[i] == NULL){
			freeMatrix(matrix,i);
			return NULL;
		}
		memset(matrix[i],0,sizeof(double)*columns);
	}
	return matrix;
}



void freeMatrix(double **matrix,int rows){
	int i=0;
	for(i=0; i < rows;i++){
		
		free((matrix)[i]);
	}
	free(matrix);

}


void printMatrix(double ** matrix,int rows,int columns){
	int i = 0;
	int j = 0;
	for(i = 0; i < rows; ++i){
		for(j = 0; j < columns; ++j)
			fprintf(stdout, "%e ",matrix[i][j]);
		
		fprintf(stdout, "%c\n",'\n');
	}
}

void printMatrix1D(double * matrix,int columns){
	int i = 0;
	for(i = 0; i < columns; i++){	
		fprintf(stdout, "%.3f ",matrix[i]);	
	}
	fprintf(stdout, "%c\n",'\n');
}




void copyMatrix( double ** matrixSource, double ** matrixDest, int rows,int columns){
	int lcv=0;
	int lcv2=0;
	if(matrixSource==NULL){
		return;
	}
	for(lcv = 0; lcv < rows; lcv++){
		for(lcv2 = 0; lcv2 < columns; lcv2++){
			matrixDest[lcv][lcv2] = matrixSource[lcv][lcv2];
		}
	}
}



void printMatrixToFile(double ** matrix,int row,int column,FILE *out){
	int i = 0;
	int j = 0;
	if(matrix ==NULL || out==NULL){
		return;
	}
	for(i = 0; i < row; ++i){
		for(j = 0; j < column; ++j)
			fprintf(out, "%.5f ",matrix[i][j]);
		
		fprintf(out, "%c\n\n",'\n');

	}
	fflush(out);
}


double **matrixGenerator(int rows,int columns){
	int i=0;
	int j = 0;
	double ** matrix = memAllocator(rows,columns);

	srand(time(NULL) * getpid());
		for(i = 0; i < rows; ++i){
			for(j = 0; j < columns; ++j)
				
				matrix[i][j] = rand()%9 + 1;
			
		}

	return matrix;
}

double *matrixGeneratorSingle(int rows){
	int i=0;
	double * matrix = malloc(sizeof(double)*rows);

	srand(time(NULL) * pthread_self());
		for(i = 0; i < rows; ++i){
				
			matrix[i] = rand()%9 + 1;
			
		}
	return matrix;
}


void copyFrom1Dto2D( double * matrixSource, double ** matrixDest, int rows,int columns){
	int i=0;
	int j=0;
	if(matrixSource==NULL){
		return;
	}
	for(i=0; i < rows; i++){
		for(j=0; j < columns; j++){
			matrixDest[i][j] = matrixSource[i*columns + j];
		}
	}
}

void copyFrom1Dto1D( double * matrixSource, double * matrixDest, int start,int end){
	int i=0;
	int j=0;
	if(matrixSource==NULL){
		return;
	}
	for(i=start,j=0; i < end; i++,j++){
		
			matrixDest[j] = matrixSource[i];
	}
}



void matrixTranspouse(double **matrix,int rows,int columns){
	int i=0;
	int j=0;
	double temp;

	for (i = 0; i < rows; ++i){
		for (j = 0; j < rows; j++){
			temp=matrix[i][j];
			matrix[i][j]=matrix[j][i];
			matrix[j][i]=temp;
			
		}
	}

}

 double* backSub(double **a, int columns, double *indx){
	int lcv=0;
	int lcv2;
	double sum = 0;
	double* roots = (double*) calloc(columns,sizeof(double));
	if(roots == NULL)
		return NULL;

	for(lcv = columns-1; lcv >= 0; --lcv ){
		sum = 0;
		for(lcv2 = lcv ; lcv2 < columns-1; ++lcv2){
			sum+= a[lcv][lcv2] * roots[lcv2];
		}

		roots[lcv] = (indx[columns-1] - sum) / a[lcv][lcv];
	} 

	return roots;
}


/*QR*/

mat matrix_new(int m, int n){
	mat x = malloc(sizeof(mat_t));
	x->v = malloc(sizeof(double) * m);
	x->v[0] = calloc(sizeof(double), m * n);
	for (int i = 0; i < m; i++)
		x->v[i] = x->v[0] + n * i;
	x->m = m;
	x->n = n;
	return x;
}
 
void matrix_delete(mat m){
	free(m->v[0]);
	free(m->v);
	free(m);
}
 
void matrix_transpose(mat m){
	for (int i = 0; i < m->m; i++) {
		for (int j = 0; j < i; j++) {
			double t = m->v[i][j];
			m->v[i][j] = m->v[j][i];
			m->v[j][i] = t;
		}
	}
}
 
mat matrix_copy(int n, double** a, int m){
	mat x = matrix_new(m, n);
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			x->v[i][j] = a[i][j];
	return x;
}
 
mat matrix_mul(mat x, mat y){
	if (x->n != y->m) return 0;
	mat r = matrix_new(x->m, y->n);
	for (int i = 0; i < x->m; i++)
		for (int j = 0; j < y->n; j++)
			for (int k = 0; k < x->n; k++)
				r->v[i][j] += x->v[i][k] * y->v[k][j];
	return r;
}
 
mat matrix_minor(mat x, int d){
	mat m = matrix_new(x->m, x->n);
	for (int i = 0; i < d; i++)
		m->v[i][i] = 1;
	for (int i = d; i < x->m; i++)
		for (int j = d; j < x->n; j++)
			m->v[i][j] = x->v[i][j];
	return m;
}
 
/* c = a + b * s */
double *vmadd(double a[], double b[], double s, double c[], int n){
	for (int i = 0; i < n; i++)
		c[i] = a[i] + s * b[i];
	return c;
}
 
/* m = I - v v^T */
mat vmul(double v[], int n){
	mat x = matrix_new(n, n);
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			x->v[i][j] = -2 *  v[i] * v[j];
	for (int i = 0; i < n; i++)
		x->v[i][i] += 1;
 
	return x;
}
 
/* ||x|| */
double vnorm(double x[], int n){
	double sum = 0;
	for (int i = 0; i < n; i++) sum += x[i] * x[i];
	return sqrt(sum);
}
 
/* y = x / d */
double* vdiv(double x[], double d, double y[], int n){
	for (int i = 0; i < n; i++) y[i] = x[i] / d;
	return y;
}
 
/* take c-th column of m, put in v */
double* mcol(mat m, double *v, int c){
	for (int i = 0; i < m->m; i++)
		v[i] = m->v[i][c];
	return v;
}
 
void matrix_show(mat m){
	for(int i = 0; i < m->m; i++) {
		for (int j = 0; j < m->n; j++) {
			printf(" %8.3f", m->v[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}
 
void householder(mat m, mat *R, mat *Q){
	mat q[m->m];
	mat z = m, z1;
	for (int k = 0; k < m->n && k < m->m - 1; k++) {
		double e[m->m], x[m->m], a;
		z1 = matrix_minor(z, k);
		if (z != m) matrix_delete(z);
		z = z1;
 
		mcol(z, x, k);
		a = vnorm(x, m->m);
		if (m->v[k][k] > 0) a = -a;
 
		for (int i = 0; i < m->m; i++)
			e[i] = (i == k) ? 1 : 0;
 
		vmadd(x, e, a, e, m->m);
		vdiv(e, vnorm(e, m->m), e, m->m);
		q[k] = vmul(e, m->m);
		z1 = matrix_mul(q[k], z);
		if (z != m) matrix_delete(z);
		z = z1;
	}
	matrix_delete(z);
	*Q = q[0];
	*R = matrix_mul(q[0], m);
	for (int i = 1; i < m->n && i < m->m - 1; i++) {
		z1 = matrix_mul(q[i], *Q);
		if (i > 1) matrix_delete(*Q);
		*Q = z1;
		matrix_delete(q[i]);
	}
	matrix_delete(q[0]);
	z = matrix_mul(*Q, m);
	matrix_delete(*R);
	*R = z;
	matrix_transpose(*Q);
}


double * qrDecomposition(double **matrix,double *matrixB,int rows,int columns){
	double ** matrix1;
	double ** matrix2;
	double *matSol;
	double *matSol1;
	mat R, Q;
	mat x;

	x= matrix_copy(columns, matrix , rows);
	householder(x, &R, &Q);

	matrix1=memAllocator(rows,rows);
	matrix2=memAllocator(rows,columns);
	copyMatrix(Q->v,matrix1,rows,rows);
 	copyMatrix(R->v,matrix2,rows,columns);
	// to show their product is the input matrix
	mat m = matrix_mul(Q, R);
 	matrixTranspouse(matrix1,rows,rows);
 	matSol = multiply1Dto2D(matrix1,matrixB,columns,columns);
 	matSol1 = backSub(matrix2,columns,matSol);
	matrix_delete(x);
	matrix_delete(R);
	matrix_delete(Q);
	matrix_delete(m);
	free(matSol);
	freeMatrix(matrix1,rows);
	freeMatrix(matrix2,rows);
	

 return matSol1;
}


double PYTHAG(double a, double b){
    double at = fabs(a), bt = fabs(b), ct, result;

    if (at > bt)       { ct = bt / at; result = at * sqrt(1.0 + ct * ct); }
    else if (bt > 0.0) { ct = at / bt; result = bt * sqrt(1.0 + ct * ct); }
    else result = 0.0;
    return(result);
}

double ** matrixMul2Dto2D(double**matrix,double **matrix1,int rows,int columns){
	double **temp=memAllocator(rows,columns);
	int i=0;
	int j=0;
	int k=0;
	for (i = 0; i < rows; i++)
		for (j = 0; j < columns; j++)
			for (k = 0; k < columns; k++)
				temp[i][j] += matrix[i][k] * matrix1[k][j];
	return temp;
}


double * multiply1Dto2D(double ** matrix, double * matrixMultiplier, int rows,int columns){
	int i=0;
	int j=0;
	double * matrixSol = malloc(sizeof(double)*columns);
	memset(matrixSol,0,columns*sizeof(double));
	if(matrixMultiplier==NULL || matrix ==NULL ){
		return NULL;
	}
	for(i=0; i < columns; i++){
		for(j=0; j < columns; j++){
			matrixSol[i] += (matrix[i][j] * matrixMultiplier[j]);
		}
	}

	return matrixSol;
}

double ax_bNorm(double **matrixA,double *matrix,double *matrixB,int columns){
	int i=0;
	double * matc =  multiply1Dto2D(matrixA,matrix,columns,columns);
	for (i = 0; i < columns; ++i){
		matrixB[i] = (matc[i] - matrixB[i]);
	}
	free(matc);
	return(norm(matrixB,columns));
}

double norm(double *matc,int columns){
	double temp=0;
	int i=0;
	for (i = 0; i < columns; ++i){
		temp += (matc[i]*matc[i]);
	}
	return(sqrt(temp));
}

int getnamed(char *name, sem_t **sem, int val){
	while (((*sem = sem_open(name, FLAGS, PERMS, val)) == SEM_FAILED) &&
		(errno == EINTR)) ;
	if (*sem != SEM_FAILED)
		return 0;
	if (errno != EEXIST)
		return -1;
	while (((*sem = sem_open(name, 0)) == SEM_FAILED) && (errno == EINTR)) ;
	if (*sem != SEM_FAILED)
		return 0;
	return -1;
}