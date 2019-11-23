#include <iostream>
#include <vector>
#include <cstring>
#include <map>
#include <set>
#include <cstdlib>
#include <cassert>
#include "Ngram.h"

#define BUFSIZE 4096
using namespace std;

void Print(vector<string> line)
{
    cout << "<s>";
    
    for(int i = 0; i < line.size(); i++)
       cout << " " << line[i];
    
    cout << " </s>" << endl;
}

double getBigramProb(Vocab &voc, Ngram &lm, const char *w1, const char *w2)
{

    VocabIndex wid1 = voc.getIndex(w1);
    VocabIndex wid2 = voc.getIndex(w2);

    if(wid1 == Vocab_None)  // OOV
        wid1 = voc.getIndex(Vocab_Unknown);
    if(wid2 == Vocab_None)  // OOV
        wid2 = voc.getIndex(Vocab_Unknown);

    VocabIndex context[] = { wid1, Vocab_None };
    return lm.wordProb( wid2, context);
}

void BuildMapping(FILE *fp, map<string, set<string> > &mapping)
{
    char buf[BUFSIZE];
	while(fscanf(fp, "%[^\n]%*c", buf) != EOF)
    {
		string line = string(buf);
		string key = line.substr(0, 2);
        int pos;
        while((pos = line.find(" ")) != -1)
        {
				mapping[key].insert(line.substr(pos+1, 2));
            	line.erase(pos, 1);
		}
       	memset(buf, '\0', sizeof(buf));
	}
}

vector<string> HandleInputLine(char *Line)
{
    vector<string> word_vector;
    string buf;
    string tmp = string(Line);
    int pos;
    while((pos = tmp.find(" ")) != -1)
       tmp.erase(pos, 1);
    
    assert(tmp.size() % 2 == 0);
	for(int i = 0; i < tmp.size(); i += 2)
	{
       buf = tmp.substr(i, 2);
       word_vector.push_back(buf);
    }
    return word_vector;
}

vector<set<string> > FindAllPossible(vector<string> InputLine, map<string, set<string> > mapping)
{
    vector< set<string> > AllPossible;
    map<string, set<string> >::iterator iter;
    for(int i = 0; i < InputLine.size(); i++)
    {
        set<string> temp;
        iter = mapping.find(InputLine[i]);
        if(iter != mapping.end())
            temp = (iter->second);
        else
		{
			printf("mapping fuck up\n");
			exit(0);
		}
		AllPossible.push_back(temp);
    }
    return AllPossible;
}

pair<vector<string>, double> FindBestBigram(vector< pair<vector<string>, double> > AnsTable, string CurWord, Ngram &lm, Vocab &vocab)
{
    vector<string>::iterator iter;
    int idx = 0;
    double Prob, MaxProb = -1000.0;
    for(int i = 0; i < AnsTable.size(); i++){
        iter = AnsTable[i].first.end();
        iter--;
        string PreWord = *(iter);
        double oldProb = AnsTable[i].second;
        double newProb = getBigramProb(vocab, lm, PreWord.c_str(), CurWord.c_str());
        if((Prob = oldProb + newProb) > MaxProb)        //因為log相加等於值相乘
        {
            MaxProb = Prob;
            idx = i;
        }
    }
    pair<vector<string>, double> BestTable = AnsTable[idx];
    BestTable.first.push_back(CurWord);
    BestTable.second = MaxProb;
    return BestTable;
}

vector<string> Viterbi(vector< set<string> > AllPossible, Ngram &lm, Vocab &vocab)
{
    vector< pair<vector<string>, double> > AnsTable;
    set<string>::iterator iter = AllPossible.begin()->begin();

    for(iter; iter != AllPossible.begin() -> end(); iter++)
    {
        vector<string> StartSet;
        StartSet.push_back((*iter));
        AnsTable.push_back(make_pair(StartSet, 0.0));
    }

    for(int i = 1; i < AllPossible.size(); i++)
    {
        vector< pair<vector<string>, double> > NewAnsTable;
        for(iter = AllPossible[i].begin(); iter != AllPossible[i].end(); iter++)
        {
            string CurWord = (*iter);
            pair<vector<string>, double> best = FindBestBigram(AnsTable, CurWord, lm, vocab);
            NewAnsTable.push_back(best);
        }
        AnsTable.clear();
        AnsTable = NewAnsTable;
    }

    double maxProb = -1000.0;
    int idx = 0;
    for(int i = 0 ; i < AnsTable.size() ; i++)
    {
        if(AnsTable[i].second > maxProb)
        {
            idx = i;
            maxProb = AnsTable[i].second;
        }
    }
    return AnsTable[idx].first;
}

int main(int argc, char *argv[])
{
    map<string, set<string> > mapping;
    Vocab vocab;
    Ngram lm(vocab, 2);
    if(argc != 5)
    {
        printf("./disambig -lm [language model] -map [ZhuYin-Big5 file]\n");
        exit(0);
    }
    for(int i = 0 ; i < 5 ; i++)
    {
        if(strcmp("-lm", argv[i]) == 0)
        {
            File frLm(argv[i+1], "r");
            lm.read(frLm);
            frLm.close();
        }
        if(strcmp("-map", argv[i]) == 0)
        {
            FILE *fp = fopen(argv[i+1], "r");
            BuildMapping(fp, mapping);
        }
	}
    char input_str[BUFSIZE];
	while(fscanf(stdin, "%[^\n]%*c", input_str) != EOF)
    {
		vector<string> InputStrVector = HandleInputLine(input_str);
		vector< set<string> > AllPossible = FindAllPossible(InputStrVector, mapping);
        vector<string> BestLine = Viterbi(AllPossible, lm, vocab);
        Print(BestLine);
    }
return 0;
}
