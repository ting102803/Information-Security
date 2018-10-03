#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CORRECT 6//정답값 지정
void main()
{
	int answer=0;
	printf("3 * 2 = ?\n\n\n");//문제 제시
	while(answer!=CORRECT){//정답이 아닐경우 무한 반복
	printf(">> Input : ");
	scanf("%d",&answer);
	if(answer==CORRECT)
	{
	printf("[!] Great Job~!\n");//정답일 경우 메세지	
	}
	else{
	printf("[?] %d is not answer\n\n",answer);
	}//정답이 아닐경우 메세지

	}
	exit(1);
}
