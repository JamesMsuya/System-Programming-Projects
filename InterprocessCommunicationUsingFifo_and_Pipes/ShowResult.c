#include "essentials.h"
#define FIFORESULT "result"
#define RESULT "result.txt"
#define SERVER "server.txt"
#define LOGRESUlT "resultsLog.txt"
#define CLIENTS 100
#define MILLI 1000


int showResult(int fdes,FILE *resultz);

int pidChecker(int pid);

static void sigHan(int sig, siginfo_t *info,void *context);


int pid;

int fd;

int fd2;

pid_t pidServer;

int count=0;

int pidSeeArr[CLIENTS];

char resultsLogFifo[10];

FILE *logResult;



int main(int argc, char const *argv[]){
	logResult=fopen(LOGRESUlT,"w");
	FILE * server = fopen(SERVER,"r");
	FILE *result =fopen(RESULT,"w");
	struct resStruct;
	struct sigaction act;
	memset(&act,0,sizeof(act));
	act.sa_sigaction = &sigHan;
	act.sa_flags = SA_SIGINFO;
	resultsLogFifo[0]='\0';
	pid=getpid();
	sprintf(resultsLogFifo,"%d",pid);
	memset(&pidSeeArr,0,sizeof(int)*CLIENTS);

	if(server==NULL){
		perror("Server is not running");
		fclose(logResult);
		fclose(result);
		exit(1);
	}
	fscanf(server,"%d",&pidServer);
	fclose(server);
	printf("%d\n",pidServer );
	if(result!=NULL){
		fprintf(result,"%d",pid);
		fclose(result);
	}
	

	if (sigaction(SIGINT,&act,0)==0){
		

	}
	if (sigaction(SIGUSR2,&act,0)==0){
		

	}

	
	mkfifo(FIFORESULT,0666);
	while (((fd2 = open(FIFORESULT, O_WRONLY)) == -1));
	
		write(fd2,&pid,sizeof(pid));
		close(fd2);	
	/*mkfifo(resultsLogFifo,0666);*/
		
	while (((fd = open(resultsLogFifo, O_RDONLY)) == -1));
	while(1){
		showResult(fd,logResult);

	}
	close(fd);
	return 0;
}





int showResult(int fdes,FILE *resultz){
	

	double time1=0;
	double time2=0;
	double pidClient=0;
	double result1=0;
	double result2=0;
	read(fdes,&result1,sizeof(result1));
	read(fdes,&time1,sizeof(time1));
	read(fdes,&result2,sizeof(result2));
	read(fdes,&time2,sizeof(time2));
	read(fdes,&pidClient,sizeof(pidClient));

	if(pidChecker((int)pidClient)==0){
		pidSeeArr[count]=pidClient;
		count++;
	}

	if(pidClient){
		printf("%.f       %e     %e\n",pidClient,result1,result2 );
		fprintf(resultz,"%.f \n",pidClient);
		fprintf(resultz,"%f, %f\n",result1,time1);
		fprintf(resultz,"%e, %f\n",result2,time2);
	}
	return 0;


	
}



static void sigHan(int sig, siginfo_t *info,void *context){
	int i=0;

      	if(sig==SIGINT){
      		for (i = 0; i < count;i++){
      			killpg(pidSeeArr[i],SIGUSR2);
    
      		}	
      		killpg(pidServer,SIGTERM);
      		fprintf(logResult,"Ctrl C was received. Exiting program");		
			fclose(logResult);
			close(fd);
			unlink(resultsLogFifo);
			unlink(RESULT);
			unlink(FIFORESULT);
	      	exit(1);

   	 	}

   	 	if(sig==SIGUSR2){
   	 		fprintf(logResult,"A kill signal was received from SeeWhat");
   	 		fclose(logResult);
   	 		close(fd);
   	 		unlink(resultsLogFifo);
   	 		unlink(RESULT);
   	 		unlink(FIFORESULT);
	      	exit(1);

   	 	}

  }

  int pidChecker(int pid){
  	int i=0;
  	for(i=0;i<count;i++){

  		if(pid==pidSeeArr[i] || pid==0){
  			return -1;
  		}
  	}

  	return 0;
  }