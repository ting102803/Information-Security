#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define BLOCK_SIZE 128
#define KEY_LENGTH 1024
#define PUB_EXP 3

int main()
{	
	char inputFileName[256];//입력 되는 파일 명
	char encoutputFileName[256];//암호화 출력되는 파일
	char decoutputFileName[256];//다시 복호화된 것을 출력하는 파일
	char in_buff[KEY_LENGTH]={0, };
	char out_buff[KEY_LENGTH]={0, };//RSA과정중 입력과 출력으로 사용할 임시 버퍼
	FILE *input_FD;
	FILE *output_FD;
	printf(">> Input file name : ");
	scanf("%s",inputFileName);//입력 받을 파일명 입력
	
	sprintf(encoutputFileName,"plain.enc");
	sprintf(decoutputFileName,"plain.enc.dec");
	//RSA 암호화 준비 단계
	RSA *keypair = RSA_generate_key(KEY_LENGTH,PUB_EXP,NULL,NULL);
	//앞서 지정한 키 길이로 키 페어 생성
	BIO *pri = BIO_new(BIO_s_mem());
	BIO *pub = BIO_new(BIO_s_mem());
	//BIO라는 객체를 초기화 하는 과정
	PEM_write_bio_RSAPrivateKey(pri,keypair,NULL,NULL,0,NULL,NULL);
	PEM_write_bio_RSAPublicKey(pub,keypair);

	size_t pri_len=BIO_pending(pri);
	size_t pub_len=BIO_pending(pub);

	char *pri_key=malloc(pri_len+1);
	char *pub_key=malloc(pub_len+1);

	BIO_read(pri,pri_key,pri_len);
	BIO_read(pub,pub_key,pub_len);
	//BIO정보를 읽어 문자열배열에 적재	
	pri_key[pri_len] = '\0';
	pub_key[pub_len] = '\0';

	FILE *key_file;
	key_file = fopen("prkey.pem","w");
	fprintf(key_file,"%s",pri_key);
	fclose(key_file);
	//개인키파일 생성
	key_file = fopen("pukey.pem","w");
	fprintf(key_file,"%s",pub_key);
	fclose(key_file);
	//공용키 파일 생성
	key_file = fopen("rsa.key","w");
	RSA_print_fp(key_file,keypair,0);
	fclose(key_file);
	//키페어 파일 생성
	input_FD=fopen(inputFileName,"rb");
	output_FD=fopen(encoutputFileName,"wb");//먼저 암호화부터
	int t=0;//암,복호화후 만들어진 파일의 길이를 저장하기위한 변수
	int k=0;//얼마나 파일을 읽었는지 저장하기위한 변수
	while(0<(k=fread(&in_buff,sizeof(char),117,input_FD))){//암호화시에는 블록사이즈가 128일때 최대 입력가능한 117단위 만큼 나눠서 읽으면 된다.
	t=RSA_public_encrypt(k,in_buff,out_buff, keypair, RSA_PKCS1_PADDING);
	if(ERR_get_error()){
	printf("RSA_public_encrypt failure: %d ret:%d\n", ERR_get_error(), t);
        return -1;
    	}//출력(t값)을 항상 블록사이즈로 나와야하는데 에러시에는 -1를 반환	
	fwrite(&out_buff,sizeof(char),t,output_FD);
	}//나온 출력값을 파일에 저장
	
	memset(in_buff,0,sizeof(char)*BLOCK_SIZE);	
	memset(out_buff,0,sizeof(char)*BLOCK_SIZE);
	//버퍼 초기화	
	fclose(output_FD);
	fclose(input_FD);
	
	input_FD=fopen(encoutputFileName,"rb");
	output_FD=fopen(decoutputFileName,"wb");
	//복호화를 위해 다시 파일을 설정
	while(fread(&in_buff,sizeof(char),BLOCK_SIZE,input_FD)){//복호화시에는 무조건 블록사이즈 단위로 읽으면 된다.
	t=RSA_private_decrypt(BLOCK_SIZE,in_buff,out_buff, keypair, RSA_PKCS1_PADDING);	
	fwrite(&out_buff,sizeof(char),t,output_FD);
	}//나온 길이 만큼 쓰기를 진행
	fclose(output_FD);
	fclose(input_FD);
	printf("RSA COMPLETE!\n");
	return 0;
}
