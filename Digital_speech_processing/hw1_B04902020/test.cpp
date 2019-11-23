#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include "hmm.h"

#define MODEL_NUM 5
#define SAMPLE_LEN 50
using namespace std;
HMM model[MODEL_NUM];
char listname[32];
char test_file[32];
char result_file[32];
double delta[MODEL_NUM][MAX_SEQ][MAX_STATE];

int main(int argc, char *argv[])
{
	if(argc != 4)
	{
		printf("argc fail\n");
		exit(0);
	}
	strcpy(listname, argv[1]);
	strcpy(test_file, argv[2]);
	strcpy(result_file, argv[3]);
	load_models(listname, model, MODEL_NUM);			//model_0x.txt和test.cpp要在同一層

	FILE *fp, *fp_result;
	fp = fopen(test_file, "r");
	fp_result = fopen(result_file, "w");
	char ob_str[MAX_SEQ];
	while(fscanf(fp, "%s", ob_str) == 1 && strcmp(ob_str, "") != 0)
	{
		memset(delta, 0, sizeof(delta));
		for(int m=0; m<MODEL_NUM; m++)
		{
			for(int i=0; i<model[m].state_num; i++)
				delta[m][0][i] = model[m].initial[i]*model[m].observation[ob_str[0]-'A'][i];
		}
		for(int m=0; m<MODEL_NUM; m++)
		{
			for(int t=1; t<strlen(ob_str); t++)
			{
				for(int i=0; i<model[m].state_num; i++)
				{
					double max_delta = 0;
					for(int j=0; j<model[m].state_num; j++)
					{
						double tmp = delta[m][t-1][j]*model[m].transition[j][i]*model[m].observation[ob_str[t]-'A'][i];
						max_delta = (tmp > max_delta) ? tmp : max_delta;
					}
					delta[m][t][i] = max_delta;
				}
			}
		}
		char ans[32];
		double p = 0;
		for(int m=0; m<MODEL_NUM; m++)
		{
			for(int i=0; i<model[m].state_num; i++)
			{
				// printf("delta[%d][%d][%d] = %G\n", m, SAMPLE_LEN-1, i, delta[m][SAMPLE_LEN-1][i]);
				if(delta[m][SAMPLE_LEN-1][i] > p)
				{
					p = delta[m][SAMPLE_LEN-1][i];
					strcpy(ans, model[m].model_name);
				}
			}
		}
		// printf("ans = %s p = %G\n", ans, p);
		fprintf(fp_result, "%s\n", ans);
	}
	fclose(fp);
	// dump_models(model, MODEL_NUM);
	return 0;
}
