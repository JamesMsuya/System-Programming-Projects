#include "essentials.h"
#define SERVER "server.txt"
#define RESULT "result"
#define RESULTTEMP "result.txt"
#define MILLI 1000
#define PATH "./logs/"


double** seeWhat(int *dimension,int pidServer);
static void sigHan(int sig, siginfo_t *info,void *context);
int j=0;
double **matrix;
double **matrixInv;
double **matrixConv;
int dimension;
int pidServer;
pid_t pidResult;
int pid;
int fd;
char fifoResult[10];/*This are temp arrays carrying names of temporarly fifos and logFiles*/
char fifoOwn[10];
char matLog[20];
int count=0;
FILE *logs;


int main(int argc, char const *argv[]){
	double temp;
	double temp1;
	char str[5];
	str[0]='\0';
	int fdShow;
	int fildes[2];
	FILE * server = fopen(SERVER,"r");
	FILE * result = fopen(RESULTTEMP,"r");
	struct timeval current,next;
	int childPid;
	struct sigaction act;
	union sigval value;
	fifoResult[0]='\0';
	fifoOwn[0]='\0';
	matLog[0]='\0';
	memset(&value,0,sizeof(value));
	memset(&act,0,sizeof(act));
	act.sa_sigaction = &sigHan;
	act.sa_flags = SA_SIGINFO;
	pid=getpid();
	value.sival_int = (int)pid;
	value.sival_ptr;


	
	if (sigaction(SIGINT,&act,0)==0){
		
		

	}
	if (sigaction(SIGUSR1,&act,0)==0){
		

	}
	if (sigaction(SIGUSR2,&act,0)==0){
		

	}
	
	if(server==NULL){
		perror("Server is not running");
		exit(1);
	}


	fscanf(server,"%d",&pidServer);
	fclose(server);



	if(result==NULL){
		while (((fdShow = open(RESULT, O_RDWR )) == -1));
		read(fdShow,&pidResult,sizeof(pidResult));
		write(fdShow,&pidResult,sizeof(pidResult));
		close(fdShow);
		
		}
	
	else{

		fscanf(result,"%d",&pidResult);
		fclose(result);
		
	}




	sprintf(fifoResult,"%d",pidResult);
	sprintf(fifoOwn,"%d",pid);
	mkfifo(fifoResult,0666);
	while (((fd = open(fifoResult, O_WRONLY )) == -1));
	

	while(1){
		/*A signal is sent from here to the main process*/
		sigqueue(pidServer,SIGUSR1,value);
		sleep(1);
		matrix=seeWhat(&dimension,pidServer);
		count++;
		sprintf(str,"%d_%d",pid,count);
		strcpy(matLog,PATH);

		strcat(matLog,str);
		logs=fopen(matLog,"w");
		fprintf(logs, "Original Matrix \n");
		printMatrixToFile(matrix,dimension,logs);
		if(childPid=fork()!=-1){
		
					
					
			if(childPid==0){

					gettimeofday(&current, NULL);
					matrixInv = inverseFull(matrix,dimension);
					fprintf(logs, "Inverted Matrix \n");
					printMatrixToFile(matrixInv,dimension,logs);
					gettimeofday(&next, NULL);
					temp1=(double) ( next.tv_usec - current.tv_usec) / MILLI + (double) (next.tv_sec - current.tv_sec )*MILLI;
					temp= determinant(matrix,dimension)-determinant(matrixInv,dimension);
					freeMatrix(matrixInv,dimension);
					freeMatrix(matrix,dimension);
					fclose(logs);
					fflush(logs);
					close(fd);
					exit(1);

			}
					while(wait(NULL) > 0);

					gettimeofday(&current, NULL);
					matrixInv = inverseFull(matrix,dimension);
					fprintf(logs, "Inverted Matrix \n");
					printMatrixToFile(matrixInv,dimension,logs);
					gettimeofday(&next, NULL);
					temp1=(double) ( next.tv_usec - current.tv_usec) / MILLI + (double) (next.tv_sec - current.tv_sec )*MILLI;
					temp= determinant(matrix,dimension)-determinant(matrixInv,dimension);
					write(fd,&temp,sizeof(temp));
					write(fd,&temp1,sizeof(temp1));
					freeMatrix(matrixInv,dimension);
					/*freeMatrix(matrixInv,dimension);
					freeMatrix(matrix,dimension);
					fclose(logs);
					close(fd);
					exit(1);*/
	


				
				
				

				
				gettimeofday(&current, NULL);
				matrixConv= convolution(matrix,dimension);
				temp = determinant(matrix,dimension)-determinant(matrixConv,dimension);
				fprintf(logs, "Convulated Matrix \n");
				printMatrixToFile(matrixConv,dimension,logs);
				gettimeofday(&next, NULL);
				temp1=(double) ( next.tv_usec - current.tv_usec) / MILLI + (double) (next.tv_sec - current.tv_sec )*MILLI;
				write(fd,&temp,sizeof(temp));
				write(fd,&temp1,sizeof(temp1));
				temp=(double)pid;
				write(fd,&temp,sizeof(temp));
				if(matrix!=NULL){
					freeMatrix(matrix,dimension);	
				}
				freeMatrix(matrixConv,dimension);
				fclose(logs);
			
		}

	}


	return 0;
}



/*
This function handles the information from server and processes them.
*/
double** seeWhat(int *dimension,int pidServer){
	
	int i=0;
	int j=0;
	double value1=0;
	int dim=0;
	int fdes;
	int tempControl;/*Check if data was read from fifo*/
	double **matrix;
	char fifoClient[10];
	fifoClient[0]='\0';
	sprintf(fifoClient,"%d",pid);

	
	while (((fdes = open(fifoClient, O_RDONLY )) == -1));
	tempControl=read(fdes,&dim,sizeof(dim));

	

	if (tempControl){
		matrix= memAllocator(dim);

		
	}
	
		for(i=0;i < dim;i++){
			for (j = 0; j < dim; j++){
				tempControl=read(fdes,&value1,sizeof(value1));
				matrix[i][j] = value1;
				
			}
		}
	*dimension = dim;
	close(fdes);
	unlink(fifoClient);

	return matrix;
}



static void sigHan(int sig, siginfo_t *info,void *context){
	  

      	if(sig==SIGINT){

			while(wait(NULL) > 0);

			if(matrix!=NULL){
				freeMatrix(matrix,dimension);
			}
			killpg(pidServer,SIGUSR2);
			killpg(pidResult,SIGUSR2);
			unlink(fifoOwn);
			fclose(logs);
			unlink(RESULTTEMP);
	      	exit(1);

   	 	}


   	 	if(sig==SIGUSR1){
   	 		while(wait(NULL) > 0);
			if(matrix!=NULL){
				freeMatrix(matrix,dimension);
			}
			killpg(pidResult,SIGUSR2);
			unlink(fifoOwn);
			fclose(logs);
			unlink(RESULTTEMP);
	      	exit(1);

   	 	}

   	 	if(sig==SIGUSR2){
   	 		while(wait(NULL) > 0);
   	 		if(matrix!=NULL){
				freeMatrix(matrix,dimension);
			}
			killpg(pidServer,SIGUSR2);
			unlink(fifoOwn);
			fclose(logs);
			unlink(RESULTTEMP);
	      	exit(1);

   	 	}

  }