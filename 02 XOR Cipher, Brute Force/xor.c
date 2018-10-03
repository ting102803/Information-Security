#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int GetFileSize(FILE * fp);
void enc(char *buff,int key);
void dec(char *buff,int key);
void main()
{
	char inputFileName[256];
	char outputFileName[256];
	FILE *input_FD;
	FILE *output_FD;

	int mode;
	char key[26];

	printf(">> Input file name : ");
	scanf("%s",inputFileName);
	printf(">> Input mode [ 0 : ENC , 1: DEC ]");
	scanf("%d",&mode);//암복호화 선택

	printf("input key : ");
	scanf("%s",key);//키입력

	if (mode==0)
	{
		sprintf(outputFileName,"encrypt.txt");
	}
	else if(mode == 1)
	{
		sprintf(outputFileName,"decrypt.txt");
	}
	else{
		printf("[!] Mode Error!\n");
		exit(1);
	}

	input_FD=fopen(inputFileName,"rb");
	output_FD=fopen(outputFileName,"wb");

	int fileSize = GetFileSize(input_FD);//파일 사이즈 계산
	char buff;
	int i;
	
	int len;
	int key_modified;

	for(i=0;i<fileSize;i++)
	{
		fread(&buff,sizeof(char),1,input_FD);

		len = i % strlen(key);//키값이 반복되야 하기때문에 나머지 계산을 이용
		key_modified=key[len];//나온 나머지값의 인덱스값이 암,복호화에 사용될 진짜 값이다
		if(mode==0)
		{
			enc(&buff,key_modified);
		}
		else if(mode==1)
		{
			dec(&buff,key_modified);
		}//암복호화 진행

		fwrite(&buff,sizeof(char),1,output_FD);
	}
	fclose(output_FD);
	fclose(input_FD);
}

int GetFileSize(FILE * fp)
{
	int offset_bkup;
	int fsize;

	offset_bkup = ftell(fp);
	fseek(fp,0,SEEK_END);//맨끝 포인터로 옮긴다
	fsize = ftell(fp);//해당값이 사이즈라서 저장한다
	fseek(fp,offset_bkup,SEEK_SET);//포인터를 원상복귀한다.
	printf("FILE Size = %d\n",fsize);
	return fsize;
}
void enc(char *buff,int  key){
	*buff = *buff^key;
}	

void dec(char *buff,int key){
	*buff = *buff^key;
}//XOR의 경우에는 역원의 경우에도 XOR이기 때문에 암,복호화 과정이 둘다 XOR로 같다.
