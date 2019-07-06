#ifndef _BTREE_H_
#define _BTREE_H_

#include<bits/stdc++.h>
using namespace std;

extern int ORDER, MAXKEY;    /*one node max key = order-1, one node max child = order*/
extern vector<int> ID_Offset;
extern char DB[50];
extern vector<int> deleteTB;

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

/*
    return blockNum, which key should be.
    use searchRec() to check it excatlly exist or not.
*/
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
    if key not found then return -1,
    else return id;
    using getRecFromData(id) to get full rec
*/
int searchRec(Tree* tree, const char* key);

/*
    using after searchRec to get complete data
*/
char* getRecFromData(int id);

/*
    if rec not exist, insert as a new record and return 0.
    if res have existed, update data and return 1.
*/
int updateRec(Tree* tree, FullRec* rec);

/*
    given key or fullrec, delete this from DB.
    if fail to delete, return 0
    if delete successfully, return 1
*/
int deleteRec(Tree* tree, FullRec* rec = NULL, const char* key = NULL);

void RewriteID_Offset();

#endif
