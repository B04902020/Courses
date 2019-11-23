#include <cstdio>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <cmath>
using namespace std;
#define Max 9223372036854775807
 
struct point
{
	long long int x, y, input_order;
};
typedef struct point Point;
struct close_pair
{
	int min, max;
};
typedef struct close_pair Close_pair;
bool cmp(Point a, Point b) { return (a.y < b.y); }
bool cmp2(Close_pair a, Close_pair b)
{
	if(a.min < b.min)
		return 1;
	else if(a.min == b.min && a.max < b.max)
		return 1;
	else
		return 0;
}

vector<Point> all_point;	
int n;
long long min_dist = Max;
vector<Close_pair> ans;
vector<int> x_pos;
vector<Point> new_left, new_right;

long long dist(Point p1, Point p2)
{
	return ((long long)((p1.x - p2.x)*(p1.x - p2.x)) + 
	(long long)((p1.y - p2.y)*(p1.y - p2.y)));
}
 
void update(long long int dis, Point a, Point b)
{
	Close_pair temp1, temp2;
	temp1.min = a.input_order, temp1.max = b.input_order;
	temp2.min = b.input_order, temp2.max = a.input_order;
	if(dis == min_dist)
		(a.input_order < b.input_order) ? ans.push_back(temp1) : ans.push_back(temp2);
	
	else if(dis < min_dist)
	{
		ans.clear();
		(a.input_order < b.input_order) ? ans.push_back(temp1) : ans.push_back(temp2);
		min_dist = dis;
	}
}
 
long long d_and_c(vector<Point> p)
{
 
	if(p.size() == 1)
		return Max;
	else if(p.size() == 2)
	{
		long long int dis = dist(p[0], p[1]);
		//printf("dis = %lld\n", dis);
		update(dis, p[0], p[1]);
		return dis;
	}
	x_pos.clear();
	for(int i=0; i<p.size(); i++)
		x_pos.push_back(p[i].x);
	
	nth_element(x_pos.begin(), x_pos.begin() + p.size()/2, x_pos.end());
	int middle_x = x_pos[p.size()/2]; 
	
	vector<Point> left, right;
	long long int left_dist, right_dist, min;
	
	for(int i=0; i<p.size(); i++)
	{
		if(p[i].x >= middle_x)
			right.push_back(p[i]);
		else
			left.push_back(p[i]);
	}
	if(left.size() == 0)			//左右數量平分
	{
		for(int i=0; i<right.size(); i++)
		{
			if(right[i].x == middle_x)
			{
				left.push_back(right[i]);
				right.erase(right.begin()+i);
			}
			if(left.size() >= p.size()/2)
				break;
		}
	}
	left_dist = d_and_c(left);
	right_dist = d_and_c(right);
	min = (left_dist < right_dist) ? left_dist : right_dist;
	
	new_left.clear();
	new_right.clear();
	for(int i=0; i<left.size(); i++)
	{
		long long dis_temp;
		dis_temp = (left[i].x - middle_x)*(left[i].x - middle_x);
		if(min >= dis_temp)
			new_left.push_back(left[i]);
	}
	for(int i=0; i<right.size(); i++)
	{
		long long dis_temp;
		dis_temp = (right[i].x - middle_x)*(right[i].x - middle_x);
		if(min >= dis_temp)
			new_right.push_back(right[i]);
	}
	
	if(new_left.size() == 0 || new_right.size() == 0)
		return min;

	int cnt_left = 0, cnt_right = 0;
	while(cnt_left < (int)new_left.size())
	{
		long long dis1 = dist(new_left[cnt_left], new_right[cnt_right]);
		if(dis1 <= min)
		{
			update(dis1, new_left[cnt_left], new_right[cnt_right]);
			min = dis1;
			for(int r=-3; r<=3; r++)
			{
				if(cnt_right + r >= 0 && cnt_right + r < (int)new_right.size() && r != 0)
				{
					long long dis2 = dist(new_left[cnt_left], new_right[cnt_right+r]);
					if(dis2 <= min)
					{
						update(dis2, new_left[cnt_left], new_right[cnt_right+r]);
						min = dis2;
					}
				}
			}
			cnt_left++;
		}
		else if(min < dis1 && new_left[cnt_left].y >= new_right[cnt_right].y)
		{
			if(cnt_right == (int)new_right.size() - 1)
				cnt_left++;
			else
				cnt_right++;
		}
		else if(min < dis1 && new_left[cnt_left].y < new_right[cnt_right].y)
			cnt_left++;
	}
	return min;
}
 
void print()
{
	sort(ans.begin(), ans.end(), cmp2);
	printf("%lld %lu\n", min_dist, ans.size());
	for(int i=0; i<ans.size(); i++)
		printf("%d %d\n",ans[i].min,ans[i].max);
}
 
int main()
{
	scanf("%d",&n);
	for(int i=1; i<=n; i++)
	{
		Point temp;
		scanf("%lld%lld",&temp.x,&temp.y);
		temp.input_order = i;
		all_point.push_back(temp);
	}
	sort(all_point.begin(), all_point.end(), cmp);
	
	d_and_c(all_point);
 
	print();
return 0;
}