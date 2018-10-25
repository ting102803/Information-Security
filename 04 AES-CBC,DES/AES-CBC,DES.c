#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/des.h>
#include <openssl/aes.h>
#define BLOCK_SIZE 128
#define KEY_SIZE 8

unsigned int aes_cbc(unsigned char * msg,unsigned char *key, unsigned int msg_len, int mode);
unsigned int des(unsigned char * msg,unsigned char *key, unsigned int msg_len, int mode);
void main()
{
	char inputFileName[256];
	char outputFileName[256];
	FILE *input_FD;
	FILE *output_FD;

	int mode;
	char key[30]={0, };
	int cipher;
	int t=0;
	char buff[BLOCK_SIZE]={0, };

	printf(">> Input mode [ 1 : ENC , 2 : DEC ]");
	scanf("%d",&mode);//암복호화 선택
	printf(">> Input file name : ");
	scanf("%s",inputFileName);
	printf("input key : ");
	scanf("%s",key);//평행이동할 키값을 받음
	printf(">> Input Cipher mode [ 1 : aes-cbc , 2 : aes-ctr 3 : des ]");
	scanf("%d",&cipher);//암호화 형식선택
	
	if (mode==1)
	{
		sprintf(outputFileName,"plain.enc");
	}
	else if(mode == 2)
	{
		sprintf(outputFileName,"plain.enc.dec");
	}
	else{
		printf("[!] Mode Error!\n");
		exit(1);
	}

	input_FD=fopen(inputFileName,"rb");
	output_FD=fopen(outputFileName,"wb");
	while(0<(t=fread(&buff,sizeof(char),BLOCK_SIZE,input_FD))){
		int res=0;
		if(cipher==1){
			res = aes_cbc(buff,key,t,mode);//1번일 경우 cbc진행
		}
		else if(cipher==2){
			//2번 ctr은 미구현
		}
		else if(cipher==3){
			res = des(buff,key,t,mode);//3번일 경우 des 진행
		}
		else printf("[!] Cipher mode Error!\n");

		if(res>0&&res<BLOCK_SIZE)
		fwrite(&buff,sizeof(char),res,output_FD);		
		else fwrite(&buff,sizeof(char),BLOCK_SIZE,output_FD);
		//리턴된 res값을 이용하여서 파일에 써준다
		//블럭사이즈보다 작고 0보다 크다면 해당하는 숫자를
		//그 외에는 블럭사이즈로 한다 블럭사이즈는 128로 지정하였다.
		memset(buff,0,sizeof(char)*BLOCK_SIZE);	
	}
	
	fclose(output_FD);
	fclose(input_FD);
	printf("[!] Cipher Complete\n");
}


unsigned int aes_cbc(unsigned char * msg,unsigned char *key, unsigned int msg_len, int mode)
{
	AES_KEY  aes_ks;
	unsigned char iv[AES_BLOCK_SIZE];
	unsigned int i,result,padding;
	unsigned char block_in[BLOCK_SIZE] = {0, };
	unsigned char block_out[BLOCK_SIZE] = {0, };
	result=0; 	
	memset(iv, 0, sizeof(iv));
	memcpy(block_in, msg, msg_len);
	if(mode==1){
		if(msg_len <BLOCK_SIZE){
			padding = BLOCK_SIZE - msg_len;
			int count=padding;
			while(count>=1){
			block_in[BLOCK_SIZE -count] = padding;
			count--;
			}//들어온 입력값이 블럭사이즈보다 작다면 나머지 칸을 패딩으로 다 채워준다
		}//맨마지막만 채우는 식으로 패딩을하면 복호화시 꽉찬 블럭을 복호화할때 에러 발생해서 패딩 방법을 바꿨다.
		AES_set_encrypt_key(key,BLOCK_SIZE,&aes_ks);
		AES_cbc_encrypt(block_in,block_out,BLOCK_SIZE,&aes_ks,iv,AES_ENCRYPT);
		result=BLOCK_SIZE;//암호화하고 나온값은 항상 블럭사이즈이다.
	}
	else if(mode ==2){
		AES_set_decrypt_key(key,BLOCK_SIZE,&aes_ks);
		AES_cbc_encrypt(block_in,block_out,BLOCK_SIZE,&aes_ks,iv,AES_DECRYPT);
		//복호화는 무조건 블럭사이즈로해서 일단 바로 복호화를 진행한다. 
		padding = block_out[BLOCK_SIZE-1];//맨 마지막 비트를 패딩이라고 가정
		int count=padding;
		while(count>=2){
		if( block_out[BLOCK_SIZE-count]!=block_out[BLOCK_SIZE-count+1]) break;
		count--;
		}
		//패딩값 만큼의 비트수가 패딩값으로 채워져 있는지 확인. 다르다면 break문으로 중단
		if(count==1)
		result = BLOCK_SIZE-padding;
		else result=BLOCK_SIZE;
	}//패딩이 있다면 count가 1까지 감소 했을 것임으로 result는 블럭사이즈에서 패딩값을 빼준다.
	memcpy(msg,block_out,BLOCK_SIZE);
	return result;
}

unsigned int des(unsigned char * msg,unsigned char *key, unsigned int msg_len, int mode)
{  
	DES_key_schedule des_ks;
	DES_cblock  des_key = {0, };
	DES_cblock  iv = {0, };
	unsigned int i,result, padding;

	unsigned char block_in[BLOCK_SIZE] = {0, };
	unsigned char block_out[BLOCK_SIZE] = {0, };

	DES_string_to_key(key,&des_key);
	DES_set_key_checked(&des_key,&des_ks);

	memcpy(block_in, msg, msg_len);

	if(mode==1){
		if(msg_len <BLOCK_SIZE){
			padding = BLOCK_SIZE - msg_len;
			int count=padding;
			while(count>=1){
			block_in[BLOCK_SIZE -count] = padding;
			count--;
			}
		}//cbc와 동일하게 패딩
	DES_ncbc_encrypt(block_in,block_out,BLOCK_SIZE,&des_ks,&iv,DES_ENCRYPT);
		result=BLOCK_SIZE;
	}//암호화 할땐  결과가 항상 블럭사이즈
	else if(mode==2){
	DES_ncbc_encrypt(block_in,block_out,BLOCK_SIZE,&des_ks,&iv,DES_DECRYPT);
		padding = block_out[BLOCK_SIZE-1];
		int count=padding;
		while(count>=2){
		if( block_out[BLOCK_SIZE-count]!=block_out[BLOCK_SIZE-count+1]) break;
		count--;
		}//복호화 할때도 cbc와 동일하게 패딩 확인 후 제거
		if(count==1)
		result = BLOCK_SIZE-padding;
		else result =BLOCK_SIZE;
	}
	memcpy(msg,block_out,BLOCK_SIZE);

	return result;
}
