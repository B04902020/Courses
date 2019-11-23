#include <cstdio>
#include <algorithm>
#include <vector>
#include <cmath>
#include <cstdlib>
using namespace std;
struct item
{
	int weight, value;
	double ratio;
};
typedef struct item Item;
bool cmp(Item a, Item b) {return (a.ratio > b.ratio);}

int ans = 0, n, weight_limit;
vector<Item> arr;

void find_lower(int limit)
{
	int cur_value = 0;
	for(int i=0; i<n; i++)
	{
		if(limit - arr[i].weight >= 0)
		{
			cur_value += arr[i].value;
			limit -= arr[i].weight;
		}
		else
			break;
	}
	ans = cur_value;
}

void knapsack(int cur, int cur_value, int limit)
{
	if(limit < 0)
		return;
	if(cur >= n || limit == 0)
	{
		ans = (cur_value > ans) ? cur_value : ans;
		return;
	}
	int temp_limit = limit;
	double upper = cur_value;
	for(int i=cur; i<n; i++)
	{
		if(temp_limit - arr[i].weight >= 0)
		{
			upper += arr[i].value;
			temp_limit -= arr[i].weight;
		}
		else if(temp_limit > 0 && temp_limit - arr[i].weight < 0)
		{
			upper += temp_limit*arr[i].ratio;
			temp_limit = 0;
		}
		if(temp_limit == 0)
			break;
	}
	//printf("upper = %f\n",upper);
	if(upper <= ans)
		return;
	knapsack(cur+1, cur_value+arr[cur].value, limit-arr[cur].weight);
	knapsack(cur+1, cur_value, limit);
	return;
}

int main()
{
	scanf("%d%d",&weight_limit,&n);
	for(int i=0; i<n; i++)
	{
		Item temp;
		scanf("%d%d",&temp.value,&temp.weight);
		temp.ratio = (double)temp.value/(double)temp.weight;
		arr.push_back(temp);
	}
	sort(arr.begin(), arr.end(), cmp);
	
	find_lower(weight_limit);
	
	knapsack(0,0,weight_limit);
	
	printf("%d\n", ans);

return 0;
}