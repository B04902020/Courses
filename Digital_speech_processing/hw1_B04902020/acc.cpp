#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

double up = 0, down = 0;
char result_file[32], ans_file[32];
int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		printf("argc fail\n");
		exit(0);
	}
	strcpy(result_file, argv[1]);
	strcpy(ans_file, argv[2]);
	
	FILE *result_fp, *ans_fp;
	result_fp = fopen(result_file,"r");
	ans_fp = fopen(ans_file,"r");
	char result_str[32], ans_str[32];
	while(fscanf(result_fp, "%s", result_str) == 1 && fscanf(ans_fp, "%s", ans_str) == 1)
	{
		if(!strcmp(result_str,""))
			break;
		down += 1;
		if(!strcmp(result_str,ans_str))
			up += 1;
	}
	printf("%G\n",up/down);
return 0;
}