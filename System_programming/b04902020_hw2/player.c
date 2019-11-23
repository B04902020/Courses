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
int main(int argc,char *argv[])
{
  if(argc != 4)
  	perror("argc error");
  srand(clock());
  insert(argv[1]);
  char player_index[4],random_key[32],number_choose[4],temp[128];
  sscanf(argv[2],"%s",player_index);
  sscanf(argv[3],"%s",random_key);
  int num = (2*(rand()%3)+1);
  //fprintf(stderr,"%s select %d\n",player_index,num);
  sprintf(temp,"%s %s %d\n",player_index,random_key,num);
  int fd,fd_judge;
  fd_judge = open(str[4],O_WRONLY);
  write(fd_judge,temp,strlen(temp)+1);
  if(strcmp(player_index,"A") == 0)  //first write
  	fd = open(str[0],O_RDONLY);
  else if(strcmp(player_index,"B") == 0)
  	fd = open(str[1],O_RDONLY);
  else if(strcmp(player_index,"C") == 0)
  	fd = open(str[2],O_RDONLY);
  else if(strcmp(player_index,"D") == 0)
  	fd = open(str[3],O_RDONLY);
  for(int i=1; i<=19; i++)
  {
  	char buf[16],A[4],B[4],C[4],D[4];
  	read(fd,buf,10);
  	sscanf(buf,"%s%s%s%s",A,B,C,D);
  	int num = (2*(rand()%3) + 1);
  	//fprintf(stderr,"%s select %d\n",player_index,num);
  	memset(temp,'\0',sizeof(temp));
    sprintf(temp,"%s %s %d\n",player_index,random_key,num);
  	write(fd_judge,temp,strlen(temp)+1); //不確定是否要丟\0
  }

exit(0);	
}