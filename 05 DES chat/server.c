#include "chat.h"

#define CLNT_MAX_NUM 5

int clnt_num = 0;
int clnt_socks[CLNT_MAX_NUM];

void * clnt_manage(void * arg)
{
	int		clnt_sock	= (int)arg;
	int		str_len		= 1;
	int		i, j;

	TALK	msg;

	while ( 0 != (str_len = read(clnt_sock, (void *)&msg, sizeof(TALK))) ) {
		pthread_mutex_lock(&mutex);//쓰레드 사용시 뮤텍스를 이용하여서 상호배제가 일어나도록 구현
		for ( i = 0 ; i < clnt_num ; i++ ) {
			if ( clnt_sock != clnt_socks[i] ) {
				write(clnt_socks[i], (void *)&msg, str_len);
			}//클라이언트 소켓의 메세지를 읽고 저장
		}
		fprintf(stdout, "[%s] %s", msg.name, msg.msg);
		memset(msg.msg, 0x0, sizeof(msg.msg));
		pthread_mutex_unlock(&mutex);
	}

	// client exit
	sprintf(msg.msg, "--- Exit ---\n");
	str_len = strlen(msg.msg);
	fprintf(stdout, "[%s] %s", msg.name, msg.msg);

	pthread_mutex_lock(&mutex);
	for ( i = 0 ; i < clnt_num ; i++ ) {
		if ( clnt_sock == clnt_socks[i] ) {
			for ( j = i ; j < clnt_num - 1 ; j++ ) {
				clnt_socks[j] = clnt_socks[j+1];
			}
		}//클라이언트가 종료 됐을때 한개를 줄이기위한 과정
		else {
			write(clnt_socks[i], (void *)&msg, NAME_SIZE + str_len);
		}
	}
	pthread_mutex_unlock(&mutex);
}

int main(int argc, char *argv[])
{
	int					serv_sock;
	struct sockaddr_in	serv_addr;

	int					clnt_sock;
	struct sockaddr_in	clnt_addr;
	socklen_t			clnt_addr_size;

	pthread_t			thread;

	// SIG HANDLER
	signal(SIGINT, (void *)handler);

	if ( argc != 2 ) {
		fprintf(stderr, "Usage : %s <port>\n", argv[0]);
		exit(1);
	}//인자 숫자가 맞지않을경우 오류 메세지

	if ( pthread_mutex_init(&mutex, NULL) ) {
		fprintf(stderr, "[!] ERROR : Mutext Init\n");
	}//쓰레드의 뮤텍스 초기화 실패시 에러메세지

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if ( -1 == serv_sock ) {
		fprintf(stderr, "[!] ERROR : Socket()\n");
	}//소켓단에서 연결이 실패할시 에러메세지
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= htonl(INADDR_ANY);
	serv_addr.sin_port			= htons(atoi(argv[1]));

	if ( -1 == bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) ) {
		fprintf(stderr, "[!] ERROR : Bind()\n");
	}//바인드를 해준다.실패시 에러 메세지
	if ( -1 == listen(serv_sock, CLNT_MAX_NUM) ) {
		fprintf(stderr, "[!] ERROR : Listen()\n");
	}//서버 소켓이 클라이언트 요청을 기다리고 실패시 에러 메세지

	while(1) {
		clnt_addr_size = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
		//클라이언트의 접속을 허락해주는 함수로 성공했으면 소켓번호로 양수를 리턴해준다.
		if ( -1 == clnt_sock ) {
			fprintf(stderr, "[!] ERROR : Accept()\n");
		}

		pthread_mutex_lock(&mutex);
		clnt_socks[clnt_num++] = clnt_sock;//클라이언트가 늘어날때마다 추가
		pthread_create(&thread, NULL, clnt_manage, (void *)clnt_sock);//새로운 쓰레드 생성
		pthread_mutex_unlock(&mutex);
		fprintf(stdout, "[!] New User : %s\n", (char *)inet_ntoa(clnt_addr.sin_addr));
		//생성완료 메세지 출력
	}

	return 0;
}
