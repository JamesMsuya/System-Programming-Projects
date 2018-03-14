/*************************************************************************************************** 
 *Lcv stands for loop control variable                                                             *
 *This programs finds matching string pattern in a directory and logs 							   *
 *some important infomation about it.                                                              *
 *It logs the row and column the first letter of the string was found. The total number of matches,*
 *total run time and number of created thradds and processes                                       *
 *created By James Msuya                                                                           *
***************************************************************************************************/


#include <dirent.h>
#include <fcntl.h>  
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include <semaphore.h>
#include <pthread.h>
#define MILLI 1000
#define FIFO "fifo" /*fifo for calculating*/
#define CASCADE "cascade" /*Fifo forcalculating cascade number of threads and total directories searched*/
#define FIF0SEM "fifosem"  /*This is a fifo to be mutual exclusive accessed by a semaphore*/
#define SEM "/sem" /*a named semaphore*/
#define PERMS (mode_t)(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define FLAGS (O_CREAT | O_EXCL)


struct arguments{
	int fd;
	char str[1024];
	char str2[1024];
	char str1[1024];
	FILE *out;
	int fdes;
	int fileDescr;
	sem_t *sem;

};


int counter(const char *str,char *str2,const char* str1,FILE *out,int *lines);

void push(char *str,char c);

void push1(int *str,int size,int c);

int countInDir(char *str,const char *str1,FILE *out,int fd,int fd3,int fd5,int fileDescr);

int dirCounter(const char *str);

int isDirectory(const char *path);

int isFile(const char *path);

int filesInDirCount(char *str);

void *wrapper(void *argm);

int getnamed(char *name, sem_t **sem, int val);

static void sigHandler(int sig, siginfo_t *info,void *context);

int condition=0;

pthread_mutex_t mylock;

pthread_cond_t condVar;

int checker=0;



	


int main(int argc, char *argv[]){
	FILE *out;
	int num=0;
	int val=2;
	int count=0;
	struct timeval now;
	struct timeval next;
	int numTemp=0;
	int fd;
	int fd2;
	int fd3;
	int fd5;
	int fileDescr;
	int fileDescr2;
	int error=0;
	int lines=0;
	int filesNo=0;
	int temp1=1;
	char temp[10];
	sem_t *sem =NULL;
	struct sigaction act;
	memset(&act,0,sizeof(act));
	act.sa_sigaction = &sigHandler;
	act.sa_flags = SA_SIGINFO;
	temp[0]='\0';
	strcpy(temp,"temp");
	gettimeofday(&now,NULL);
	out=fopen("log.txt","w");

	sem_unlink(SEM);
	getnamed(SEM,&sem,1);
	/*Signal handlers */	
	if (sigaction(SIGINT,&act,0)==0){
		

	}

	unlink(temp);
	mkfifo(FIF0SEM,0666);
	mkfifo(temp,0666);
	mkfifo(FIFO,0666);
	mkfifo(CASCADE,0666);
	
	while (((fd = open(temp, O_RDWR)) == -1) && (errno == EINTR)); 

	while (((fd3 = open(FIFO, O_RDWR)) == -1) && (errno == EINTR)); 

	while (((fd5 = open(CASCADE, O_RDWR)) == -1) && (errno == EINTR)); 

	while (((fileDescr = open(FIF0SEM, O_RDWR)) == -1) && (errno == EINTR));

	write(fileDescr,&numTemp,sizeof(int));
	write(fileDescr,&numTemp,sizeof(int));


	if (argc != 3){
		fprintf(stderr, "USAGE: ./%s 'string' <directory> \n",argv[0]);
		close(fd);
		fclose(out);
		return 1;
	}
	else{
		 error = countInDir(argv[2],argv[1],out,fd,fd3,fd5,fileDescr);
	}

	
	while (((fd2 = open(temp, O_RDONLY)) == -1) && (errno == EINTR));
	close(fd);
	while(temp1){

		numTemp=0;
		temp1=read(fd2,&numTemp,sizeof(num));
		num+=numTemp;
		if (temp1){
			filesNo++;
		}
		
	}

	close(fd2);
	while (((fd = open(FIFO, O_RDONLY)) == -1) && (errno == EINTR)); 
	close(fd3);
	temp1=1;
	while(temp1){
		numTemp=0;
		temp1=read(fd,&numTemp,sizeof(numTemp));
		lines+=numTemp;
	}

	close(fd);
	while (((fd2 = open(CASCADE, O_RDONLY)) == -1) && (errno == EINTR)); 
	close(fd5);

	while (((fileDescr2 = open(FIF0SEM, O_RDONLY)) == -1) && (errno == EINTR));
	close(fileDescr);
	
		temp1=read(fileDescr2,&val,sizeof(numTemp));


	count=dirCounter(argv[2]);



	
	close(fileDescr2);
	unlink(temp);
	unlink(CASCADE);
	unlink(FIF0SEM);
	sem_close(sem);
	sem_unlink(SEM);
	unlink(FIFO);
	fprintf(out,"\n%d %s were found in total.\n",num,argv[1]);
	fflush(out);
	fclose(out);

	gettimeofday(&next,NULL);
	/***********************************************/
		/*Summary*/
	printf("Total number of strings found : %d\n",num);
	printf("Number of directories searched: %d\n",count+1);
    printf("Number of files searched:  %d\n",filesNo);
    printf("Number of lines searched:  %d\n",lines);
    printf("Number of cascade threads created: ");
    temp1=1;
	while(temp1){
		numTemp=0;
		temp1=read(fd2,&numTemp,sizeof(numTemp));
		
		if(temp1){
			printf("%d ",numTemp);
		}
	}
	close(fd2);
    printf("\nNumber of search threads created: %d\n",filesNo);
    printf("Max # of threads running concurrently: %d\n",val);
    printf("Total run time, in milliseconds. %.3f\n",((double)(next.tv_sec-now.tv_sec)*MILLI + (double)(next.tv_usec - now.tv_usec)/MILLI));
    if(error==0 && checker==0){
    	 printf("Exit condition: normal.\n");
    }
    else if(error!=0 && checker==0){
    	 printf("Exit condition:due to error #%d\n",error);
    }
    else if(checker==1){
    	 printf("Exit condition:due to signal no# %d\n",SIGINT);
     }

	/***********************************************/

return 0;
}



