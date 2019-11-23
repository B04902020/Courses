#include <cstdio>
#include <vector>
#include <algorithm>
#include <cstring>
using namespace std;
#define Dim 5
#define ROUND 1126

struct data
{
	double input[Dim];
	int output;
};

int step = 0, n;					//step紀錄幾輪修正, n為train data數量
char file[] = "./train.txt";		//檔案名稱
vector<struct data> train_data;
double weight[Dim];

void read_data(char *file)
{
	FILE *fp = fopen(file, "r");
	struct data temp;
	temp.input[0] = 1;
	while(fscanf(fp, "%lf %lf %lf %lf %d", &temp.input[1], &temp.input[2], &temp.input[3], &temp.input[4], &temp.output) != EOF)
		train_data.push_back(temp);
	n = train_data.size();
}

int sign(double x)
{
	int ans = (x <= 0) ? -1 : 1;
	return ans; 
}

double multiply(double *w, double *x, int dim)
{
	double ans = 0;
	for(int i=0; i<dim; i++)
		ans += w[i]*x[i];
	return ans;
}

void fix(double *w, double *x, int y, int dim)
{
	for(int i=0; i<dim; i++)
		w[i] = (w[i] + x[i]*y);
}

void PLA()
{
	int correctnum = 0, cur = 0, finish = 0;		//correctnum代表有幾個點已正確, cur代表目前要驗證的點, finish為結束flag
	while(!finish)
	{
		if(sign(multiply(train_data[cur].input, weight, Dim)) == train_data[cur].output)
			correctnum++;
		else
		{
			fix(weight, train_data[cur].input, train_data[cur].output, Dim);
			step += 1;
			correctnum = 0;
		}
		cur = (cur+1) % n;
		if(correctnum == n)
			finish = 1;
	}
}

int main()
{
	int mean = 0;
	read_data(file);
	for(int i=0; i<ROUND; i++)
	{
		random_shuffle(train_data.begin(), train_data.end());
		PLA();
		printf("%d\n", step);
		mean += step;
		step = 0;
		memset(weight, 0, sizeof(weight));
	}
	mean /= ROUND;					
	printf("%d\n", mean);
	return 0;
}