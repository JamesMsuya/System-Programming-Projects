/* 
 * File:   main.c
 * Author: James Msuya
 *
 * Created on February 27, 2017, 1:48 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int counter(const char *str,const char* str1);

void push(char *str,char c);

void push1(int *str,int c);

/*
 * 
 */
int main(int argc, char const *argv[]){
	

    if(argc==3){
       
        counter(argv[2],argv[1]);
    }
    else{
    	fprintf(stderr, "USAGE: progname 'string' filename \n");
    }
    return (0);
}

/**
 * This function counts the occurance of a given string and prints
 * its coordinates and totol of them in the file.
 */
int counter(const char* str,const char* str1){
    FILE* input=NULL;
    char *temp;
    int *temp1;
    int *temp2;
    char chr='0';
    int lcv=1;
    int lcv1;
    int size=strlen(str1);
    int num=0;
    
    input=fopen(str,"r");

    if(input==NULL){
        printf("file unavailable");
    }
    else
    {
    	temp=(char *)malloc((size+1)*sizeof(char));
	    temp1=(int *)malloc(sizeof(int)*(size));
	    temp2=(int *)malloc(sizeof(int)*(size));
	    
	    for(int i=0;i<size;i++){
	        temp[i]='-';
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

		            	printf("[%d, %d] konumunda ilk karakter bulundu.\n",temp2[0],temp1[0]);
		                num++;
		            }
	           	}
	        }
	        lcv++;
	        
	    }
	    
	    printf("\n%d adet %s bulundu.\n",num,str1);
	    free(temp);
	    free(temp1);
	    free(temp2);
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
    int lcv;
    
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
    int lcv;
    int size=sizeof(str)/4;
    
    for(lcv=0;lcv<size-1;lcv++){
        str[lcv]=str[lcv+1];   
    }
    str[size-1]=c;

}
