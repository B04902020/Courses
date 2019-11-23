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
#define MAXDATASIZE 100
void *get_addr_info(struct sockaddr *sa)
{
    if(sa->sa_family == AF_INET)
        return &(((struct sockaddr_in *)sa)->sin_addr);
    else
        return  &(((struct sockaddr_in6 *)sa)->sin6_addr); 
}
int main(int argc, char *argv[])
{
    struct addrinfo hints,*servinfo,*p;
    int sockfd,nbytes;
    char s[INET6_ADDRSTRLEN],buf[MAXDATASIZE];
    if(argc != 2)
    {
        fprintf(stderr,"argc error\n");
        exit(1);
    }
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if(getaddrinfo(argv[1],PORT,&hints,&servinfo) != 0)
    {
        perror("getaddrinfo error");
        exit(1);
    }
    for(p=servinfo; p!=NULL; p=p->ai_next)
    {
        if((sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1)
        {
            perror("socket error");
            continue;
        }
        if(connect(sockfd,p->ai_addr,p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("connect error");
            continue;
        }
        break;
    }
    if(p == NULL)
    {
        fprintf(stderr,"client failed to connect\n");
        exit(1);
    }
    inet_ntop(p->ai_family,get_addr_info(p->ai_addr),s,sizeof(s));
    fprintf(stdout,"client connect to %s\n",s);
    if((nbytes = recv(sockfd,buf,sizeof(buf),0)) == -1)
    {
        perror("recv error");
        exit(1);
    }
    buf[nbytes] = '\0';
    fprintf(stdout,"client receive: %s\n",buf);
    close(sockfd);
return 0;
}
