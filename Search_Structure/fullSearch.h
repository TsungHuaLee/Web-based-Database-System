#ifndef _FULLSEARCH_H_
#define _FULLSEARCH_H_

#include<bits/stdc++.h>
using namespace std;


typedef struct hashNode
{
    string key;
    int offset;
}hashNode;

unsigned long hash33(char *str);

void buildInvertTable(vector<vector<hashNode>>& hashTable, const char* dbName = "YouTube");

vector<int> getIntersection(vector<vector<int>> &sets);

void readFile(vector<int>& offset, const char* fileName = "./YouTube/data.rec");

/*
    search input from hashTable,
    return a vector of offset, using readFile to get data
*/
vector<int> fullSearch(vector<vector<hashNode>>& hashTable, char* input = NULL, const char* dbName= "YouTube");

#endif
