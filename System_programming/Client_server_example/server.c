#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#define PORT "3490"
#define BACKLOG 10              //一次 listen 多少
void sig_handler(int num)
{
    while(waitpid(-1,NULL,WNOHANG) > 0);
}
void *get_in_addr(struct sockaddr *sa)
{
    if(sa->sa_family == AF_INET)
        return &(((struct sockaddr_in *)sa)->sin_addr);
    else
        return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}
int main()
{
    struct addrinfo hints,*res,*p;
    struct sockaddr_storage their_addr;         //可以透過accept()取得連線者的位址
    struct sigaction sa;
    int yes = 1,rv,sockfd,newfd;
    char s[INET6_ADDRSTRLEN];                   //inet_ntop將位址存入
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;                 //自動填入本機ip
    if((rv = getaddrinfo(NULL,PORT,&hints,&res)) != 0)
    {
        fprintf(stderr,"getaddrinfo: %s\n",gai_strerror(rv));
        return 1;
    }
    fprintf(stdout,"getaddrinfo success\n");
    for(p=res; p!=NULL; p=p->ai_next)               //用迴圈找出可以連的
    {
       if((sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1)
       {
           perror("server error");
           continue;
       }
       if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)       //設定socket 
       {
           perror("setsockopt");
           exit(1);
       }
       if(bind(sockfd,p->ai_addr,p->ai_addrlen) == -1)
       {
            perror("server error");
            close(sockfd);
            continue;
       }
       break;
    }
    if(p == NULL)
    {
        fprintf(stderr,"sever fail to bind");
        exit(1);
    }
    fprintf(stdout,"bind success\n");
    if(listen(sockfd,BACKLOG) == -1)
    {
        perror("listen error");
        exit(1);
    }
    fprintf(stdout,"listen success\n");
    //設定sigaction
    sa.sa_handler = sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if(sigaction(SIGCHLD,&sa,NULL) == -1)
    {
        perror("sigaction error");
        exit(1);
    }
    fprintf(stdout,"signal register\n");
    while(1)
    {
        socklen_t sin_size = sizeof(their_addr);
        fprintf(stdout,"ready to accept\n");
        if((newfd = accept(sockfd,(struct sockaddr *)&their_addr,&sin_size)) == -1)
        {
            perror("accept error");
            exit(1);
        }
        inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *) &their_addr),s,sizeof(s));
        printf("get connection from %s\n",s);
        if(fork() == 0)     //呼叫child 和連線者打招呼
        {
            close(sockfd);
            if(send(newfd,"Hello world!",12,0) == -1)
            {
                perror("send error");
                exit(1);
            }
            close(newfd);
            exit(0);
        }
        close(newfd);
    }
return 0;
}
