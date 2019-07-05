#ifndef _BTREE_H_
#define _BTREE_H_

#include<bits/stdc++.h>
using namespace std;

extern int ORDER, MAXKEY;    /*one node max key = order-1, one node max child = order*/
extern vector<int> ID_Offset;
extern char DB[50];


typedef struct
{
    char vid[12];
    int id;
}BlockRec;

typedef struct
{   /*expected size -> vid + '\0' + key + '\0' == 17*/
    char data[12];
    int exNum;
}oneRec;

typedef struct TreeNode
{
    oneRec* rec;
    struct TreeNode** child;
    int num;
    bool leaf;
}Node;

typedef struct TREE
{
    struct TreeNode* root;
}Tree;

typedef struct
{
    int blockNum;
    vector<int> blockList;
}TreeInfo;

void InitializeNode(Node* newNode);

void TreeCreate(Tree* tree);

void InitPara(int argc, char const *argv[]);

void SplitChild(Node* par, int idx);

void InsertNonFULL(Node* curNode, const oneRec* newRec);

Tree* Insert(Tree* tree, const oneRec* newRec);

int search(Node* node, const char* key);

void Build(Tree* tree, TreeInfo* treeInfo, const char* dbName = "YouTube");

void DeleteNode(Node* node);

void DeleteTree(Node* node);

/*
-----------------------------------------------------------
-----------------------------------------------------------
*/

typedef struct
{
    char url[500];
    char title[500];
    char content[500];
    int viewCount;
    int res;
    int duration;
}FullRec;

/*
    if not found return -1, else return id;
    using getRecFromData(id) to get full rec
*/
int searchRec(Tree* tree, const char* key);

char* getRecFromData(int id);

int updateRec(Tree* tree, FullRec* rec);

void RewriteID_Offset();

#endif
