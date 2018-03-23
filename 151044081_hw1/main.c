#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*
THIS FUNCTION SEARCHES STRING IN A FILE

TAKES AS ARGUMAN->ARGV

AND LIST THE OCCURANCE WITH BELOW ROW AND COLUMN

*/
void list(char **argv);
int main(int argc, char **argv){
		
	/* controls/open files*/
	if(argc!=3){
		
		fprintf(stderr, "Usage: %s filename string\n", argv[0]);
		exit(1);
	}

	list(argv);/*call function*/
		
	return 0;
}

void list(char **argv){

	char *buffer, ch;
	FILE *fpInp;
	int nLine=1, nCol=1, nNumOccurance=0,i;
	long lPosition;

	/*file open control*/
	if((fpInp=fopen(argv[2],"r")) == NULL){
		
		perror("Input file couldn't opened");
		exit(1);	
	}	
	
	buffer = malloc((strlen(argv[1])*sizeof(char)));
	
	while(fscanf(fpInp, "%c", &ch) != EOF){
	
		/* save file pointers position*/
		lPosition = ftell(fpInp);
		
		/*check new line*/
		if(ch == '\n'){
			
			nLine++;
			nCol = 1;
		}
		/*check space and tab*/
		else if(ch == ' ' || ch == '\t'){

			nCol++;
		}
		else{
			/*store first ch in buffer[0]*/
			buffer[0] = ch;

			nCol++;
			/*i is a counter for use in while's condition*/
			i = 1;
			/*get another character*/
			ch = fgetc(fpInp);

			while( i < strlen(argv[1]) && ch != EOF){

				/*if character is legal,store in buffer*/
				if(ch != ' ' && ch != '\t' && ch != '\n'){
					/*store buffer with ch*/
					buffer[i] = ch;

					i++;

				}
				/*get another character again for the while loop*/
				ch = fgetc(fpInp);
			}
			
			/* if buffer is same with argv[1],increase occurance*/
			if(strncmp(&argv[1][0], buffer, strlen(argv[1])) == 0){
				
				nNumOccurance++;
				
				printf( "[%d,%d] konumunda ilk karakter bulundu.\n",nLine, nCol-1);		
			}
			
			/* turn back to saved position */
			fseek(fpInp, lPosition, SEEK_SET);
		}
		
	}
	
	printf("%s  %d kez bulundu\n", argv[1], nNumOccurance);
	
	free(buffer);
	fclose(fpInp);

}

