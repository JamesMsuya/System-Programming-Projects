#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>



int counter(const char *str,char *str2,const char* str1,FILE *out,FILE *tempFile);

void push(char *str,char c);

void push1(int *str,int c);

int countInDir(char *str,const char *str1,FILE *out,FILE *temp);

int isDirectory(const char *path);

int isFile(const char *path);




int main(int argc, char *argv[]){
	FILE *out;
	FILE *temp;
	int num=0;
	int numTemp=0;

	out=fopen("log.txt","w");
	temp=fopen("temp.txt","w");

	if (argc != 3){
		fprintf(stderr, "USAGE: ./%s 'string' directory \n",argv[0]);
		return 1;
	}
	else{
		 countInDir(argv[2],argv[1],out,temp);
	}

	fclose(temp);
	temp=fopen("temp.txt","r");
	while(!feof(temp)){

		numTemp=0;
		fscanf(temp,"%d",&numTemp);
		num+=numTemp;

	}
	fprintf(out,"\n%d %s were found in total.\n",num,argv[1]);
	fflush(out);
	fclose(out);
	fclose(temp);
	remove("temp.txt");

return 0;
}



int countInDir(char *str,const char *str1,FILE *out,FILE *temp){
	struct dirent *direntp=NULL;
	DIR *dirp=NULL;
	char cwd[1024];
	int pid;

	
	cwd[1023]='\0';

	if ((dirp = opendir(str)) == NULL){
		perror ("Failed to open directory");
		return 1;
	}

	while ((direntp = readdir(dirp))!= NULL){

		pid=fork();

		strcpy(cwd,str);
		strcat(cwd,"/");
		strcat(cwd,direntp->d_name);

			
			if(pid == 0){


				if(direntp->d_name[0]!='~'){

					if(isFile(cwd)){
				   	
						counter(cwd,direntp->d_name,str1,out,temp);
						fclose(out);
						while ((closedir(dirp) == -1) && (errno == EINTR));	
						exit(1);
					}
					
					if(isDirectory(cwd) && (strcmp(direntp->d_name,"..")!=0) && (strcmp(direntp->d_name,".")!=0)){

						strcpy(str,cwd);
						while ((closedir(dirp) == -1) && (errno == EINTR));
						countInDir(str,str1,out,temp);
						fclose(out);
						fclose(temp);
						exit(1);

					}
					else{
						while ((closedir(dirp) == -1) && (errno == EINTR));
						fclose(out);
						fclose(temp);
						exit(1);
					}

				}
				else{
					while ((closedir(dirp) == -1) && (errno == EINTR));
					fclose(out);
					fclose(temp);
					exit(1);
				}

			}	
						
		}

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
int counter(const char* str,char *str2,const char* str1,FILE *out,FILE *tempFile){
    FILE* input=NULL;
    char *temp;
    int *temp1;
    int *temp2;
    char chr='0';
    int lcv=1;
    int lcv1=0;
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
	    
	    while(chr!=EOF){
	        chr= '-';

	        for(lcv1=1;(chr !='\n' && chr!= EOF);lcv1++){
	            chr = fgetc(input);
	           
	            if(chr!=9 && chr!=32 && chr!='\n'){
		            push(temp,chr); 
		            push1(temp1,lcv1);
		            push1(temp2,lcv);

		            if(strcmp(str1,temp)==0){

		            	fprintf(out,"%s: [%d, %d] first character is found.\n",str2,temp2[0],temp1[0]);
		            	fflush(out);

		                num++;
		            }
	           	}
	        }
	        lcv++;
	        
	    }
	    
	   
	    fprintf(tempFile,"%d\n",num);
	    fflush(tempFile);
	    free(temp);
	    free(temp1);
	    free(temp2);
	    fclose(tempFile);
	    fclose(input);
	 }
    return (num); 
}


/**
*This function pushes the first element out of the string and append a new 
*element.
*/

void push(char *str,char c){
    int size = strlen(str);
    int lcv=0;
    
    for(lcv=0;lcv<size;lcv++){
        str[lcv]=str[lcv+1];   
    }
    str[size-1]=c;
}

/**
*This function pushes the first element out of the string and append a new 
*element. This is a special case of functon push but for integers.
*/
void push1(int *str,int c){
    int lcv=0;
    int size=sizeof(str)/sizeof(int);
    
    for(lcv=0;lcv<size-1;lcv++){
        str[lcv]=str[lcv+1];   
    }
    str[size-1]=c;

}