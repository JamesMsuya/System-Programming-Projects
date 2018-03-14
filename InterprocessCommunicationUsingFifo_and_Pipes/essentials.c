#include "essentials.h"



/*This function allocates memory to a square matix*/
double ** memAllocator(int dimension){
	int i=0;
	double **matrix = (double**) malloc(sizeof(double*)*dimension);
	if(matrix == NULL)
		return NULL;

	for(i = 0; i < dimension; ++i){
		matrix[i] = (double*) malloc(sizeof(double)*dimension);

		if(matrix[i] == NULL){
			freeMatrix(matrix,i);
			return NULL;
		}
		memset(matrix[i],0,sizeof(double)*dimension-1);
	}
	return matrix;
}


/*This function allocates memeory to a n*m matrix where n and m are not the same*/
double ** memAllocator1(int rows,int columns){
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
		memset(matrix[i],0,sizeof(double)*columns-1);
	}
	return matrix;
}




double determinant(double ** matrix,int dimension){
	int i=0;
	long double results=1;
	double ** matrix1 = solvedByGESP(matrix,dimension);
	if (matrix1==NULL){
	
		return 0;
	}

	for (i = 0; i < dimension; i++){
		results *= matrix1[i][i];
	}
	freeMatrix(matrix1,dimension);
	return results;
}


int applyPartialPivoting(double ** matrix,int rows,int columns,int pivoteIndex){
	int lcv=0;
	int rowWithMaxRatio = pivoteIndex;
	double maxRatio = 0;
	double scaleFactor = 0 ;
	double tempMaxRatio = 0;
	double* swap = NULL;
	if(!(scaleFactor = max(&matrix[pivoteIndex][pivoteIndex],columns - pivoteIndex -1)))
		return 1;
	maxRatio = fabs(matrix[pivoteIndex][pivoteIndex])/ scaleFactor;	

	for(lcv = pivoteIndex+1; lcv < rows; lcv++){

		if(!(scaleFactor = max(&matrix[lcv][pivoteIndex],columns-pivoteIndex-1)))
			return 1;

		if((tempMaxRatio = fabs(matrix[lcv][pivoteIndex])/scaleFactor) > maxRatio)
		{
			maxRatio = tempMaxRatio;
			rowWithMaxRatio = lcv;
		}
	}

	if(rowWithMaxRatio != pivoteIndex){	
		swap = matrix[pivoteIndex];
		matrix[pivoteIndex] = matrix[rowWithMaxRatio];
		matrix[rowWithMaxRatio] = swap;
	}

	return 0 ;
}



double max(double *matrix,int rows){
	int lcv=0;
	double temp=fabs(matrix[0]);
		
		for (lcv = 0; lcv < rows; lcv++){
		
			if(temp < fabs(matrix[lcv])){
				temp=fabs(matrix[lcv]);
			}
		}
return temp;

}



void freeMatrix(double **matrix,int rows){
	int i=0;
	for(i=0; i < rows;i++){
		
		free((matrix)[i]);
	}
	free(matrix);

}


void printMatrix(double ** matrix,int dimension){
	int i = 0;
	int j = 0;
	for(i = 0; i < dimension; ++i){
		for(j = 0; j < dimension; ++j)
			fprintf(stdout, "%.3f ",matrix[i][j]);
		
		fprintf(stdout, "%c\n",'\n');
	}
}



double ** solvedByGESP(double ** augmentedMatrix,int dimension){
	int i=0;
	int j=0;
	int k=0;
	int error = 0;
	double multiplicand = 0;
	double pivot = 0;
	double ** matrixCopy  = memAllocator(dimension);

	copyMatrix(augmentedMatrix,matrixCopy,dimension);

	for(i = 0; i < dimension-1; ++i){
	
		if(applyPartialPivoting(matrixCopy,dimension,dimension,i)){
		
			freeMatrix(matrixCopy,dimension);
			return NULL;
		}
		
		pivot = matrixCopy[i][i];

		if(pivot!=0)
		{
			for(j = i+1; j < dimension; ++j)
			{
				multiplicand = (matrixCopy[j][i]/pivot);

				for(k = i; k < dimension; ++k)
				{
					matrixCopy[j][k] -= matrixCopy[i][k] * multiplicand;
				}				
			}
		}
	}

	for(i = 0; i < dimension; ++i)
	{
		error = 1;
		for(j = 0; j < dimension; ++j){
		
			if(matrixCopy[i][j] != 0)
				error = 0;
		}

		if(error){
		
			freeMatrix(matrixCopy,dimension);
			return NULL;
		}
	}

	return matrixCopy;
}



void copyMatrix( double ** matrixSource, double ** matrixDest, int dimension){
	int lcv=0;
	int lcv2=0;
	if(matrixSource==NULL){
		return;
	}
	for(lcv = 0; lcv < dimension; lcv++){
		for(lcv2 = 0; lcv2 < dimension; lcv2++){
			matrixDest[lcv][lcv2] = matrixSource[lcv][lcv2];
		}
	}
}