int countInDir(char *str,const char *str1,FILE *out,int fd,int fd3,int fd5, int fileDescr){
	struct arguments argm;
	struct arguments *argArr;
	pthread_t tid;
	pthread_t pth;
	struct dirent *direntp=NULL;
	DIR *dirp=NULL;
	char cwd[1024];
	int pid;
	int lcv=0;
	int fildes[2];
	int num=0;
	int fdes[2];
	int fdes2[2];
	int tempNum=0;
	sem_t *sem=sem_open(SEM, 1);
	int value=filesInDirCount(str);
	pthread_cond_init(&condVar, NULL);
	pthread_mutex_init(&mylock, NULL);
	cwd[1023]='\0';
	argm.str[0] ='\0';
	argm.str2[0] = '\0';
	argm.str1[0] = '\0';
	argArr= malloc(sizeof(argm)*filesInDirCount(str));

	write(fd5,&value,sizeof(value));
	

	if ((dirp = opendir(str)) == NULL){
		perror ("Failed to open directory");
		sem_close(sem);
		free(argArr);
		return SIGPIPE;
	}

	if (pipe(fildes) == -1) {
		free(argArr);
		sem_close(sem);
      	perror("failed to create the pipe"); 
      	return SIGPIPE; 
  	} 

	if (pipe(fdes) == -1) {
		free(argArr);
		sem_close(sem);
      	perror("failed to create the pipe"); 
      	return SIGPIPE; 
  	} 

  	if (pipe(fdes2) == -1) {
		free(argArr);
		sem_close(sem);
      	perror("failed to create the pipe"); 
      	return SIGPIPE; 
  	} 


	while ((direntp = readdir(dirp))!= NULL){

		
		strcpy(cwd,str);
		strcat(cwd,"/");
		strcat(cwd,direntp->d_name);



		if(direntp->d_name[0]!='~' && (strcmp(direntp->d_name,"..")!=0) && (strcmp(direntp->d_name,".")!=0)){

			if(isFile(cwd)){
				/*thread*/ /*pipe*/
		   		
				strcpy(argm.str,cwd);
				strcpy(argm.str2,direntp->d_name);
				strcpy(argm.str1,str1);
				argm.out = out;
				argm.fd = fildes[1];
				argm.sem=sem;
				argm.fdes = fdes2[1];
				argm.fileDescr=fileDescr;
				argArr[lcv]=argm;


				lcv++;

				pthread_create(&tid, NULL, wrapper,&argArr[lcv-1]);
			
				write(fdes[1],&tid,sizeof(tid));

			}
					


			if(isDirectory(cwd) && (strcmp(direntp->d_name,"..")!=0) && (strcmp(direntp->d_name,".")!=0)){

				pthread_mutex_lock(&mylock);
					
					condition=1;
					if ((pid = fork()) == -1) {
	      				perror("failed to fork"); 
	      				sem_close(sem);
	      				return -1;
	  				}
	  				condition=0;
	  				
     			pthread_mutex_unlock(&mylock);

				if(pid == 0){
					/*fifo*/
					strcpy(str,cwd);
					while ((closedir(dirp) == -1) && (errno == EINTR));
					close(fdes[0]);
					close(fdes[1]);
					close(fdes2[0]);
					close(fdes2[1]);
					close(fildes[0]);
					close(fildes[1]);
					free(argArr);
					condition=0;
					sem_close(sem);
					countInDir(str,str1,out,fd,fd3,fd5,fileDescr);
					close(fdes[0]);
					close(fdes[1]);
					fclose(out);
					exit(1);

				}
			
			}
	
		}

	}	

		close(fdes[1]);
		while(read(fdes[0],&pth,sizeof(pth))){
			pthread_join(pth,NULL);

		}
		close(fdes[0]);
		free(argArr);
		pthread_mutex_destroy(&mylock);
		close(fildes[1]);
		while(read(fildes[0],&tempNum,sizeof(tempNum))){

			write(fd,&tempNum,sizeof(num));

		}

		close(fdes2[1]);
		while(read(fdes2[0],&tempNum,sizeof(tempNum))){
			write(fd3,&tempNum,sizeof(num));

		}

		close(fildes[0]);
		close(fdes2[0]);
		sem_close(sem);

		while(wait(NULL) > 0);

		
		
	while ((closedir(dirp) == -1) && (errno == EINTR));


	
return 0;

}


