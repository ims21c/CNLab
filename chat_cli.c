#include <stdio.h> // 표준 입출력 라이브러리
#include <stdlib.h> // 문자열 변환, 의사 난수 생성, 동적 메모리 관리 등의 함수들을 포함
#include <string.h> // 메모리 블록이나 문자열을 다룰 수 있는 함수들을 포함
#include <strings.h> // BSD에서 사용되는 POSIX로 구현된 string 헤더
#include <fcntl.h>	// 파일컨트롤을 위한 헤더
#include <sys/socket.h> // 소켓 프로그래밍을 위한 헤더
#include <netinet/in.h> // 소켓 프로그래밍을 위한 헤더
#include <sys/time.h>	// C언어 시간 출력
#include <unistd.h>	//
#include <arpa/inet.h>

#define MAXLINE     1000
#define NAME_LEN    20

char *EXIT_STRING= "exit";
// 소켓 생성 및 서버 연결, 생성된 소켓리턴
int tcp_connect(int af, char *servip, unsigned short port);
void errquit(char *mesg) { perror(mesg); exit(1); }

int main(int argc, char *argv[]) {
	char bufall[MAXLINE+NAME_LEN], // 이름+메시지를 위한 버퍼
	     *bufmsg;  // bufall 에서 메시지부분의 포인터
	int maxfdp1, s, namelen;   // 최대 소켓번호, 소켓 디스크립터, 이름길이
	fd_set read_fds;

	if(argc != 4) {
		printf("사용법 : %s sever_ip  port name \n", argv[0]);
		exit(0);
	}

	sprintf(bufall, "[%s] :", argv[3]);  // bufall 의 앞부분에 이름을 저장
	namelen= strlen(bufall);
	bufmsg = bufall+namelen;  // 메시지 시작 부분 지정

	s = tcp_connect(AF_INET, argv[1], atoi(argv[2]));

	if(s==-1)
		errquit("tcp_connect fail");

	system("clear");
	puts("서버에 접속 중 입니다.......");

	maxfdp1 = s + 1;
	FD_ZERO(&read_fds);

	while(1) {
		FD_SET(0, &read_fds);
	        FD_SET(s, &read_fds);
        	
		if(select(maxfdp1, &read_fds, NULL,NULL,NULL) < 0)
            		errquit("select fail");
		
		if (FD_ISSET(s, &read_fds)) {
			int nbyte;
			
			if ((nbyte = recv(s, bufmsg, MAXLINE, 0)) > 0)  {
				bufmsg[nbyte] = 0;
				printf("%s \n", bufmsg);
//				printf("<번호/명령(go,exit)>: ");
			}
		}//end if
	
	        if (FD_ISSET(0, &read_fds)) {
        		if(fgets(bufmsg, MAXLINE, stdin)) {
 
	               		if (send(s, bufall, namelen+strlen(bufmsg), 0) < 0)
        				puts("Error : Write error on socket.");

               			if (strstr(bufmsg, EXIT_STRING) != NULL ) {
					puts("Good bye.");
					close(s);
					exit(0);
				}//end if
			}//end if
		} //end if
	} // end of while
}

int tcp_connect(int af, char *servip, unsigned short port) {
	struct sockaddr_in servaddr;
	int  s;
    	// 소켓 생성
	if ((s = socket(af, SOCK_STREAM, 0)) < 0)
    		return -1;
    	// 채팅 서버의 소켓주소 구조체 servaddr 초기화
    	bzero((char *)&servaddr, sizeof(servaddr));
    	servaddr.sin_family = af;
    	inet_pton(AF_INET, servip, &servaddr.sin_addr);
    	servaddr.sin_port = htons(port);

	// 연결요청
	if(connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr))< 0)
		return -1;
	
	return s;
}

