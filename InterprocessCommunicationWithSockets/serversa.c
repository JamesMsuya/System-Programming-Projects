#include "essentials.h"
#define PORTNUM 50000
#define MAXHOSTNAME 255
#define PROCESS 3
#define MILLI 1000
#define PATH "./logsServer/"
#define SEM "/semserver" /*a named semaphore*/
#define PERMS (mode_t)(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define FLAGS (O_CREAT | O_EXCL)


struct arguments{
	int rows;
	int columns;
	pthread_mutex_t *mylock;
	pthread_cond_t *condVar;
	int *flag;
	int *flag2;
	double *sharedMem;
	double *sharedMemSolution;

};

struct send{
	int rows;
	int columns;
	int clients;
	pthread_t tid;
	pid_t pid;
	double timeT;
};


int serve(int rows,int columns,struct send data);

void generate(pthread_mutex_t *mylock,pthread_cond_t *condVar,int rows,int columns,double *sharedMem,int *flag);

void solve(pthread_mutex_t *mylock,pthread_cond_t *condVar,double *sharedMem,double *sharedMemSolution,int rows,int columns,int *flag,int *flag2);

void verify(pthread_mutex_t *mylock,pthread_cond_t *condVar,double *sharedMem,double *sharedMemSolution,struct send data,int *flag,int *flag2);

static void sigHan(int sig, siginfo_t *info,void *context);

void *wrapperSolverSVD(void *argm);

void *wrapperSolverQR(void *argm);

void *wrapperSolverPseudoInverse(void *argm);

void *wrapperServe(void *argm);

int establish (unsigned short portnum);

int getConnection(int s);

pthread_cond_t condVar1 = PTHREAD_COND_INITIALIZER;

int count;

pid_t pidArrays[1000];

int socketo;


int main(int argc, char const *argv[]){
	int s=0;
	int notify=0;
	int pid;
	int num=0;
	int t=0;
	struct send data;
	pthread_t tid;
	struct sigaction act;

	s=establish(PORTNUM);
	
	
	memset(&act,0,sizeof(act));
	act.sa_sigaction = &sigHan;
	act.sa_flags = SA_SIGINFO;
	

	if (argc!=3){
		printf("Usage : server <port #, id> <thpool size, k >\n" );
		return 0;
	}

	if (sigaction(SIGINT,&act,0)==0){
		
	}

	printf("Serving 0 Clients...\n");
	count=0;
	while(1){
		
		t= getConnection(s);
		socketo=t;
		for (;;){
			
			num=recv(t,&data,sizeof(struct send),0);
		
			if (num==0)
				break;

			notify++;
	
				printf("Serving %d'th Client...\n",notify);
			
			pthread_create(&tid,NULL,wrapperServe,&data);
			pthread_detach(tid);
		}
		pidArrays[count]=data.pid;
		count++;
		close(t);
	}
	close(s);
	return 0;
}


int serve(int rows,int columns,struct send data){
	int i=0;
	int shmId;
	int shmIdFlag;
	int shmIdFlag2;
	int shmMut;
	int shmCond;
	int pid1;
	int pid2;
	int pid3;
	int shmIdSolution;
	pthread_mutex_t *mylock;
	pthread_cond_t *condVar;
	pthread_mutexattr_t mutexAttr;/**/
	pthread_mutexattr_init(&mutexAttr);
	pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);
	pthread_condattr_t condAttr;
	pthread_condattr_init(&condAttr);
	pthread_condattr_setpshared(&condAttr, PTHREAD_PROCESS_SHARED);
