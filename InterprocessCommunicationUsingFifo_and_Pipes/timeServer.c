#include "essentials.h"
#define MILLI 1000
#define REQUESTNO 1000
#define LOGFILE "logServer.txt"
#define SERVER "server.txt"
#define CLIENTS 1000


FILE *logFile;

double **matrixGenerator( int dimension);

int pidChecker(int pid);

int serve(int dimension,struct timeval timePrev,FILE *logFile,int pid);

static void sigHandler(int sig,siginfo_t *info,void *context);

void sigHan(int sig);

void sigAlarm(int tick);

int fifoChecker(int pid);

int temp=0;

int i=0;

int ticks;

int pidClients[CLIENTS];

int count=0;

int request[REQUESTNO];




int main(int argc, const char **argv){
	struct timeval timePrev;
	memset(pidClients,0,sizeof(int)*CLIENTS);
	gettimeofday(&timePrev, NULL);
	int childPid;
	int secChild;
	logFile = fopen(LOGFILE,"w");
	FILE *server = fopen(SERVER,"w");
	struct sigaction act;
	pid_t pid=getpid();
	fprintf(server,"%d",pid);
	fclose(server);
	if(argc!=4){
		perror("Usage : ./timeServer <ticks> <dimension> <fifo>");
		if ((logFile!=NULL)){
			fclose(logFile);
		}
		unlink(SERVER);
		exit(0);
	}
	ticks = atoi(argv[1]);
	int dimension = atoi(argv[2]);
	memset(request,0,sizeof(int)*REQUESTNO);
	memset(&act,0,sizeof(act));
	act.sa_sigaction = &sigHandler;
	act.sa_flags = SA_SIGINFO;
	

/*Signal handlers */
	
	if (sigaction(SIGINT,&act,0)==0){
		

	}
	if (sigaction(SIGUSR2,&act,0)==0){
		

	}
	if (sigaction(SIGTERM,&act,0)==0){
		

	}
	if(sigaction(SIGALRM,&act,0)==0){

	}



	if(logFile==NULL){
		perror("log File could not be created");
		unlink(SERVER);
		exit(1);
	}

 	/*Child process to alarm the parent process to handle request in an interval of time given as a main parameter*/
	if((secChild=fork())!=-1){
		if(secChild==0){
			while(1){
				alarm(ticks);
			}
		}

	}

	if (sigaction(SIGUSR1,&act,0)==0){
		
	

	}

	while(1){
		
		
		if(i!=0){
			sleep(1);
			/*Achild process to handle request*/
			if((childPid=fork())!=-1){
				if(childPid==0){	
					serve(dimension,timePrev,logFile,request[i-1]);
					fclose(logFile);				
					exit(1);
				}
			}

		}
		
	}

	while(wait(NULL) > 0);
	unlink(SERVER);
	fclose(logFile);



	return 0;
}




int serve(int dimension,struct timeval timePrev,FILE *logFile,int pid){
	int fd;
	int i=0;
	int j=0;
	int dim= 2*dimension;
	double **matrix=matrixGenerator(dim);
	struct timeval current;
	gettimeofday(&current, NULL);
	char fifoClient[10];
	fifoClient[0]='\0';
	sprintf(fifoClient,"%d",pid);

	mkfifo(fifoClient,0666);
	while (((fd = open(fifoClient, O_WRONLY)) == -1)); 
	fprintf(logFile,"Time: %f pid: %d determinant: %f\n",(double) ( current.tv_usec - timePrev.tv_usec) / MILLI + 
								(double) (current.tv_sec - timePrev.tv_sec )*MILLI,pid,determinant(matrix,dim));
	write(fd,&dim,sizeof(dim));

	for(i = 0; i < dim; i++){
	
		for(j = 0; j < dim; j++){
		
			write(fd,&matrix[i][j],sizeof(matrix[i][j]));

		}
	}
	fprintf(stdout,"Time: %f pid: %d determinant: %f\n",(double) ( current.tv_usec - timePrev.tv_usec ) / MILLI + 
								(double) (current.tv_sec - timePrev.tv_sec )*MILLI,pid,determinant(matrix,dim));
	
	freeMatrix(matrix,dim);
	close(fd);
	fflush(logFile);

	return 0;
}



double **matrixGenerator( int dimension){
	int i=0;
	int j = 0;
	double ** matrix = memAllocator(dimension);

	srand(time(NULL));
	while(determinant(matrix,dimension)==0){
		for(i = 0; i < dimension; ++i){
			for(j = 0; j < dimension; ++j)
				matrix[i][j] = rand()%9 + 1;
			
		}
	}
	return matrix;
}


/*the handler for all the signals that are coming to the server*/
static void sigHandler(int sig, siginfo_t *info,void *context){
	int j=0;
       if(sig == SIGUSR1){

       		temp = info->si_value.sival_int;
       		if(count < 100 && pidChecker(temp)==0){
	       		pidClients[count]=temp; 
	       		count++;
	       	}
	       	if(i<REQUESTNO){
	       		request[i]=temp;
       			i++;     	
       		}
      	}

      	if(sig==SIGINT){

			while(wait(NULL) > 0);
			for (j = 0; j < count;j++){
      			killpg(pidClients[j],SIGUSR1);
    
      		}
      		unlink(SERVER);	
			fclose(logFile);
	      	exit(1);

   	 	}

   	 	if(sig==SIGUSR2){
 	
			
			while(wait(NULL) > 0);
			for (j = 0; j < count;j++){
      			killpg(pidClients[j],SIGUSR1);
    
      		}
			unlink(SERVER);
			fclose(logFile);
	      	exit(1);
   	 	}

   	 	if(sig==SIGTERM){
   	 		
 
			while(wait(NULL) > 0);
			unlink(SERVER);
			fclose(logFile);
	      	exit(1);

   	 	}
   	 	if(sig==SIGALRM){

   	 		i=0;


   	 	}

  }


void sigAlarm(int tick){

	usleep(tick/MILLI);
	kill(getpid(),SIGALRM);

}

int pidChecker(int pid){
  	int j=0;
  	for(j=0;j<count;j++){

  		if(pid==pidClients[j]){
  			return -1;
  		}
  	}

  	return 0;
  }