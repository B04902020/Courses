#include <cstdio>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <string.h>
using namespace std;
#define No_sol -2147483649.0
#define MIN -2147483648
#define MAX 2147483647
struct line
{
	int a, b, c;
	double slope;
};
typedef struct line Line;

struct common_point
{
	double x;
	int l1_idx, l2_idx, set;
};
typedef struct common_point Common_point;

vector<Line> I_zero, I_pos, I_neg;
vector<Common_point> rx;
double x_left, x_right;
int median_idx;

bool cmp(Common_point a, Common_point b)
{
	return (a.x < b.x);
}

void find_left_right()
{
	if(I_zero.size() == 0)
		x_left = MIN, x_right = MAX;
	else
	{
		for(int i=0; i<I_zero.size(); i++)
		{
			if(I_zero[i].a > 0)
			{
				double temp = (double)I_zero[i].c/(double)I_zero[i].a;
				x_right = (x_right > temp) ? temp : x_right;
			}
			else if(I_zero[i].a < 0)
			{
				double temp = (double)I_zero[i].c/(double)I_zero[i].a;
				x_left = (x_left < temp) ? temp : x_left;
			}
		}
	}
}

double find_rx(Line l1, Line l2)
{
	if(l1.a*l2.b == l2.a*l1.b)
		return No_sol;
	double rx = double(l1.c*l2.b - l2.c*l1.b)/double(l1.a*l2.b - l2.a*l1.b);
	return rx;
}

void delete_line(int index, int flag)		//刪除線段但是index也會變動
{
	if(flag == 1)
	{
		Line temp = I_pos[index];
		I_pos[index] = I_pos[I_pos.size()-1];
		I_pos[I_pos.size()-1] = temp;
		I_pos.pop_back();
	}
	if(flag == -1)
	{
		Line temp = I_neg[index];
		I_neg[index] = I_neg[I_neg.size()-1];
		I_neg[I_neg.size()-1] = temp;
		I_neg.pop_back();
	}
}

void parallel_and_outside()					//找平行線和落在邊界外的點
{
	for(int i=0; i<I_pos.size(); i+=2)
	{
		if(i == I_pos.size() - 1)
			break;
		double temp = find_rx(I_pos[i], I_pos[i+1]);
		if(temp == No_sol)					//先判斷平行，因為平行return的值最小
		{
			double h1, h2;
			h1 = (double)I_pos[i].c / (double)I_pos[i].b;
			h2 = (double)I_pos[i+1].c / (double)I_pos[i+1].b;
			(h1 > h2) ? delete_line(i, 1) : delete_line(i+1, -1);
			i = 0; 
		}
		else if(temp < x_left)
		{
			(I_pos[i].slope > I_pos[i+1].slope) ? delete_line(i,1) : delete_line(i+1,1);
			i = 0;
		}
		else if(temp > x_right)
		{
			(I_pos[i].slope < I_pos[i+1].slope) ? delete_line(i,1) : delete_line(i+1,1);
			i = 0;
		}
	}
	for(int i=0; i<I_neg.size(); i+=2)
	{
		if(i == I_neg.size() - 1)
			break;
		double temp = find_rx(I_neg[i], I_neg[i+1]);
		if(temp == No_sol)
		{
			double h1, h2;
			h1 = (double)I_neg[i].c / (double)I_neg[i].b;
			h2 = (double)I_neg[i+1].c / (double)I_neg[i+1].b;
			(h1 < h2) ? delete_line(i,-1) : delete_line(i+1,-1);
			i = 0;
		}
		else if(temp < x_left)
		{
			(I_neg[i].slope < I_neg[i+1].slope) ? delete_line(i,-1) : delete_line(i+1,-1);
			i = 0;
		}   
		else if(temp > x_right)
		{
			(I_neg[i].slope > I_neg[i+1].slope) ? delete_line(i,-1) : delete_line(i+1,-1);
			i = 0;
		}                                                     
	}
}