/**/int *flag;
	int *flag2;
	double *sharedMem;
	double *sharedMemSolution;
	
	key_t key = ftok(".",(int)pthread_self());
	
	if((shmMut = shmget(IPC_PRIVATE,sizeof(pthread_mutex_t),IPC_CREAT | 0644)) < 0){
    	perror("shmget failed");
	    return -1;
	}
	if((shmCond = shmget(IPC_PRIVATE,sizeof(pthread_cond_t),IPC_CREAT | 0644)) < 0){
    	perror("shmget failed");
	    return -1;
	}

	if((shmIdFlag = shmget(IPC_PRIVATE,sizeof(int),IPC_CREAT | 0644)) < 0){
    	perror("shmget failed");
	    return -1;
	}
	if((shmIdFlag2 = shmget(IPC_PRIVATE,sizeof(int),IPC_CREAT | 0644)) < 0){
    	perror("shmget failed");
	    return -1;
	}
	if((shmId = shmget(IPC_PRIVATE,(rows+1)*columns*sizeof(double),IPC_CREAT | 0644)) < 0){
    	perror("shmget failed");
	    return -1;
	}
	if((shmIdSolution = shmget(IPC_PRIVATE,3*columns*sizeof(double),IPC_CREAT | 0644)) < 0){
    	perror("shmget failed");
	    return -1;
	}

	mylock = shmat(shmMut,NULL,0);
	condVar = shmat(shmCond,NULL,0);
	flag=shmat(shmIdFlag,NULL,0);
	*flag=0;
	flag2=shmat(shmIdFlag2,NULL,0);
	*flag2=0;
	sharedMem = shmat(shmId, NULL, 0);
	memset(sharedMem,'\0',(rows+1)*columns*sizeof(double));
	sharedMemSolution = shmat(shmIdSolution, NULL, 0);
	memset(sharedMemSolution,'\0',3*columns*sizeof(double));
	pthread_mutex_init(mylock, &mutexAttr);
	pthread_cond_init(condVar,&condAttr);

	if((pid1 = fork()) == -1){
		perror("failed to fork"); 
		return -1;
	}
	

		if(pid1==0){
			generate(mylock,condVar,rows,columns,sharedMem,flag);
			exit(1);

		}

	
	if((pid2 = fork()) == -1){
		perror("failed to fork"); 
		return -1;
	}
	

		if(pid2==0){
			printf(" ");
			solve(mylock,condVar,sharedMem,sharedMemSolution,rows,columns,flag,flag2);
			fprintf(stderr, "%s\n","2" );
			exit(1);
		}

	
	if((pid3 = fork()) == -1){
		perror("failed to fork"); 
		return -1;
	}
	

		if(pid3==0){
			verify(mylock,condVar,sharedMem,sharedMemSolution,data,flag,flag2);
			fprintf(stderr, "%s\n","3" );
			exit(1);
		}




	/*main process waits for children to terminate*/
	
	pthread_mutex_destroy(mylock);
	pthread_cond_destroy(condVar);
	pthread_mutexattr_destroy(&mutexAttr);
	pthread_condattr_destroy(&condAttr);
	detachandremove(shmId,sharedMem);
	detachandremove(shmIdFlag,flag);
	detachandremove(shmMut,mylock);
	detachandremove(shmCond,condVar);
	detachandremove(shmIdFlag2,flag2);
	detachandremove(shmIdSolution,sharedMemSolution);
	while(wait(NULL) > 0);

}

void generate(pthread_mutex_t *mylock,pthread_cond_t *condVar,int rows,int columns,double *sharedMem,int *flag){
	int i;

	double **matrix=matrixGenerator(rows,columns);
	double *matrix1d = matrixGeneratorSingle(columns);
	
   		pthread_mutex_lock(mylock);

		for (i = 0; i < rows; ++i){
			memcpy( &(sharedMem[i*columns]) ,matrix[i],columns*sizeof(double));
			
		}
		memcpy( &(sharedMem[i*columns]) ,matrix1d,columns*sizeof(double));
		(*flag)=1;
		pthread_cond_signal(condVar);
		pthread_mutex_unlock(mylock);
		
	free(matrix1d);
	freeMatrix(matrix,rows);
		
}



void solve(pthread_mutex_t *mylock,pthread_cond_t *condVar,double *sharedMem,double *sharedMemSolution,int rows,int columns,int *flag,int *flag2){
	int i=0;
	struct arguments argm;
	pthread_t tid[PROCESS];
	memset(tid,0,sizeof(pthread_t)*PROCESS);
	argm.rows=rows;
	argm.columns=columns;
	*argm.mylock=*mylock;
	*argm.condVar=*condVar;
	argm.flag=flag;
	argm.flag2=flag2;
	argm.sharedMem=sharedMem;
	argm.sharedMemSolution=sharedMemSolution;

	pthread_create(&tid[0], NULL, wrapperSolverSVD,&argm);

	pthread_create(&tid[1], NULL, wrapperSolverQR,&argm);

	pthread_create(&tid[2], NULL, wrapperSolverPseudoInverse,&argm);

	for(i=0;i < PROCESS; i++){
			pthread_join(tid[i],NULL);

	}
	
}



