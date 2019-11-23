#include <cstdio>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
char str1[128], str2[128];
int cnt = 0, longest_len;
char ans[200010][128], temp_ans[128] = "\0";
vector<string> final_ans;
int DP[128][128];

void dfs(int cur_i, int cur_j)
{
 	 int l = strlen(temp_ans);
 	 if(l == longest_len)
 	 {
 	 	int flag = 0;
 	 	char tmp[128];
 	 	for(int i=strlen(temp_ans)-1; i>=0; i--)
 	 		ans[cnt][flag++] = temp_ans[i];
 	 	ans[cnt++][flag] = '\0';
 	 	return;
 	 }
 	 if(cur_i == 0 || cur_j == 0)
 	 	return;
 	 for(int i=1; i<=cur_i-1; i++)
 	 {
 	 	if(DP[cur_i][cur_j] == DP[i][cur_j] && str1[i-1] == str2[cur_j-1])
 	 	{
 	 		if(DP[i][cur_j] == DP[i-1][cur_j-1] + 1)
 	 		{
 	 			temp_ans[l] = str1[i-1];
 	 			temp_ans[l+1] = '\0';
 	 			dfs(i-1, cur_j-1);
 	 			temp_ans[l] = '\0';
 	 		}
 	 	}
 	 }
 	 for(int j=1; j<=cur_j-1; j++)
 	 {
 	 	if(DP[cur_i][cur_j] == DP[cur_i][j] && str1[cur_i-1] == str2[j-1])
 	 	{
 	 		if(DP[cur_i][j] == DP[cur_i-1][j-1] + 1)
 	 		{
 	 			temp_ans[l] = str1[cur_i-1];
 	 			temp_ans[l+1] = '\0';
 	 			dfs(cur_i-1, j-1);
 	 			temp_ans[l] = '\0';
 	 		}
 	 	}
 	 }
 	 int same = (str1[cur_i-1] == str2[cur_j-1]) ? 1 : 0;
 	 if(DP[cur_i][cur_j] == DP[cur_i-1][cur_j-1] + same)
 	 {
 	 	if(same)
 	 		temp_ans[l] = str1[cur_i-1], temp_ans[l+1] = '\0';
 	 	dfs(cur_i-1, cur_j-1);
 	 	temp_ans[l] = '\0';
 	 }
}
int main()
{
	scanf("%s%s",str1,str2);
	int l1 = strlen(str1), l2 = strlen(str2);
	for(int i=1; i<=l1; i++)
	  for(int j=1; j<=l2; j++)
	  {
	  	  int same, max_dp;
	   	  same = (str1[i-1] == str2[j-1]) ? 1 : 0;
	   	  max_dp = max(DP[i-1][j-1] + same, DP[i-1][j]);
	   	  max_dp = max(DP[i][j-1], max_dp);
	   	  DP[i][j] = max_dp;
	  }
	longest_len = DP[l1][l2];
	dfs(l1, l2);
	printf("%d %d\n",longest_len,cnt);
    for(int i=0; i<cnt; i++)
        final_ans.push_back(ans[i]);

    sort(final_ans.begin(), final_ans.end());

    // for(int i=0; i<final_ans.size(); i++)
    //     printf("%s\n", final_ans[i].c_str());
    for(int i=0; i<=l1; i++)
    {
        for(int j=0; j<=l2; j++)
            printf("%d ", DP[i][j]);
        printf("\n");
	}
    return 0;
}