#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int GetFileSize(FILE * fp);
void Encrypt(char *buff,int key);
void Decrypt(char *buff,int key);
int main(void)
{
	char inputFileName[256];
	char outputFileName[256];
	FILE * input_FD;
	FILE * output_FD;
	int mode; //모드 선택을 위한 변수
	int key; //키값 입력을 받기위한 변수

	printf(">> Input File Name : ");
	scanf("%s", inputFileName);

	printf(">> Input Mode [ 0 : Encrypt, 1 : Decrypt ] ");
	scanf("%d", &mode);

	printf(">> Input Key : ");
	scanf("%d", &key);
	//파일이름과 모드 키값을 입력받음
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
	//모드에 따른 암호화,복호화 완료 파일 지정
	input_FD = fopen(inputFileName,"rb");
	output_FD = fopen(outputFileName,"wb");
	//2진수로 파일을 읽고 쓴다
	int fileSize = GetFileSize(input_FD);//파일 사이즈 계산
	char buff;
	int i;
	for(i=0; i<fileSize-1;i++){
		fread(&buff,sizeof(char),1,input_FD);

		if(mode==0){
			Encrypt(&buff,key);
		}
		else if(mode==1){
			Decrypt(&buff,key);
		}
		fwrite (&buff, sizeof(char),1,output_FD);
	}//모드에 따라 적절한 함수를 실행한후 결과문자열을 작성한다
	//마지막 문자열까지 복호,암호화 된다면 오류날수가 있어서 제외하였다. 	
	fwrite ("\n", sizeof(char),1,output_FD);//단어 맨끝엔 문자열 변경을 넣어서 출력을 깔끔하게해주었다.
	fclose(output_FD);
	fclose(input_FD);
	return 0;
}
int GetFileSize(FILE * fp)
{
	int offset_bkup;
	int fsize;

	offset_bkup = ftell(fp);//파일 포인터의 현재위치를 저장
	fseek(fp,0,SEEK_END);//파일 포인터를 파일끝으로 이동
	fsize = ftell(fp);//파일 포인터의 현재위치를 저장
	fseek(fp,offset_bkup,SEEK_SET);//파일 포인터를 다음 작업을 위해 저장한 원래위치로 옮긴다.

	printf("FILE Size = %d\n",fsize);
	return fsize;
}

void Encrypt(char *buff,int  key){
	key = key%26;//알파벳은 26개이기때문에 0~25로만 판단
	int flag=0;//대문자인지 소문자인지 판단용 상수
	if(*buff>=65&&*buff<=90) flag=1;//대문자일경우 flag=1
	*buff = *buff+key;//문자열에 키값만큼 더한다
	if((*buff>90&&flag==1)||*buff>122) *buff= *buff-26;
}	//만약 대문자였고 Z를 넘어 갔을경우에는 다시 A로 와야하고
	//소문자였고 z를 넘었을경우에는 다시 a로와야해서 -26을 한다

void Decrypt(char *buff,int key){
	key = key%26;
 	int flag=0;
   	if(*buff>=97&&*buff<=122) flag=1;//이번에는 소문자일경우 flag=1이다 위와 다른이유는 복호화의 경우에는 값을 빼기를 하기때문에 소문자일경우에 확인을 해야한다.
	*buff = *buff-key;
 	if((*buff<97&&flag==1)||*buff<65) *buff=*buff+26;
}//만약 소문자이고 a보다 작거나 대문자이고 A보다 작을경우 알파벳 한바퀴를 넘었기때문에 +26을 해준다
