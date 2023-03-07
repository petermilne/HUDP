/* udp_client.c:
 * send a file as as series of fixed size UDP datagrams at specified interval
 *
 * udpclient <IP address> <port> <sz> <file> <usecs>
 *
 * <file>=data set samples at fixed interval ('-' :: use stdin)
 * <sz>=sample size
 * <usecs> interval
 *
 * eg this runs nicely at 20kHz
 * ./udp_client 10.12.198.128 53676 128 shot_data 50
 *
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>

#include <sys/time.h>
#include <signal.h>

#define MAXDGRAM 10240

void missed_alarm(int signum)
{

}
int main(int argc, char**argv)
{
   	int sockfd;
	struct sockaddr_in servaddr;
#define ADDRP	(struct sockaddr *)&servaddr
#define ADDRSZ  sizeof(servaddr)
	char *sendbuf = malloc(MAXDGRAM);
	int nb;
	int maxdgram = MAXDGRAM;
	FILE* fp;
	struct itimerval timer;
	sigset_t alarm_sig;
	int signum;

	sigemptyset(&alarm_sig);
	sigaddset(&alarm_sig, SIGALRM);
	signal(SIGALRM, missed_alarm);

	timer.it_value.tv_sec = timer.it_interval.tv_sec = 0;

	if (getenv("MAXDGRAM")){
		maxdgram = atoi(getenv("MAXDGRAM"));		
	}
	sendbuf = malloc(maxdgram);

	if (argc < 6) {
		printf("usage:  udpclient <IP address> <port> <sz> <file> <usecs>\n");
		exit(1);
  	}

   	sockfd=socket(AF_INET,SOCK_DGRAM,0);

   	bzero(&servaddr,sizeof(servaddr));
   	servaddr.sin_family = AF_INET;
   	servaddr.sin_addr.s_addr=inet_addr(argv[1]);
   	servaddr.sin_port=htons(atoi(argv[2]));
	maxdgram = atoi(argv[3]);
	if (strcmp(argv[4], "-") == 0){
		fp = stdin;
	}else{
		fp = fopen(argv[4], "r");
		assert(fp);
	}
	timer.it_value.tv_usec = timer.it_interval.tv_usec = atoi(argv[5]);

	if (setitimer(ITIMER_REAL, &timer, 0) < 0) {
	    perror("setitimer");
	    exit(1); 
	}
	while(1){
		while((nb = fread(sendbuf, 1, maxdgram, fp)) > 0){
			sigwait(&alarm_sig, &signum);
			sendto(sockfd, sendbuf, nb, 0, ADDRP, ADDRSZ);
		}
		rewind(fp);
	}
	return 0;
}