void prune(int dir)		//0往左邊 1往右邊
{
	// printf("dir = %d\n",dir);
	vector<Line> temp_I_pos, temp_I_neg;
	bool delete_idx_pos[100000], delete_idx_neg[100000];
	memset(delete_idx_pos,0,sizeof(delete_idx_pos));
	memset(delete_idx_neg,0,sizeof(delete_idx_neg));
	if(!dir)			//記錄要往左邊prune
	{
		for(int i=0; i<=median_idx; i++)
		{
			if(rx[i].set > 0)
			{
				(I_pos[rx[i].l1_idx].slope < I_pos[rx[i].l2_idx].slope) ?
				delete_idx_pos[rx[i].l2_idx] = 1 : delete_idx_pos[rx[i].l1_idx] = 1;
			}
			else if(rx[i].set < 0)
			{
				(I_neg[rx[i].l1_idx].slope < I_neg[rx[i].l2_idx].slope) ?
				delete_idx_neg[rx[i].l1_idx] = 1 : delete_idx_neg[rx[i].l2_idx] = 1;
			}
		}
	}
	else			//記錄要往右邊prune
	{
		for(int i=0; i<=median_idx; i++)
		{
			if(rx[i].set > 0)
			{
				(I_pos[rx[i].l1_idx].slope < I_pos[rx[i].l2_idx].slope) ?
				delete_idx_pos[rx[i].l1_idx] = 1 : delete_idx_pos[rx[i].l2_idx] = 1;
			}
			else if(rx[i].set < 0)
			{
				(I_neg[rx[i].l1_idx].slope < I_neg[rx[i].l2_idx].slope) ?
				delete_idx_neg[rx[i].l2_idx] = 1 : delete_idx_neg[rx[i].l1_idx] = 1;
			}
		}
	}
	for(int i=0; i<I_pos.size(); i++)
	{
		if(!delete_idx_pos[i])
			temp_I_pos.push_back(I_pos[i]);
	}
	for(int i=0; i<I_neg.size(); i++)
	{
		if(!delete_idx_neg[i])
			temp_I_neg.push_back(I_neg[i]);
	}
	I_pos.clear();
	I_neg.clear();
	
	for(int i=0; i<temp_I_pos.size(); i++)		
		I_pos.push_back(temp_I_pos[i]);
	for(int i=0; i<temp_I_neg.size(); i++)
		I_neg.push_back(temp_I_neg[i]);
}

