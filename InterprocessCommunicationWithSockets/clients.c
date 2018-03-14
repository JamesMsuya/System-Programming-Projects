#include "essentials.h"
#define PORTNUM 50000
#define PROCESS 3
#define MAXHOSTNAME 255
#define MILLI 1000
#define PATH "./logsClients/"
#define SEM "/sem" /*a named semaphore*/
#define PERMS (mode_t)(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define FLAGS (O_CREAT | O_EXCL)
  
struct send{
	int rows;
	int columns;
	int clients;
	pthread_t tid;
	pid_t pid;
	double timeT;
	int socket;
};

struct arguments{
	int rows;
	int columns;
	int clients;
	sem_t *sem;
};

void sendRequest(int row,int columns,int clients);

int callSocket(char *hostname, unsigned short portnum);

static void sigHan(int sig, siginfo_t *info,void *context);

int establish (unsigned short portnum);

void *sendToServer(void *argm);

int socketFd;

double average=0;

double aveSquare=0;

struct timeval now;

FILE *out;




int main(int argc, char const *argv[]){
	char sasa[255];
	sem_t *sem =NULL;
	gettimeofday(&now,NULL);
	struct sigaction act;
	memset(&act,0,sizeof(act));
	sprintf(sasa, "logClients_%d.txt",getpid());
	out=fopen(sasa,"w");
	act.sa_sigaction = &sigHan;
	act.sa_flags = SA_SIGINFO;
	sem_unlink(SEM);
	getnamed(SEM,&sem,1);
	sem_close(sem);

	if (sigaction(SIGINT,&act,0)==0){
		
		

	}
	if(argc < 4){
		printf("clients <#of columns of A, m> <#of rows of A, p> <#of clients, q>\n");
		sem_close(sem);
		sem_unlink(SEM);
		return 0;
	}

	sendRequest(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]));
	
	fclose(out);
	sem_unlink(SEM);
	return 0;
} 


void sendRequest(int rows,int columns,int clients){
	int i=0;
	struct arguments arg;
	pthread_t *tid = malloc(clients*sizeof(pthread_t)); 
	sem_t *sem=sem_open(SEM, 1);
	memset(tid,0,clients*sizeof(pthread_t));
	char hostname[MAXHOSTNAME+1];
	gethostname(hostname,MAXHOSTNAME);
	socketFd=callSocket(hostname,PORTNUM);
	if(socketFd<0){
		printf("Server is not running\n");
		sem_close(sem);
		free(tid);
		return;
	}
	arg.rows=rows;
	arg.columns=columns;
	arg.clients=clients;
	arg.sem=sem;

	for (i = 0; i < clients; ++i){
		pthread_create(&tid[i], NULL, sendToServer,&arg);
	}

	for (i = 0; i < clients; ++i){
		pthread_join(tid[i],NULL);
	}

	fprintf(out,"\n Average waiting time(in milliseconds) is %f.\n",average/clients);
	fprintf(out,"\n Standard deviation is %f.\n",sqrt((aveSquare-average)/(clients-1)));
	close(socketFd);
	sem_close(sem);
	free(tid);
}




int callSocket(char *hostname, unsigned short portnum){
	struct sockaddr_in sa;
	struct hostent *hp;
	int a, s;

		if ((hp= gethostbyname(hostname)) == NULL){
			errno= ECONNREFUSED;
			return(-1);
		}
	memset(&sa,0,sizeof(sa));
	memcpy((char *)&sa.sin_addr,hp->h_addr,hp->h_length);
	sa.sin_family= hp->h_addrtype;
	sa.sin_port= htons((u_short)portnum);


	if ((s= socket(hp->h_addrtype,SOCK_STREAM,0)) < 0)
		return(-1);

	if (connect(s,(struct sockaddr *)&sa,sizeof(sa)) < 0){ /* connect */
		close(s);
		return(-1);
	}
	 /* if connect */
	return(s);
}

void *sendToServer(void *argm){
	int i=0; 
	int j=0;
	int tempo=0;
	struct arguments *arg = argm;
	double timeT;
	struct send data;
	struct timeval next;
	char file[255];
	char temp[255];
	FILE *out;
	file[0]='\0';
	temp[0]='\0';
	data.rows = arg->rows;
	data.columns = arg->columns;
	data.clients = arg->clients;
	data.pid = getpid();
	data.tid = pthread_self();
	strcpy(file,PATH);
	sprintf(temp,"%lu.txt",data.tid);
	strcat(file,temp);
	/*out=fopen(file,"w");*/
	double *sendToClients= malloc(1+sizeof(double)*(data.rows + PROCESS)*data.columns);

	/*if (out==NULL){
		perror("Out is null\n" );
	
	}*/


	sem_wait(arg->sem);
		gettimeofday(&next,NULL);
		timeT=(double)(next.tv_sec-now.tv_sec)*MILLI + (double)(next.tv_usec - now.tv_usec)/MILLI;
		average += timeT;
		aveSquare += (timeT*timeT);
		data.timeT=timeT;
		send(socketFd,&data,sizeof(struct send),0);

	sem_post(arg->sem);

	/*sem_wait(arg->sem);
		tempo=recv(socketFd,sendToClients,(1+sizeof(double)*(data.rows + PROCESS)*data.columns),0);
	sem_post(arg->sem);

	if (tempo!=0){
		
	
		fprintf(out, "\nGenerated A matrix \n");
		for(i = 0; i < data.rows; ++i){
			for(j = 0; j < data.columns; ++j)
				fprintf(out, "%.5f ",sendToClients[i*data.columns + j]);
		
			fprintf(out, "%c\n",'\n');
		}

		fprintf(out, "\nGenerated B matrix \n");
		for(j = 0; j < data.columns; ++j)
			fprintf(out, "%.5f ",sendToClients[data.rows*data.columns+ j]);
			fprintf(out, "%c\n",'\n');
	
		fprintf(out, "SOLUTION MATRICES 'X' \n");
		fprintf(out, "\nBy SVD Method \n");
		for(j = 0; j < data.columns; ++j){
			fprintf(out, "%.5f ",sendToClients[(data.rows + 1 )*data.columns+ j]);
		}

		fprintf(out, "\n\n\nBy QR Method \n");
		for(j = 0; j < data.columns; ++j){
			fprintf(out, "%.5f ",sendToClients[(data.rows + 2 )*data.columns+ j]);
		}
		fprintf(out, "\n\n\nBy Pseudo-Inverse Method \n");
		for(j = 0; j < data.columns; ++j){
			fprintf(out, "%.5f ",sendToClients[(data.rows + 3 )*data.columns+ j]);
		}
	
		fprintf(out, "%c\n",'\n');
		fprintf(out, "\n\n\nThe Norm of error : %f\n",sendToClients[(data.rows + PROCESS)*data.columns]);
	}
*/
		

	free(sendToClients);
	/*fclose(out);*/
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
	
	return(s);
}




static void sigHan(int sig, siginfo_t *info,void *context){
	  

      	if(sig==SIGINT){
      		close(socketFd);
      		sem_unlink(SEM);
	      	exit(1);

   	 	}


   	 	if(sig==SIGUSR1){
   	 		close(socketFd);
      		sem_unlink(SEM);
	      	exit(1);

   	 	}

  }