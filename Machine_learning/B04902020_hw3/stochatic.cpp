#include <cstdio>
#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
using namespace std;
 
#define DIM 20
#define Iter 2000
#define Rate 0.001

struct data
{  
	double x[DIM+1];
	int y;
};
typedef struct data Data;
vector<Data> train_data;
vector<Data> test_data;

int sign(double x)
{
	if(x > 0)
		return 1;
	return -1;
}
 
void read_data(fstream &fp, vector<Data> &input)
{
	while(!fp.eof()){
		Data temp;
		temp.x[0] = 1;
		for(int i = 1; i <= DIM; i++)
			fp >> temp.x[i];
		fp >> temp.y;
		input.push_back(temp);
	}
	fp.close();
}
 
double sigmoid(double x)
{  
	return 1.0 / (1.0 + exp(-x));
}
 
double Multi(double *x,double *y,int dim)
{
	double temp = 0.0;
	for(int i = 0; i <dim; i++)
		temp += x[i] * y[i];
	return temp;
}
 
void Gradient_des(vector<Data> &input,double *weight, double *grad)
{
	for(int i = 0; i < input.size(); i++){
		double temp = sigmoid(-1 * Multi(weight, input[i].x, DIM+1) * (double)input[i].y);
		for(int j = 0; j <= DIM; j++)
			grad[j] += -1.0 * temp * input[i].x[j] * input[i].y; 
	}
	for(int i = 0; i <= DIM; i++)
		grad[i] = grad[i] / input.size();
}
 
void Stochastic_des(Data node, double *weight, double *grad)
{
	double temp = sigmoid(-1 * Multi(weight, node.x, DIM+1) * (double)node.y);
	for(int j = 0; j <= DIM; j++)
		grad[j] += -1.0 * temp * node.x[j] * node.y;
 
}

double Error(vector<Data> &input, double *weight)
{
	double err = 0.0;
	for(int i = 0; i < input.size(); i++){
		if(sign(Multi(input[i].x, weight, DIM+1)) != input[i].y)
			err++;
	}
	return err / input.size();
}

void Logistic_Reg(vector<Data> &input, double *weight, double rate, int iter)
{
    for(int i = 0; i < iter; i++)
    {
		double grad[DIM+1] = {0.0};
		// Gradient_des(input, weight, grad);
		Stochastic_des(input[i%input.size()], weight, grad);
		for(int j = 0; j <= DIM; j++)
			weight[j] = weight[j] - (rate * grad[j]);
		cout << Error(train_data, weight) << endl;
	}

}
 
int main(){

	fstream fp1("train.txt");
	fstream fp2("test.txt");
	read_data(fp1,train_data);
	read_data(fp2,test_data);

	double weight[DIM+1];
	for(int i = 0; i <= DIM; i++)
		weight[i] = 0;
	Logistic_Reg(train_data,weight, Rate, Iter);
	// cout << Error(train_data, weight) << endl;
 // 	cout << Error(test_data, weight) << endl;

    return 0;
}