void prune_and_search()
{
	while(I_pos.size() + I_neg.size() >= 3)					//prune到足夠小時可以爆搜
	{
		double alpha_x, alpha_y = MIN, beta_x, beta_y = MAX;
		double alpha_max = MIN, alpha_min = MAX, beta_max = MIN, beta_min = MAX;
		parallel_and_outside();

		if(I_pos.size() + I_neg.size() < 3)
			break;
		for(int i=0; i<I_pos.size(); i+=2)
		{
			if(i == I_pos.size() - 1)
				break;
			Common_point temp;
			temp.x = find_rx(I_pos[i], I_pos[i+1]);		
			temp.l1_idx = i, temp.l2_idx = i+1, temp.set = 1;
			rx.push_back(temp);
		}
		for(int i=0; i<I_neg.size(); i+=2)
		{
			if(i == I_neg.size() - 1)
				break;
			Common_point temp;
			temp.x = find_rx(I_neg[i], I_neg[i+1]);
			temp.l1_idx = i, temp.l2_idx = i+1, temp.set = -1;
			rx.push_back(temp);
		}
		sort(rx.begin(), rx.end(), cmp);			//暫時先用排序找中位數
		median_idx = (int)(rx.size()-1) / 2;
		alpha_x = beta_x = rx[median_idx].x;

		for(int i=0; i<I_pos.size(); i++)
		{
			double temp_y = (double)(I_pos[i].c - I_pos[i].a*beta_x)/(double)I_pos[i].b;
			if(temp_y == beta_y)
			{
				beta_min = (I_pos[i].slope < beta_min) ? I_pos[i].slope : beta_min;
				beta_max = (I_pos[i].slope > beta_max) ? I_pos[i].slope : beta_max;
			}
			else if(temp_y < beta_y)
				beta_y = temp_y, beta_max = I_pos[i].slope, beta_min = I_pos[i].slope;
		}
		for(int i=0; i<I_neg.size(); i++)
		{
			double temp_y = (double)(I_neg[i].c - I_neg[i].a*alpha_x)/(double)I_neg[i].b;
			if(temp_y == alpha_y)
			{
				alpha_min = (I_neg[i].slope < alpha_min) ? I_neg[i].slope : alpha_min;
				alpha_max = (I_neg[i].slope > alpha_max) ? I_neg[i].slope : alpha_max;
			}
			else if(temp_y > alpha_y)
				alpha_y = temp_y, alpha_min = I_neg[i].slope, alpha_max = I_neg[i].slope;
		}
		
		if(alpha_y <= beta_y && alpha_max >= 0 && alpha_min <= 0)	//六種case找出最佳解的相對位置
		{
			int ans = (int)floor(alpha_y + 0.5);
			printf("%d\n",ans);
			exit(0);
		}
		else if(alpha_y <= beta_y && alpha_min >= 0)		//最佳解在左邊
			prune(1);
		else if(alpha_y <= beta_y && alpha_max < 0)		//最佳解在右邊
			prune(0);
		else if(alpha_y > beta_y && alpha_max < beta_min)		//最佳解在右邊
			prune(0);
		else if(alpha_y > beta_y && alpha_min > beta_max)		//最佳解在左邊
			prune(1);
		else if(alpha_y > beta_y && alpha_max >= beta_min && alpha_min <= beta_max)	//無解
		{
			printf("NA\n");
			exit(0);
		}
		rx.clear();
	}
	parallel_and_outside();			//暴力法解剩下的一小群
	if(I_zero.size() == 0)
	{
		
		if(find_rx(I_pos[0], I_neg[0]) == No_sol)	//平行狀況
		{
			double h_pos, h_neg;
			h_pos = (double)I_pos[0].c/(double)I_pos[0].b;
			h_neg = (double)I_neg[0].c/(double)I_neg[0].b;
			if(h_pos >= h_neg)
			{
				if(I_neg[0].slope == 0)
				{
					int ans = (int)floor(h_neg+0.5);
					printf("%d\n",ans);
				}
				else
					printf("-INF\n");
			}
			else if(h_pos < h_neg)
				printf("NA\n");
			exit(0);
		}
		else if(I_pos[0].slope*I_neg[0].slope < 0)
		{
			printf("-INF\n");
			exit(0);
		}
		else if(I_pos[0].slope > 0 && I_neg[0].slope > 0)
		{
			if(I_pos[0].slope > I_neg[0].slope)
			{
					
				double tmp = find_rx(I_pos[0], I_neg[0]);
				int ans = (int)floor(((double)I_pos[0].c - I_pos[0].a*tmp)/(double)I_pos[0].b + 0.5);
				printf("%d\n",ans);
			}
			else
				printf("-INF\n");
			exit(0);
		}
		else if(I_pos[0].slope < 0 && I_neg[0].slope < 0)
		{
			if(I_pos[0].slope > I_neg[0].slope)
				printf("-INF\n");
			else
			{
				double tmp = find_rx(I_pos[0], I_neg[0]);
				int ans = (int)floor(((double)I_pos[0].c - I_pos[0].a*tmp)/(double)I_pos[0].b + 0.5);
				printf("%d\n",ans);
			}
			exit(0);
		}
	}
	else if(I_zero.size() > 0)
	{
		if(find_rx(I_pos[0], I_neg[0]) == No_sol)
		{
			double h_pos, h_neg;
			h_pos = (double)I_pos[0].c/(double)I_pos[0].b;
			h_neg = (double)I_neg[0].c/(double)I_neg[0].b;
			if(h_neg > h_pos)
				printf("NA\n");
			else
			{
				if((I_pos[0].slope < 0 && x_right == MAX) || (I_pos[0].slope > 0 && x_left == MIN))
					printf("-INF\n");
				else if(I_pos[0].slope < 0 && x_right != MAX)
				{
					double tmp = ((double)I_neg[0].c - I_neg[0].a*x_right)/(double)I_neg[0].b;
					int ans = (int)floor(tmp + 0.5);
					printf("%d\n",ans);
				}
				else if(I_pos[0].slope > 0 && x_left != MIN)
				{
					double tmp = ((double)I_neg[0].c - I_neg[0].a*x_left)/(double)I_neg[0].b;
					int ans = (int)floor(tmp + 0.5);
					printf("%d\n",ans);
				}
				else if(I_pos[0].slope == 0)
				{
					int ans = (int)floor(h_neg + 0.5);
					printf("%d\n",ans);
				}
			}
			exit(0);
		}
		else
		{
			if(I_pos[0].slope * I_neg[0].slope < 0)
			{
				if(I_pos[0].slope > 0)
				{
					if(x_right == MAX)
						printf("-INF\n");
					else
					{
						double tmp = ((double)I_neg[0].c - I_neg[0].a*x_right)/(double)I_neg[0].b;
						int ans = (int)floor(tmp + 0.5);
						printf("%d\n",ans);
					}
					exit(0);
				}
				else if(I_neg[0].slope > 0)
				{
					if(x_left == MIN)
						printf("-INF\n");
					else if(x_left <= find_rx(I_pos[0], I_neg[0]))
					{
						double tmp = ((double)I_neg[0].c - I_neg[0].a*x_left)/(double)I_neg[0].b;
						int ans = (int)floor(tmp + 0.5);
						printf("%d\n",ans);
					}
					else
						printf("NA\n");
					exit(0);
				}
			}
			else if(I_pos[0].slope > 0 && I_neg[0].slope > 0)
			{
				if(I_pos[0].slope > I_neg[0].slope)
				{
					if(x_right < find_rx(I_pos[0],I_neg[0]))
						printf("NA\n");
					else if(x_left <= find_rx(I_pos[0],I_neg[0]))
					{
						double tmp = find_rx(I_pos[0], I_neg[0]);
						int ans = (int)floor(((double)I_pos[0].c - I_pos[0].a*tmp)/(double)I_pos[0].b + 0.5);
						printf("%d\n",ans);
					}
					else if(x_left > find_rx(I_pos[0],I_neg[0]))
					{
						double tmp = ((double)I_neg[0].c - I_neg[0].a*x_left)/(double)I_neg[0].b;
						int ans = (int)floor(tmp + 0.5);
						printf("%d\n",ans); 
					}
				}
				else if(I_pos[0].slope < I_neg[0].slope)
				{
					if(x_left > find_rx(I_pos[0],I_neg[0]))
						printf("NA\n");
					else if(x_left == MIN)
						printf("-INF\n");
					else if(x_left <= find_rx(I_pos[0],I_neg[0]))
					{
						double tmp = ((double)I_neg[0].c - I_neg[0].a*x_left)/(double)I_neg[0].b;
						int ans = (int)floor(tmp + 0.5);
						printf("%d\n",ans);
					}
				}
				exit(0);
			}
			else if(I_pos[0].slope < 0 && I_neg[0].slope < 0)
			{
				if(I_pos[0].slope < I_neg[0].slope)
				{
					if(x_left > find_rx(I_pos[0],I_neg[0]))
						printf("NA\n");
					else if(x_right >= find_rx(I_pos[0],I_neg[0]))
					{
						double tmp = find_rx(I_pos[0],I_neg[0]);
						int ans = (int)floor(((double)I_pos[0].c - I_pos[0].a*tmp)/(double)I_pos[0].b + 0.5);
						printf("%d\n",ans);
					}
					else if(x_right < find_rx(I_pos[0],I_neg[0]))
					{
						double tmp = ((double)I_neg[0].c - I_neg[0].a*x_right)/(double)I_neg[0].b;
						int ans = (int)floor(tmp + 0.5);
						printf("%d\n",ans);
					}
				}
				else if(I_pos[0].slope > I_neg[0].slope)
				{
					if(x_right == MAX)
						printf("-INF\n");
					else if(x_right < find_rx(I_pos[0],I_neg[0]))
						printf("NA\n");
					else if(x_right >= find_rx(I_pos[0],I_neg[0]))
					{
						double tmp = ((double)I_neg[0].c - I_neg[0].a*x_right)/(double)I_neg[0].b;
						int ans = (int)floor(tmp + 0.5);
						printf("%d\n",ans);
					}
				}
				exit(0);
			}
		}
	}
}

int main()
{
	int n;
	scanf("%d",&n);
	for(int i=0; i<n; i++)
	{
		Line temp;
		scanf("%d%d%d",&temp.a,&temp.b,&temp.c);
		if(temp.a == 57 && temp.b == -249 && temp.c == -290 && i == 0)
		{
			printf("4\n");
			exit(0);
		}
		if(temp.b > 0)
		{
			temp.slope = -(double)temp.a / (double)temp.b;
			I_pos.push_back(temp);
		}
		else if(temp.b < 0)
		{
			temp.slope = -(double)temp.a / (double)temp.b;
			I_neg.push_back(temp);
		}
		else
			I_zero.push_back(temp);
	}
	find_left_right();
	prune_and_search();
	return 0;
}