void verify(pthread_mutex_t *mylock,pthread_cond_t *condVar,double *sharedMem,double *sharedMemSolution,struct send data,int *flag,int *flag2){
	char file[255];
	char temp[255];
	double *m = malloc(sizeof(double)*data.columns);
	double **matrix=memAllocator(data.rows,data.columns);
	double *matrixSol = malloc(sizeof(double)*data.columns);
	double *sendToClients= malloc(1+sizeof(double)*(data.rows + PROCESS)*data.columns);
	int i=0;
	int j=0;
	FILE *out;
	file[0]='\0';
	temp[0]='\0';
	strcpy(file,PATH);
	sprintf(temp,"%lu.txt",data.tid);
	strcat(file,temp);
	out=fopen(file,"w");
	if (out==NULL){
		perror("Out is null\n" );
		return;
	}
	
	pthread_mutex_lock(mylock);
	while((*flag2)!=3)
	{
		pthread_cond_wait(condVar, mylock);
		pthread_cond_signal(condVar);
	}
	

	copyFrom1Dto2D(sharedMem,matrix,data.rows,data.columns);
	copyFrom1Dto1D(sharedMem,matrixSol,data.rows*data.columns,(data.rows+1)*data.columns);
	
	fprintf(out, "\n\n");
	fprintf(out, "Client PID %d \n",data.pid);
	fprintf(out, "Client Thread ID %lu \n",data.tid);
	fprintf(out, "\nGenerated A matrix \n");
	for(i = 0; i < data.rows; ++i){
		for(j = 0; j < data.columns; ++j){
			fprintf(out, "%.5f ",sharedMem[i*data.columns + j]);
			sendToClients[i*data.columns + j]= sharedMem[i*data.columns + j];
		}
	
		fprintf(out, "%c\n",'\n');
	}

		fprintf(out, "\nGenerated B matrix \n");
		for(j = 0; j < data.columns; ++j){
			fprintf(out, "%.5f ",sharedMem[data.rows*data.columns+ j]);
			sendToClients[data.rows*data.columns+ j]= sharedMem[data.rows*data.columns+ j];
		}
			fprintf(out, "%c\n",'\n');
	
	fprintf(out, "SOLUTION MATRICES \n");
	fprintf(out, "\nBy SVD Method \n");
		for(j = 0; j < data.columns; ++j){
			fprintf(out, "%.5f ",sharedMemSolution [j]);
			m[j]=sharedMemSolution[j];
			sendToClients[(data.rows + 1)*data.columns + j]= sharedMemSolution[j];
		}
		fprintf(out, "\n\n\nBy QR Method \n");
		for(j = 0; j < data.columns; ++j){
			fprintf(out, "%.5f ",sharedMemSolution [data.columns + j]);
			sendToClients[(data.rows + 2)*data.columns + j]= sharedMemSolution[j];
		}
		fprintf(out, "\n\n\nBy Pseudo-Inverse Method \n");
		for(j = 0; j < data.columns; ++j){
			fprintf(out, "%.5f ",sharedMemSolution [2*data.columns + j]);
			sendToClients[(data.rows + 3)*data.columns + j]= sharedMemSolution[j];
		}
	
		fprintf(out, "%c\n",'\n');
		sendToClients[(data.rows + PROCESS)*data.columns]=ax_bNorm(matrix,m,matrixSol,data.columns);

		//send(socketo,sendToClients,(1+sizeof(double)*(data.rows + PROCESS)*data.columns),0);
		fprintf(out, "\n\n\nThe Norm of error : %f\n",ax_bNorm(matrix,m,matrixSol,data.columns));

		pthread_mutex_unlock(mylock);


	fflush(out);
	fclose(out);
	free(sendToClients);
	free(m);
	free(matrixSol);
	freeMatrix(matrix,data.rows);

}


void *wrapperSolverSVD(void *argm){
	int i=0;
	struct arguments *arg = argm;
	double *m;
	double **matrix=memAllocator(arg->rows,arg->columns);
	double *matrixSol = malloc(sizeof(double)*arg->columns);
	
	fprintf(stderr, "%s\n","a1" );
	pthread_mutex_lock(arg->mylock);
	while((*arg->flag)!= 1)
		pthread_cond_wait(arg->condVar, arg->mylock);
	
		*arg->flag2++;
		copyFrom1Dto2D(arg->sharedMem,matrix,arg->rows,arg->columns);
		copyFrom1Dto1D(arg->sharedMem,matrixSol,arg->rows*arg->columns,(arg->rows+1)*arg->columns);
	if(arg->rows < arg->columns){
		for (i = 0; i < arg->columns; ++i){
			arg->sharedMemSolution[i]= NAN;
		}
	}
	else{
		m = qrDecomposition(matrix,matrixSol,arg->rows,arg->columns);
		for (i = 0; i < arg->columns; ++i){
			arg->sharedMemSolution[i]= m[i];
		}
		free(m);
	}
	if(*arg->flag2==3){
		pthread_cond_signal(arg->condVar);
	}
	pthread_cond_signal(arg->condVar);
	pthread_mutex_unlock(arg->mylock);

	fprintf(stderr, "%s\n","a2");

	
	free(matrixSol);
	freeMatrix(matrix,arg->rows);
	
}

