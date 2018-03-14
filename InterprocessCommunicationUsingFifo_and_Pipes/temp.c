#include "essentials.h"



double **matrixGenerator( int dimension);

double ** convolution(double **matrix, int dimension);




int main(int argc, const char **argv){

	double **matrix;
	double **matrix1;
	double **matrix2;
	double det=0;
	double det2=0;
	double det3=0;
	int dim = atoi(argv[1]);
	dim*=2;
	matrix=matrixGenerator(dim);
	det=determinant(matrix,dim);
	matrix1=inverseFull(matrix,dim);
	matrix2=convolution(matrix,dim);

	printMatrix(matrix,dim);
	printf("%f\n",det);
	det2=determinant(matrix1,dim);
	printf("------------------------\n\n");
	printMatrix(matrix1,dim);
	printf("%e\n",det2);
	printf("------------------------\n\n");
	printf("\n");
	printMatrix(matrix2,dim);
	det3=determinant(matrix2,dim);
	printf("------------------------\n\n");
	printf("\n");
	printf("%e\n",det-det2);
	printf("%e\n",det-det3);
	freeMatrix(matrix2,dim);
	freeMatrix(matrix,dim);
	freeMatrix(matrix1,dim);
	return 0;
}






double **matrixGenerator( int dimension){
	int i=0;
	int j = 0;
	double ** matrix = memAllocator(dimension);

	srand(time(NULL));
	while(determinant(matrix,dimension)==0.0){
		for(i = 0; i < dimension; ++i){
			for(j = 0; j < dimension; ++j)
				
				matrix[i][j] = rand()%9 + 1;
			
		}
	}
	return matrix;
}


double ** convolution(double **matrix, int dimension){
    int i, j, m, n, mm, nn;
    double sum;
   	double kernel[3][3];
   	memset(kernel,0,sizeof(double)*9);
   	kernel[1][1]=1;
    double **temp=memAllocator(dimension);
    int kCenterX = 3 / 2;
    int kCenterY = 3 / 2;


    for(i=0; i < dimension; ++i){               /* rows*/
    
        for(j=0; j < dimension; ++j){           /* columns*/
        
            sum = 0;                           
            for(m=0; m < 3; ++m){      
            
                mm = 2 - m;       /* row index of flipped kernel */

                for(n=0; n < 3; ++n){ 
                
                    temp[i][j]=matrix[i][j];
                   
                }
            }
        }
    }

    return temp;
}