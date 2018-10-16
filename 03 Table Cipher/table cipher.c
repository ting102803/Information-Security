#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int GetFileSize(FILE * fp);
void enc(char *buff,int key,char table[]);
void dec(char *buff,int key,char table[]);
void main()
{
	char inputFileName[256];
	char outputFileName[256];
	FILE *input_FD;
	FILE *output_FD;

	int mode;
	char keytable[]="koegbpdwitncmfryqzulashxvj";//키테이블 값 선언
	int key1,key2;//평행이동할 키값 저장용
	printf(">> Input file name : ");
	scanf("%s",inputFileName);
	printf(">> Input mode [ 0 : ENC , 1: DEC ]");
	scanf("%d",&mode);//암복호화 선택
	
	printf("key table : %s\n",keytable);
	printf("input 2 key : ");
	scanf("%d %d",&key1,&key2);//평행이동할 키값을 받음
	//2개를 받는데 한개의 키값으로 진행하고싶다면 중복으로 입력
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
	
	int flag=0;//평행이동 키값 순환을 위한 변수
	int key_modified;//평행이동된 값을 저장하기위한 변수
	
	for(i=0;i<fileSize;i++)
	{
		fread(&buff,sizeof(char),1,input_FD);
		if(buff<65||(buff>90&&buff<97)||buff>122) {//문자열이 아니라면 그대로 넘어간다		
		}		
		else if(flag==0){//첫번째 문자열은 그대로 키값 사용
		key_modified=0;
		flag=1;
		}
		else if(flag==1){//첫번째 키값만큼 평행이동 필요
		key_modified-=key1;//평행 이동시킨다는건 테이블이 이동했다고 생각해야함으로 인덱스를 -나 +를 해야한다 둘중 아무거나를 해도 어차피 암,복호화 과정이 반대라서 한개는 보정을 해야해서 어느것을 해도 상관이없다
		flag=2;	//다음에는 두번째 키값으로 가야함으로 flag수정	
		}
		else{//두번째 키값만큼 평행이동 필요
		key_modified-=key2;
		flag=1;	//다음에는 첫번째 키값으로 가야함으로 flag수정
		}
		if(key_modified<0){
			key_modified+=26;
		}//계산시 음수가 나올경우 +26을 더하여 보정함으로 배열접근을 유지
		if(mode==0)
		{
			enc(&buff,key_modified,keytable);
		}
		else if(mode==1)
		{
			dec(&buff,key_modified,keytable);
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
void enc(char *buff,int  key,char table[]){
	int index=0;
	if(*buff>=65&&(*buff<=90))//대문자일경우
	{
		index=*buff-65;//인덱스값으로 접근하여야함으로 A만큼 빼준다
		*buff=table[(index+key)%26];//그 후 들어온 키값을 이용하여 테이블에서 변경할 키값을 찾는다
	}else if(*buff>=97&&(*buff<=122)){//소문자일경우
		index=*buff-97;
		*buff=table[(index+key)%26];
	}//소문자도 동일하다


}	

void dec(char *buff,int key,char table[]){
	int i=0;//키테이블은 소문자이니까
	if(*buff>=97&&(*buff<=122)){//소문자일 경우만 생각하면된다.
		for(i=0;i<26;i++){
			if(table[i]==*buff)break;		
		}//들어온 암호화된 소문자의 인덱스를 찾는 과정
		*buff=((i+26-key)%26)+65;//이번에는 방향이 반대로이기때문에 키값을 빼서 위치를 찾아야하는데 음수가 되니 +26을하여 보정한뒤 나머지 연산으로 찾은후 +65를 해주어 대문자로 치환한다.
	}

}
