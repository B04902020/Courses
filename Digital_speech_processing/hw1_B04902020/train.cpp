#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include "hmm.h"
using namespace std;

#define SAMPLE_NUM 10000
#define SAMPLE_LEN 50
int iterations;
char initail_file[32];
char training_file[32];
char model_file[32];
HMM model;
double alpha[MAX_SEQ][MAX_STATE];
double beta[MAX_SEQ][MAX_STATE];
double Gamma[MAX_SEQ][MAX_STATE];
double epsilon[MAX_SEQ][MAX_STATE][MAX_STATE];
double fix_observ[6][6];

void forward(char *ob_str)			//compute alpha
{
	for(int i=0; i<model.state_num; i++)		//初始化alpha[0][state_id]
		alpha[0][i] = model.initial[i]*model.observation[ob_str[0]-'A'][i];

	for(int i=1; i<strlen(ob_str); i++)
	{
		for(int j=0; j<model.state_num; j++)
		{
			for(int k=0; k<model.state_num; k++)
				alpha[i][j] += alpha[i-1][k]*model.transition[k][j];
			
			alpha[i][j] *= model.observation[ob_str[i]-'A'][j];
		}
	}
}
void backward(char *ob_str)			//compute beta
{
	for(int i=0; i<model.state_num; i++)		//初始化beta[T-1][state_id]
		beta[strlen(ob_str)-1][i] = 1;
	
	for(int i=strlen(ob_str)-2; i>=0; i--)
	{
		for(int j=0; j<model.state_num; j++)
		{	
			for(int k=0; k<model.state_num; k++)
				beta[i][j] += beta[i+1][k]*model.transition[j][k]*model.observation[ob_str[i+1]-'A'][k];
		}
	}
}
void compute_gamma(char *ob_str)
{
	for(int t=0; t<strlen(ob_str); t++)
	{
		double down = 0;
		for(int i=0; i<model.state_num; i++)
			down += alpha[t][i]*beta[t][i];
		for(int i=0; i<model.state_num; i++)
		{
			double up = alpha[t][i]*beta[t][i];
			Gamma[t][i] += (up / down);
			fix_observ[ob_str[t]-'A'][i] += (up / down);
		}
	}
}
void compute_epsilon(char *ob_str)
{
	for(int t=0; t<strlen(ob_str)-1; t++)
	{
		double down = 0;
		for(int i=0; i<model.state_num; i++)
		{
			for(int j=0; j<model.state_num; j++)
				down += alpha[t][i]*model.transition[i][j]*model.observation[ob_str[t+1]-'A'][j]*beta[t+1][j];
		}
		for(int i=0; i<model.state_num; i++)
		{
			for(int j=0; j<model.state_num; j++)
			{
				double up = alpha[t][i]*model.transition[i][j]*model.observation[ob_str[t+1]-'A'][j]*beta[t+1][j];
				epsilon[t][i][j] += (up / down);
			}
		}
	}
}
void fix_model()
{
	for(int i=0; i<model.state_num; i++)
		model.initial[i] = (Gamma[0][i] / SAMPLE_NUM);

	for(int i=0; i<model.state_num; i++)
	{
		for(int j=0; j<model.state_num; j++)
		{
			double up = 0, down = 0;
			for(int t=0; t<SAMPLE_LEN-1; t++)
			{
				
				up += epsilon[t][i][j];
				down += Gamma[t][i];
			}
			model.transition[i][j] = (up / down);
			// printf("%G / %G = %G\n",up,down,up/down);
		}
	}
	for(int i=0; i<model.observ_num; i++)
	{
		for(int j=0; j<model.state_num; j++)
		{
			double up = 0, down = 0;
			for(int t=0; t<SAMPLE_LEN; t++)
				down += Gamma[t][j];
			up = fix_observ[i][j];
			model.observation[i][j] = (up / down);
		}
	}
}
int main(int argc, char *argv[])
{
	if(argc != 5)
 	{
 		printf("argc fail\n");
 		exit(0);
 	}
 	sscanf(argv[1],"%d",&iterations);
 	strcpy(initail_file, argv[2]);
 	strcpy(training_file, argv[3]);
 	strcpy(model_file, argv[4]);
	loadHMM(&model, initail_file);
	
	for(int r=0; r<iterations; r++)						//run iteration times
	{
		FILE *fp;
		char ob_str[MAX_SEQ];
		
		memset(Gamma, 0, sizeof(Gamma));
		memset(epsilon, 0, sizeof(epsilon));
		memset(fix_observ, 0, sizeof(fix_observ));
		
		fp = fopen(training_file, "r");
		while(fscanf(fp, "%s", ob_str) == 1 && strcmp(ob_str, "") != 0)
		{
			memset(alpha, 0, sizeof(alpha));
			memset(beta, 0, sizeof(beta));
			
			forward(ob_str);
			backward(ob_str);
			compute_gamma(ob_str);
			compute_epsilon(ob_str);
		}
		fix_model();
		fclose(fp);
	}
	FILE *model_fp;
	model_fp = fopen(model_file, "w");
	dumpHMM(model_fp, &model);
	fclose(model_fp);

    return 0;
}
