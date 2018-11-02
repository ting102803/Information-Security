#define BLOCK_SIZE 128
#define KEY_SIZE 8

#include "chat.h"

unsigned int des(unsigned char * msg,unsigned char *key, unsigned int msg_len, int mode);
TALK	msg;
char key[KEY_SIZE];//키를 저장하기위해 전역변수로 선언

void * send_msg(void * arg)
{
	int sock = (int)arg;
	while(1) {
		fgets(msg.msg, MSG_SIZE, stdin);		
		printf("\033[F\033[J");//라인 정리
		fprintf(stdout, "[%s] %s", msg.name, msg.msg);//메세지 깔끔히 출력
		des(msg.msg,key, strlen(msg.msg), 1);//Des 암호화
		write(sock, (void *)&msg, sizeof(TALK));//전송
      memset(msg.msg, 0x0, MSG_SIZE);//초기화
	}
}

void * recv_msg(void * arg)
{
	int sock = (int)arg;
	int str_len;
	int padding;
	while(1) {
		TALK tmp;
		str_len = read(sock, (void *)&tmp, sizeof(TALK));
		if ( -1 == str_len ) {
			return (void *)1;
		}//메세지를 읽는다
		padding=des(tmp.msg,key, BLOCK_SIZE, 2);//des복호화후 패딩값을 저장
		tmp.msg[padding]='\0';//마지막 위치에 0을 지정하여서 출력시 패딩부분 잘리도록 설정
		fprintf(stdout, "[%s] %s", tmp.name, tmp.msg);//출력
		memset(tmp.msg, 0x0, MSG_SIZE);//초기화
	}
}

int main(int argc, char *argv[])
{	
	
	int					serv_sock;
	struct sockaddr_in	serv_addr;

	pthread_t			send_thread, recv_thread;
	void *				thread_result;
	// SIG HANDLER
	signal(SIGINT, (void *)handler);
	if ( argc != 5 ) {
		fprintf(stderr, "Usage : %s <ip> <port> <name> <key>\n", argv[0]);
		exit(1);
	}//ip port 이름에 키 값까지 인자를 더받아야한다
	sprintf(msg.name, "%s", argv[3]);
	sprintf(key, "%s", argv[4]);//키값과 이름은 저장
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);//소켓 생성
	if ( -1 == serv_sock ) {
		fprintf(stderr, "[!] ERROR : Socket()\n");
	}//생성이 잘됐는지 확인
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= inet_addr(argv[1]);
	serv_addr.sin_port			= htons(atoi(argv[2]));
	//주소와 포트를 지정
	

	if ( -1 == connect(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) ) {
		fprintf(stderr, "[!] ERROR : Connect()\n");
	}//소켓을 연결

	pthread_create(&send_thread, NULL, send_msg, (void *)serv_sock);
	pthread_create(&recv_thread, NULL, recv_msg, (void *)serv_sock);

	pthread_join(send_thread, &thread_result);
	pthread_join(recv_thread, &thread_result);
	//쓰레드로 생성후 작동
	close(serv_sock);

	return 0;
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
		}//들어온 입력값이 블럭사이즈보다 작다면 나머지 칸을 패딩으로 다 채워준다
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
		}//패딩값 만큼의 비트수가 패딩값으로 채워져 있는지 확인. 다르다면 break문으로 중단
		if(count==1)
		result = BLOCK_SIZE-padding;
		else result =BLOCK_SIZE;
	}//패딩이 있다면 count가 1까지 감소 했을 것임으로 result는 블럭사이즈에서 패딩값을 빼준다.
	memcpy(msg,block_out,BLOCK_SIZE);

	return result;
}
