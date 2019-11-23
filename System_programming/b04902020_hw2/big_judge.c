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
char buf[5000][16];
int cnt=0;
int player_score[21]={0};
void combination(int n)
{
  for(int i=1; i<=n-3; i++)
    for(int j=i+1; j<=n-2; j++)
       for(int k=j+1; k<=n-1; k++)
          for(int l=k+1; l<=n; l++)
          {
            sprintf(buf[cnt],"%d %d %d %d\n",i,j,k,l);
            cnt++;
          }
}
int main(int argc,char *argv[])
{
  if(argc != 3)
  	perror("argc error");
  int judge_num,player_num;
  sscanf(argv[1],"%d",&judge_num);
  sscanf(argv[2],"%d",&player_num);
  // printf("judge_num=%d player_num=%d\n",judge_num,player_num);
  combination(player_num);
  // for(int i=0; i<cnt; i++)
  //   printf("%s",buf[i]);
  int pipe_to_judge[judge_num+1][2],pipe_to_big_judge[judge_num+1][2];
  for(int i=1; i<=judge_num; i++)
  {
  	  pipe(pipe_to_judge[i]);
      pipe(pipe_to_big_judge[i]);
  }
  for(int i=1; i<=judge_num; i++)
  {
    pid_t pid;
    pid = fork();
    if(pid == 0)
    {
      char buff[128];
      sprintf(buff,"%d",i);
      close(pipe_to_judge[i][1]);
      dup2(pipe_to_judge[i][0],0);
      close(pipe_to_judge[i][0]);
      close(pipe_to_big_judge[i][0]);
      dup2(pipe_to_big_judge[i][1],1);
      close(pipe_to_big_judge[i][1]);
      execl("./judge","./judge",buff,NULL);
    }
    else
    {
    	close(pipe_to_judge[i][0]);
    	close(pipe_to_big_judge[i][1]);
    }
  }
  struct timeval tv;
  tv.tv_sec = 3;
  tv.tv_usec = 0;
  fd_set readfds;
  int read_cnt=0,finish=0,write_cnt=0;
  for(int i=1; i<=judge_num; i++) // first assign
  {
     write(pipe_to_judge[i][1],buf[write_cnt],strlen(buf[write_cnt])+1); //buf存player的組合
     write_cnt++;
     if(write_cnt == cnt)
        break;
  }
  while(!finish)
  {
    for(int i=1; i<=judge_num; i++)
      FD_SET(pipe_to_big_judge[i][0],&readfds);
    select(pipe_to_big_judge[judge_num][0]+1,&readfds,NULL,NULL,&tv);
    for(int i=1; i<=judge_num; i++)
    {
      if(FD_ISSET(pipe_to_big_judge[i][0],&readfds))
      {
        int player1[2],player2[2],player3[2],player4[2]; // 0->player編號 1->名次
        char ret[128];
        read(pipe_to_big_judge[i][0],ret,128);
        //fprintf(stderr,"big_read=%s\n",ret);
        sscanf(ret,"%d%d%d%d%d%d%d%d",&player1[0],&player1[1],&player2[0],&player2[1]
          ,&player3[0],&player3[1],&player4[0],&player4[1]);
        player_score[player1[0]] += (4 - player1[1]);
        player_score[player2[0]] += (4 - player2[1]);
        player_score[player3[0]] += (4 - player3[1]);
        player_score[player4[0]] += (4 - player4[1]);
        read_cnt++;
        if(read_cnt == cnt)
        {
           finish = 1;
           break;
        }
        else if(read_cnt != cnt && write_cnt == cnt)
           continue; 
        else     //assign new player
        {
          write(pipe_to_judge[i][1],buf[write_cnt],strlen(buf[write_cnt])+1);
          write_cnt++;
        }
      }
    }
  }
  for(int i=1; i<=judge_num; i++)
     write(pipe_to_judge[i][1],"-1 -1 -1 -1\n",13);
  for(int i=1; i<=judge_num; i++)
     wait(NULL);
  //output the rank
  int index[20];
  for(int i=0; i<player_num; i++)
    index[i] = (i+1);
  for(int i=0; i<player_num; i++)
    for(int j=i+1; j<player_num; j++)
    {
      if(player_score[index[i]] < player_score[index[j]])
      {
        int temp = index[i];
        index[i] = index[j];
        index[j] = temp;
      }
    }
  for(int i=0; i<player_num; i++)
     printf("%d %d\n",index[i],player_score[index[i]]);
exit(0);	
}
