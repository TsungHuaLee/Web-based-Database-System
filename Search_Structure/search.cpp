#include<bits/stdc++.h>
using namespace std;

typedef struct OneRec
{   /*expected size -> vid + '\0' + key + '\0' == 17*/
    char data[12];
    uint key;
}oneRec;

typedef struct TreeNode
{
    vector<oneRec> rec;
    struct TreeNode** child;
    bool leaf;
}Node;

typedef struct TREE
{
    struct TreeNode* root;
}Tree;
uint primary_key;   //max data number = 4G
size_t BLOCKSIZE, ORDER = 4, maxKey;    /*one node max key = order-1, one node max child = order*/

void InitPara(int argc, char const *argv[])
{
    primary_key = 0;
    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "-m") == 0)
        {
            BLOCKSIZE = atoi(argv[i+1]);
            ORDER = BLOCKSIZE;
            i++;
        }
    }
    maxKey = ORDER - 1;
    return;
}

Node* AllocateNode()
{
    Node* newNode;
    newNode = (Node*)malloc(sizeof(Node));
    /*多宣告一個位置給 FULL 時的 key ,之後會 split */
    newNode->child = (Node**)malloc(sizeof(Node*)*(ORDER+2));
    for(int i = 0; i < ORDER+2; i++)
    {
        newNode->child[i] = NULL;
    }

    return newNode;
}

void TreeCreate(Tree* tree)
{
    Node* root = AllocateNode();
    root->leaf = true;
    tree->root = root;
    return;
}
/*
split one child (t key) to two child (each t/2 key)
*/
void SplitChild(Node* par, int idx)
{
    Node* newChild = AllocateNode();
    Node* oriChild = par->child[idx];
    newChild->leaf = oriChild->leaf;

    int mid = ORDER/2;
    oneRec upwardKey = oriChild->rec[mid];
    /*give largest order/2 key and corresponding order/2+1 child*/
    for(int mid = mid + 1, i = mid + 1; i < oriChild->rec.size(); i++)
        newChild->rec[i - mid] = oriChild->rec[i];
    if(!par->leaf)
        for(int mid = mid + 1, i = mid + 1; i <= ORDER; i++)
        {
            newChild->child[i - mid] = oriChild->child[i];
            oriChild->child[i] = NULL;
        }
    /*adjust ori child*/
    oriChild->rec.resize(mid);
    /*adjust parent child ptr, insert new child as a child of parent*/
    for(int i = par->rec.size(); i >= idx + 1; i--)
        par->child[i+1] = par->child[i];
    par->child[idx + 1] = newChild;
    par->rec.insert(par->rec.begin() + idx, upwardKey);
    return;
}
/*

*/
void InsertNonFULL(Node* curNode, oneRec* newRec)
{
    size_t size = curNode->rec.size();
    /*curNode is leaf*/
    if(curNode->leaf)
    {
        if(size >= 1)
            for(int i = 0; i < size; i++)
                if(memcmp(newRec->data, curNode->rec[i].data, sizeof(newRec->data)) < 0)
                {
                    curNode->rec.insert(curNode->rec.begin() + i, *newRec);
                    break;
                }
        else
            curNode->rec.push_back(*newRec);
    }
    else
    {
        uint idx = 0;
        /* determine child of curNode to which recursion descends */
        if(size >= 1)
            for(int i = 0; i < size; i++)
                if(memcmp(newRec->data, curNode->rec[i].data, sizeof(newRec->data)) < 0)
                {
                    idx = i;
                    break;
                }
        else
        {
            if(curNode->child[idx]->rec.size() > maxKey)
            {
                SplitChild(curNode, idx);
                if(memcmp(newRec->data, curNode->rec[idx].data, sizeof(newRec->data)) > 0)
                    idx = idx + 1;
            }
            InsertNonFULL(curNode.child[idx], newRec);
        }
    }
    return;
}
/*
    handle the case root is full
    findish by calling InsertNonFULL()
    Guarantee the root is not full!
*/
void Insert(Tree* tree, oneRec* newRec)
{
    Node* root = tree->root;
    /*case: root is full*/
    if(root->rec.size() >= maxKey)
    {
        Node* newRoot = AllocateNode();
        tree->root = newRoot;
        newRoot->leaf = false;
        newRoot->child[0] = root;
        SplitChild(newRoot, 0);
        InsertNonFULL(newRoot, newRec);
    }
    else
        InsertNonFULL(root, newRec);
    return;
}
void Exec()
{
    FILE* fin = fopen("temp", "r");
    if (fin == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    char *token, line[10000] = {};
    while(fgets(line, sizeof(line), fin) != NULL)
    {
        token = strtok(line, "\t");     // token = url
        token = strtok(line, "?v=");    // token = youtube.com
        token = strtok(NULL, "?v=");    // token = vid
        printf("%s\n", token);
        //long fPos = ftell(fin);
    }
}
int main(int argc, char const *argv[]) {
    //InitPara(argc, argv);

    Exec();

    return 0;
}
