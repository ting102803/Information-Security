#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int GetFileSize(FILE * fp);
void Encrypt(char *buff,int intkey);
void Decrypt(char *buff,int intkey);
int main(void)
{
	char inputFileName[256];
	char outputFileName[256];
	FILE * input_FD;
	FILE * output_FD;
	int mode;
	char key[30];//영단어를 저장하기위한 char배열
	int intkey[30];//알파벳에 따른 숫자값을 구하기위한 정수배열
	printf(">> Input File Name : ");
	scanf("%s", inputFileName);

	printf(">> Input Mode [ 0 : Encrypt, 1 : Decrypt ] ");
	scanf("%d", &mode);

	printf(">> Input Key : ");
	scanf("%s", key);

	if(mode == 0){
		sprintf(outputFileName , "encrypt.txt");
	}
	else if (mode == 1 ){
		sprintf(outputFileName, "decrypt.txt");
	}
	else{
		printf("[!] Mode Error ! \n ");
		exit(1);
	}

	input_FD = fopen(inputFileName,"rb");
	output_FD = fopen(outputFileName,"wb");
	int fileSize = GetFileSize(input_FD);
	char buff;
	int i;
	for(i=0; i<strlen(key);i++){
		if(key[i]>=97&&key[i]<=122){
			intkey[i]=key[i]-97;
		}
		else if(key[i]>=65&&key[i]<=90){
			intkey[i]=key[i]-65;
		}
		else intkey[i]=key[i]%26;
	}
	//문자열값들을 숫자값으로 변환하는 과정이다.
	//대문자와 소문자일 경우에 따라 나눠서 각각에 a값만큼 빼준다
	for(i=0; i<fileSize-1;i++){
		fread(&buff,sizeof(char),1,input_FD);

		if(mode==0){
			Encrypt(&buff,intkey[i%strlen(key)]%26);
		}
		else if(mode==1){
			Decrypt(&buff,intkey[i%strlen(key)]%26);
		}//키값을 줄때 영단어보다 키값이 짧을경우 반복이 될수있게 처리를 하여야해서 %이용하여서 나머지를 이용하였다.

		fwrite ( &buff, sizeof(char),1,output_FD);
	}
	fwrite("\n",sizeof(char),1,output_FD);
	fclose(output_FD);
	fclose(input_FD);
	return 0;
}
int GetFileSize(FILE * fp)
{
	int offset_bkup;
	int fsize;

	offset_bkup = ftell(fp);
	fseek(fp,0,SEEK_END);
	fsize = ftell(fp);
	fseek(fp,offset_bkup,SEEK_SET);

	printf("FILE Size = %d\n",fsize);
	return fsize;
}

void Encrypt(char *buff,int intkey){
	int flag=0;//대문자인지 소문자인지 판단용 상수
	if(*buff>=65&&*buff<=90) flag=1;
	*buff = *buff+intkey;
	if((*buff>90&&flag==1)||*buff>122) *buff= *buff-26;
}	//만약 대문자였고 Z를 넘어 갔을경우에는 다시 A로 와야하고
	//소문자였고 z를 넘었을경우에는 다시 a로와야해서 -26을 한다

void Decrypt(char *buff,int intkey){
	int flag=0;//대문자인지 소문자인지 판단용 상수
   if(*buff>=97&&*buff<=122) flag=1;
	*buff = *buff-intkey;
 	if((*buff<97&&flag==1)||*buff<65) *buff=*buff+26;
}//이번에는 소문자일경우 flag=1이다 위와 다른이유는 복호화의 경우에는 값을 빼기를 하기때문에 소문자일경우에 확인을 해야한다.
