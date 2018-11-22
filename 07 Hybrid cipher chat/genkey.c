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

	printf("RSA Key Pair Production COMPLETE!\n");
	return 0;
}