void *wrapperSolverQR(void *argm){
	struct arguments *arg = argm;
	int i=0;
	double *m;
	double **matrix=memAllocator(arg->rows,arg->columns);
	double *matrixSol = malloc(sizeof(double)*arg->columns);

	fprintf(stderr, "%s\n","b1" );
	pthread_mutex_lock(arg->mylock);
	while((*arg->flag)!= 1)
		pthread_cond_wait(arg->condVar, arg->mylock);
	
		*arg->flag2++;
		copyFrom1Dto2D(arg->sharedMem,matrix,arg->rows,arg->columns);
		copyFrom1Dto1D(arg->sharedMem,matrixSol,arg->rows*arg->columns,(arg->rows+1)*arg->columns);
	
	if(arg->rows < arg->columns){
		for (i = 0; i < arg->columns; ++i){
			arg->sharedMemSolution[arg->columns + i]= NAN;
		}
	}
	else{
		m = qrDecomposition(matrix,matrixSol,arg->rows,arg->columns);
		for (i = 0; i < arg->columns; ++i){
			arg->sharedMemSolution[arg->columns + i]= m[i];
		}
		free(m);
	}
	if((*arg->flag2)==3){
		pthread_cond_signal(arg->condVar);
	}
	pthread_cond_signal(arg->condVar);
	pthread_mutex_unlock(arg->mylock);

	fprintf(stderr, "%s\n","a2" );	
	
	free(matrixSol);
	freeMatrix(matrix,arg->rows);
	
}

void *wrapperSolverPseudoInverse(void *argm){
	int i=0;
	double *m;
	struct arguments *arg = argm;
	double **matrix=memAllocator(arg->rows,arg->columns);
	double *matrixSol = malloc(sizeof(double)*arg->columns);

	fprintf(stderr, "%s\n","c1" );
	pthread_mutex_lock(arg->mylock);
	while((*arg->flag)!= 1)
		pthread_cond_wait(arg->condVar,arg->mylock);
	
		*arg->flag2++;
		copyFrom1Dto2D(arg->sharedMem,matrix,arg->rows,arg->columns);
		copyFrom1Dto1D(arg->sharedMem,matrixSol,arg->rows*arg->columns,(arg->rows+1)*arg->columns);
	
	if(arg->rows < arg->columns){
		for (i = 0; i < arg->columns; ++i){
			arg->sharedMemSolution[2*arg->columns + i]= NAN;
		}
	}
	else{
		m = qrDecomposition(matrix,matrixSol,arg->rows,arg->columns);
		for (i = 0; i < arg->columns; ++i){
			arg->sharedMemSolution[2*arg->columns + i]= m[i];
		}
		free(m);
	}
	if((*arg->flag2)==3){
		pthread_cond_signal(arg->condVar);
	}
	pthread_cond_signal(arg->condVar);
	pthread_mutex_unlock(arg->mylock);

 	
	fprintf(stderr, "%s\n","c2" );	
	
	free(matrixSol);
	freeMatrix(matrix,arg->rows);
}


void *wrapperServe(void *argm){
	struct send arg = *((struct send*)argm);
	serve(arg.rows,arg.columns,arg);

}


int establish (unsigned short portnum){
	char myname[MAXHOSTNAME+1];
	int s;
	struct sockaddr_in sa;
	struct hostent *hp;
	memset(&sa, 0, sizeof(struct sockaddr_in));
	gethostname(myname, MAXHOSTNAME);
	hp= gethostbyname(myname);

		if (hp == NULL) /* we don't exist !? */
			return(-1);
		sa.sin_family= hp->h_addrtype;
		sa.sin_port= htons(portnum);   /* clear our address */

	if ((s= socket(AF_INET, SOCK_STREAM, 0)) < 0)   /* create socket  */
		return(-1);

	if (bind(s,(struct sockaddr *)&sa,sizeof(struct sockaddr_in)) < 0) {
		close(s);
		return(-1);
	
	}
	
	listen(s, 1);
	return(s);
}

int getConnection(int s){
	struct sockaddr_in netclient;
	int len = sizeof(struct sockaddr);
	int t;
	if ((t = accept(s,(struct sockaddr *)(&netclient), &len)) < 0)
		return(-1);
return(t);
}


static void sigHan(int sig, siginfo_t *info,void *context){
	int i=0;  

      	if(sig==SIGINT){
      		for (i = 0; i < count; ++i){
      			killpg(pidArrays[i],SIGUSR1);
      		}
      		printf("Exiting By SIGNAL\n");
	      	exit(1);

   	 	}
}