/**
 *The function to check if the given path is directory. 
 */
int isDirectory(const char *path){
   struct stat buf;
   if (stat(path, &buf)!= 0){
       return 0;
   }

   return S_ISDIR(buf.st_mode);
}


/**
 *The function to check if the given path is a regular file. 
 */
int isFile(const char *path){
   struct stat buf;
   if (stat(path, &buf)!= 0){
       return 0;
   }

   return S_ISREG(buf.st_mode);
}
/**
 * This function counts the occurance of a given string and prints
 * its coordinates and totol of them in the file.
 */
int counter(const char* str,char *str2,const char* str1,FILE *out,int *lines){
    FILE* input=NULL;
    char *temp;
    int *temp1;
    int *temp2;
    char chr='0';
    int lcv=1;
    int lcv1=0;
    int counter=0;
    int size=strlen(str1);
    int num=0;
    int i=0;
    
    input=fopen(str,"r");

    if(input==NULL){
        perror("file unavailable");
        return(-1);
    }
    else{
	    temp=(char *)malloc((size+1)*sizeof(char));
	    temp1=(int *)malloc(sizeof(int)*(size));
	    temp2=(int *)malloc(sizeof(int)*(size));
	    
	    for(i = 0;i<size;i++){
	        temp[i]='x';
	        temp1[i]=0;
	        temp2[i]=0;
	    }
	    temp[size]='\0';
	    
	    while(!feof(input)){
	        chr= '-';
	        counter++;
	        for(lcv1=1;(chr !='\n' && !feof(input));lcv1++){
	            
	            fread(&chr,sizeof(char),1,input);
	            
	            if(chr!=9 && chr!=32 && chr!='\n' && !feof(input)){
		            push(temp,chr); 
		            push1(temp1,size,lcv1);
		            push1(temp2,size,lcv);

		            if(strcmp(str1,temp)==0){

		            	fprintf(out,"%d - %lu %s: [%d, %d] %s first character is found.\n",getpid(),pthread_self(),str2,temp2[0],temp1[0],str1);
		            	fflush(out);

		                num++;
		            }
	           	}
	        }
	        lcv++;
	        
	    }
	    
	    free(temp);
	    free(temp1);
	    free(temp2);
	    fclose(input);
	 }
	*lines=counter;
    return (num); 
}


