#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

#define OUTPUT_FILE "log.log"
#define TEMP_FILE_FOR_COUNT "counter.txt"
#define PATH_MAX 4096

/*Bu fonksiyon verilen "filename" isimli dosyayı açar
	gelen "str" stringini arar. */
int listFile(char *filename,char *str);
/*Bu fonksiyon gelen directory'i açar ,
fork yapar ona göre gerekli fonksiyonu çağırır
Child işi yapar,parent çocukları bekler.*/
int listDir(char *directory,char *str);
/*isdirectory fonksiyonunu "KİTAPTAN" aldım*/
int isdirectory(char * path);
/*Bu fonksiyon verilen path'ten filename'i bulur ve return eder.*/
char *findFileName(char *filename);
int main(int argc, char **argv){

	FILE *fpTemp,*fpOut;
	int occurance = 0 , i = 0;
		
	/* controls/open files*/
	if(argc!=3){
		
		fprintf(stderr, "Usage: %s string directory\n", argv[0]);
		exit(1);
	}

	if(listDir(argv[2],argv[1]) == -1){

		fprintf(stderr, "ListDir fonksiyonu hata verdi!\n");
		exit(1);
	}

	if((fpOut = fopen(OUTPUT_FILE,"a")) == NULL){

		fprintf(stderr, "%s couldn't open\n", OUTPUT_FILE);
		return -1;
	}

	if((fpTemp = fopen(TEMP_FILE_FOR_COUNT,"r")) == NULL){
			
			exit(1);

	}

	/*calculate total occurance*/
	while(fscanf(fpTemp,"%d",&i) != EOF){

		occurance+=i;
	}
	/*write total occurance to log.log*/
	fprintf(fpOut, "\n%d  %s were found in total\n",occurance,argv[1]);

	/*close files*/
	fclose(fpOut);
	fclose(fpTemp);

	/*remove temp file*/
	remove(TEMP_FILE_FOR_COUNT);

	return 0;
}

int listDir(char *directory,char *str){
	/*BU KODLARI KİTAPTAN ALDIM*/

	struct dirent *direntp;

	pid_t childPid;

	DIR *dirp;

	char pathOfFile[PATH_MAX];

	if((dirp = opendir(directory)) == NULL){

		perror("Failed to open directory");

		return -1;
	}

	while((direntp = readdir(dirp)) != NULL){

		if(strcmp("..", direntp->d_name) == 0 || strcmp(".", direntp->d_name) == 0)
			
			continue;
	/*BURAYA KADAR*/	

		/*Create new path*/
		strncpy(pathOfFile, directory, PATH_MAX);
		strcat(pathOfFile, "/");
		strcat(pathOfFile, direntp->d_name);

		/*FORK KODUNU KİTAPTAN ALDIM*/
		if((childPid = fork()) == -1){

			perror("Fork failed");

			return -1;
		}
		/*BURAYA KADAR*/

		/*child process*/
		if(childPid == 0){

			/*if directory call listDir*/
			if((isdirectory(pathOfFile)) != 0){

				if((listDir(pathOfFile,str)) == -1)

					return -1;
			}

			else{

				/*if file call listFile*/
				if((listFile(pathOfFile,str)) == -1)

					return -1;
			}

			/*kilss children*/
			exit(0);

		}

		/*parent process*/
		else{

			/*Tüm çocukları bekler*/
			while(wait(NULL)>0);

		}


	}

	/*BU KODU KİTAPTAN ALDIM*/
	while((closedir(dirp) == -1) && (errno == EINTR));

	return 0;
}

int listFile(char *filename,char *str){

	char *buffer, ch;
	FILE *fpInp,*fpOut,*fpTemp;
	int nLine=1, nCol=1, nNumOccurance=0,i;
	long lPosition;

	/*file open controls*/
	if((fpInp = fopen(filename,"r")) == NULL){
		
		fprintf(stderr, "%s couldn't open\n", filename);
		return -1;	
	}	
	if((fpOut = fopen(OUTPUT_FILE,"a")) == NULL){

		fprintf(stderr, "%s couldn't open\n", OUTPUT_FILE);
		return -1;
	}
	if((fpTemp = fopen(TEMP_FILE_FOR_COUNT,"a")) == NULL){

		fprintf(stderr, "%s couldn't open\n", TEMP_FILE_FOR_COUNT);
		return -1;
	}

	buffer = malloc((strlen(str)*sizeof(char)));

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

			while( i < strlen(str) && ch != EOF){

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
			if(strncmp(str, buffer, strlen(str)) == 0){
				
				nNumOccurance++;

				fprintf(fpOut, "%s: ", findFileName(filename));
				
				fprintf(fpOut,"[%d,%d] %s first character is found..\n",nLine, nCol-1,str);		
			}
			
			/* turn back to saved position */
			fseek(fpInp, lPosition, SEEK_SET);
		}
		
	}

	/*write occurance to temp file*/
	fprintf(fpTemp,"%d\n", nNumOccurance);
	/*close files*/
	free(buffer);
	fclose(fpInp);
	fclose(fpTemp);
	fclose(fpOut);

	return 0;
}

/*BU KODU KİTAPTAN ALDIM*/
int isdirectory(char *path){

	struct stat statbuf;

	if(stat(path,&statbuf) == -1)

		return 0;
	else

		return S_ISDIR(statbuf.st_mode);

}
/*Bu fonksiyon parametre olarak filename alır,sondan başlayarak verilen char pointer içerisinde '/' karakterini arar.
Bulduğunda mallocla yer alır ve bulması gereken stringi str'a atar ve return eder.*/
char * findFileName(char *filename){

	int i,j=0,k;

	char *str;


	for(i = strlen(filename)-1 ; i >= 0 ; i--){

		if(filename[strlen(filename-1)] == '/')

			return filename;
		/*if ch '/' */
		if(filename[i] == '/'){

			str=(char *)malloc(strlen(filename)-i-1);
			/*store in str*/
			for(k = i+1 ; k < strlen(filename) ; k++){

				str[j] = filename[k];

				j++;

			}
			/*put '\0' to end of char * */
			str[j] = '\0';

			return str;
			/*free malloc*/
			free(str);

			
		}

	}
	return " ";

}

