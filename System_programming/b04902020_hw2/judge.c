/*b04902020 張耀中*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>  
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <ctype.h>
char str[5][128]={"./judge_A.FIFO","./judge_B.FIFO","./judge_C.FIFO","./judge_D.FIFO","./judge.FIFO"};
char player_index[4][8]={"A","B","C","D"};
char random_key[4][8]={"10","15","20","25"};
int score[4]={0},delay[4]={0};  
void insert(char argv[])
{
  int l1 = strlen(str[0]),l2 = strlen(argv),l3 = strlen(str[4]);
  if(l2 == 1)
  {
  	for(int j=0; j<4; j++)
    {
       for(int i=l1+1; i>=7; i--)
           str[j][i] = str[j][i-1];
       str[j][7] = argv[0];
    } 
    for(int i=l3+1; i>=7; i--)
       str[4][i] = str[4][i-1];
    str[4][7] = argv[0];	
  }
  else if(l2 == 2)
  {
  	for(int j=0; j<4; j++)
  	{
  		for(int i=l1+2; i>=7; i--)
  			str[j][i] = str[j][i-2];
  	  str[j][7] = argv[0];
  	  str[j][8] = argv[1];
  	}
  	for(int i=l3+2; i>=7; i--)
  		str[4][i] = str[4][i-2];
  	str[4][7] = argv[0];
  	str[4][8] = argv[1];
  }
}
void compute_score(int show_on[],int temp_score[])
{
  for(int i=0; i<4; i++)
        if(show_on[i] == 0)
           delay[i] = 1;
    for(int i=0; i<4; i++)
    {
        int overlap = 0;
        for(int j=0; j<4; j++)
        {
           if(temp_score[i] == temp_score[j] && i != j)
              overlap = 1;
        }
        if(!overlap && delay[i] == 0)
           score[i] += temp_score[i];
    }
}
int main(int argc,char *argv[])
{
  if(argc != 2)
  	 perror("argc error");
  insert(argv[1]);
  while(1)
  {
    char buff[128]={'\0'};
  	read(0,buff,100);
    //fprintf(stderr,"judge read=%s\n",buff);
    if(strncmp(buff,"-1 -1 -1 -1\n",2) == 0)
    	 exit(0);
    memset(score,0,sizeof(score));
    memset(delay,0,sizeof(delay));
    for(int i=0; i<5; i++)
       mkfifo(str[i],0777);
    char player[4][4]; //player big_judge assigned
    sscanf(buff,"%s%s%s%s",player[0],player[1],player[2],player[3]);
    
    int temp_score[4]={0},show_on[4]={0};
    int fd_judge;
    struct timeval tv;
    tv.tv_sec = 3; tv.tv_usec = 0; 
    fd_set readfd;
    for(int i=0; i<4; i++)
    {
       pid_t pid;
       pid = fork();
       if(pid == 0)  //child
       {
       	  if(execl("./player","./player",argv[1],player_index[i],random_key[i],NULL) < 0)
       	  	 perror("execl error");
       } 
       else  //parent
       {
          fd_judge = open(str[4],O_RDONLY);
          FD_ZERO(&readfd);
          FD_SET(fd_judge,&readfd);
          select(fd_judge+1,&readfd,NULL,NULL,&tv);
          if(FD_ISSET(fd_judge,&readfd))
          {
            char buffer[128]={'\0'};
            read(fd_judge,buffer,16); 
            int digit=0,alpha=-1,l=strlen(buffer);
            for(int k=0; k<l; k++)
            {
             if(isalpha(buffer[k]))
                alpha = (buffer[k]-'A'),show_on[buffer[k]-'A'] = 1;
             if(isdigit(buffer[k]) && isdigit(buffer[k-1]) == 0)
                digit++;
             if(digit == 2)  
             {
                temp_score[alpha] = (delay[alpha] == 0) ? (buffer[k]-'0') : 0; 
                break;
             }
            }
          }
       }
    }
    compute_score(show_on,temp_score);  //compute first round score
    //fprintf(stderr,"%d %d %d %d\n",score[0],score[1],score[2],score[3]);
    int fd[4];                        //open player judge
    for(int i=0; i<4; i++) 
       fd[i] = open(str[i],O_WRONLY);
    
    for(int r=1; r<=19; r++)
    {
      int last_result[4]={0};
      memset(show_on,0,sizeof(show_on));
      for(int p=0; p<4; p++)
      {
        char ret_player[16];
        sprintf(ret_player,"%d %d %d %d",temp_score[0],temp_score[1],temp_score[2],temp_score[3]);
        write(fd[p],ret_player,strlen(ret_player)+1);
        FD_ZERO(&readfd);
        FD_SET(fd_judge,&readfd);
        select(fd_judge+1,&readfd,NULL,NULL,&tv);
        if(FD_ISSET(fd_judge,&readfd))
        {
            char buffer[128]={'\0'};
            read(fd_judge,buffer,16); 
            int digit=0,alpha=-1,l=strlen(buffer);
            for(int k=0; k<l; k++)
            {
             if(isalpha(buffer[k]))
                alpha = (buffer[k]-'A'),show_on[buffer[k]-'A'] = 1;
             if(isdigit(buffer[k]) && isdigit(buffer[k-1]) == 0)
                digit++;
             if(digit == 2)  
             {
                last_result[alpha] = (delay[alpha] == 0) ? (buffer[k]-'0') : 0; 
                break;
             }
            }
        }
      }
      compute_score(show_on,last_result); 
      //fprintf(stderr,"%d %d %d %d\n",score[0],score[1],score[2],score[3]);
      for(int i=0; i<4; i++)
         temp_score[i] = last_result[i];
    }
    int rank[4]={0};
    for(int i=0; i<4; i++)
    {
       int place=0;
       for(int j=0; j<4; j++)
       {
         if(score[i] <= score[j])
            place++;
       }
       rank[i] = place;
    }
    char ret_big_judge[128];
    sprintf(ret_big_judge,"%s %d\n%s %d\n%s %d\n%s %d\n",player[0],rank[0],
      player[1],rank[1],player[2],rank[2],player[3],rank[3]);
    //fprintf(stderr,"judge_write=%s\n",ret_big_judge);
    write(1,ret_big_judge,strlen(ret_big_judge)+1);
    for(int i=0; i<5; i++)
        unlink(str[i]);
    for(int i=0; i<4; i++)
       wait(NULL);
  }
exit(0);	
}