/**
*This function pushes the first element out of the string and append a new 
*element.
*/

void push(char *str,char c){
    int size = strlen(str);
    int lcv=0;
    
    for(lcv=0;lcv<size-1;lcv++){
        str[lcv]=str[lcv+1];   
    }
    str[size-1]=c;
}

/**
*This function pushes the first element out of the string and append a new 
*element. This is a special case of functon push but for integers.
*/
void push1(int *str,int size,int c){
    int lcv=0;
    
    for(lcv=0;lcv<size-1;lcv++){
        str[lcv]=str[lcv+1];   
    }
    str[size-1]=c;

}


/*A wrapper function to be passed into a thread*/
void *wrapper(void *argm){
	int num=0;
	int lines=0;
	int fd;
	int num1=0;
	int num2=0;
	struct arguments *arg = argm;
	sem_wait(arg->sem);
	read(arg->fileDescr,&num1,sizeof(num1));
	read(arg->fileDescr,&num2,sizeof(num2));
	num2+=1;
	if(num1 < num2){
		num1=num2;
	}

	write(arg->fileDescr,&num1,sizeof(num1));
	write(arg->fileDescr,&num2,sizeof(num2));

	sem_post(arg->sem);

	pthread_mutex_lock(&mylock);
	

	
	while (condition==1)
   		pthread_cond_wait(&condVar, &mylock);
	
	num = counter(arg->str,arg->str2,arg->str1,arg->out,&lines);
	write(arg->fd,&num,sizeof(num));
	write(arg->fdes,&lines,sizeof(lines));
	
	pthread_cond_signal(&condVar);
	pthread_mutex_unlock(&mylock);

	sem_wait(arg->sem);
	read(arg->fileDescr,&num1,sizeof(num1));
	read(arg->fileDescr,&num2,sizeof(num2));
	
	num2--;
	write(arg->fileDescr,&num1,sizeof(num1));
	write(arg->fileDescr,&num2,sizeof(num2));

	sem_post(arg->sem);


	
	
}



/*
 *Traverses in a file and calculate the number of files in it.
 */
int filesInDirCount(char *str){
	int count=0;
	struct dirent *direntp=NULL;
	DIR *dirp=NULL;
	char cwd[1024];
	cwd[1023]='\0';

	
	if ((dirp = opendir(str)) == NULL){
		perror ("Failed to open directory");
		return 0;
	}

	while ((direntp = readdir(dirp))!= NULL){

		
		strcpy(cwd,str);
		strcat(cwd,"/");
		strcat(cwd,direntp->d_name);


		if(direntp->d_name[0]!='~' && (strcmp(direntp->d_name,"..")!=0) && (strcmp(direntp->d_name,".")!=0)){

			if(isFile(cwd)){
				count++;
			}
		}
	}
	while ((closedir(dirp) == -1) && (errno == EINTR));
	return count;
}



int dirCounter(const char *str){
	struct dirent *direntp=NULL;
	DIR *dirp=NULL;
	char cwd[1024];
	cwd[1023]='\0';
	int count=0;

	
	if ((dirp = opendir(str)) == NULL){
		perror ("Failed to open directory");
		return 0;
	}

	if(!isDirectory(str)){
		while ((closedir(dirp) == -1) && (errno == EINTR));
		return 0;
	}
	while ((direntp = readdir(dirp))!= NULL){

		
		strcpy(cwd,str);
		strcat(cwd,"/");
		strcat(cwd,direntp->d_name);


		if(direntp->d_name[0]!='~' && (strcmp(direntp->d_name,"..")!=0) && (strcmp(direntp->d_name,".")!=0)){

			if(isDirectory(cwd)){
				
				count += (1+ dirCounter(cwd));
				
			}
		}
	}
	while ((closedir(dirp) == -1) && (errno == EINTR));
	return count;
}

/**/
static void sigHandler(int sig, siginfo_t *info,void *context){
	
      	if(sig==SIGINT){
      		checker=1;
      		killpg(getpid(),SIGINT);
      		
	      	exit(1);

   	 	}
}

int getnamed(char *name, sem_t **sem, int val) {
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