/*This function creates a sub matrix from main matrix. 
ie it creates n*n matrix from 2n*2n matrix*/
void copy( double ** matrixSource, double ** matrixDest, int rowStart,int rowEnd,int colStart, int colEnd){
	int i=0;
	int j=0;
	int lcv=0;
	int lcv2=0;
	if(matrixSource==NULL){
		return;
	}
	for(lcv = rowStart,i=0; lcv < rowEnd; lcv++,i++){
		for(lcv2 = colStart,j=0; lcv2 < colEnd; lcv2++,j++){
			matrixDest[i][j] = matrixSource[lcv][lcv2];
		}
	}
}

/*This function copies from n*n submatrix to 2n*2n matrix*/
void copyToSource( double ** matrixSource, double ** matrixDest, int rowStart,int rowEnd,int colStart, int colEnd){
	int i=0;
	int j=0;
	int lcv=0;
	int lcv2=0;
	if(matrixSource==NULL){
		return;
	}
	for(lcv = rowStart,i=0; lcv < rowEnd; lcv++,i++){
		for(lcv2 = colStart,j=0; lcv2 < colEnd; lcv2++,j++){
			matrixDest[lcv][lcv2] = matrixSource[i][j];
		}
	}
}



/*This function finds the inverse of n*n matrix all combine all of them into a single matrix of order 2n*2n */
double **inverseFull(double **matrix,int dimension){

	/*the first quarter of matrix is inverse*/
	double **temp;
	double **temp1=memAllocator(dimension/2);
	double **temp2=memAllocator(dimension);
	copy(matrix,temp1,0,dimension/2,0, dimension/2);
	temp=inverse(temp1,dimension/2);
	copyToSource(temp, temp2,0,dimension/2,0, dimension/2);
	freeMatrix(temp1,dimension/2);
	freeMatrix(temp,dimension/2);

	/*the second quarter of matrix is inverse from 0,n to n,2n*/
	temp1=memAllocator(dimension/2);
	copy(matrix,temp1,0,dimension/2,dimension/2, dimension);
	temp=inverse(temp1,dimension/2);
	copyToSource( temp, temp2,0,dimension/2,dimension/2, dimension);
	freeMatrix(temp1,dimension/2);
	freeMatrix(temp,dimension/2);

	/*the third quarter of matrix is inverse from n,2n to 0,n*/
	temp1=memAllocator(dimension/2);
	copy(matrix,temp1,dimension/2, dimension,0, dimension/2);
	temp=inverse(temp1,dimension/2);
	copyToSource( temp, temp2,dimension/2, dimension,0, dimension/2);
	freeMatrix(temp1,dimension/2);
	freeMatrix(temp,dimension/2);

	/*the fourth quarter of matrix is inverse from n,2n to n,2n*/
	temp1=memAllocator(dimension/2);
	copy(matrix,temp1,dimension/2, dimension,dimension/2, dimension);
	temp=inverse(temp1,dimension/2);
	copyToSource( temp, temp2,dimension/2, dimension,dimension/2, dimension);
	freeMatrix(temp1,dimension/2);
	freeMatrix(temp,dimension/2);


	return temp2;
}


/*,int rowEnd, int colStart, int colEnd*/
double ** inverse(double **matrix1, int n){
	int i=0;
	int j=0;
	int k=0;
	double a=1;
	double ratio=1;
	double **temp;
	double **matrix = memAllocator1(n,2*n);
	copy(matrix1,matrix,0,n,0,n);
    for(i = 0; i < n; i++){

        for(j = n; j < 2*n; j++){

            if(i==(j-n))

                matrix[i][j] = 1.0;

            else

                matrix[i][j] = 0.0;

        }

    }
  
    for(i = 0; i < n; i++){

        for(j = 0; j < n; j++){

            if(i!=j){

            	if(matrix[i][i] != 0){
            		ratio = matrix[j][i]/matrix[i][i];
            	}
              
                for(k = 0; k < 2*n; k++){

                    matrix[j][k] -= ratio * matrix[i][k];

                }

            }

        }

    }

    for(i = 0; i < n; i++){

    	if(matrix[i][i]!=0){
    		 a = matrix[i][i];
    	}
      
        for(j = 0; j < 2*n; j++){

            matrix[i][j] /= a;

        }

    }
temp=memAllocator(n);
copy(matrix,temp,0,n,n,2*n);
freeMatrix(matrix,n);

return temp;
}

void printMatrixToFile(double ** matrix,int dimension,FILE *out){
	int i = 0;
	int j = 0;
	if(matrix ==NULL || out==NULL){
		return;
	}
	for(i = 0; i < dimension; ++i){
		for(j = 0; j < dimension; ++j)
			fprintf(out, "%.5f ",matrix[i][j]);
		
		fprintf(out, "%c\n\n",'\n');

	}
	fflush(out);
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