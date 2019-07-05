#include "Btree.h"

int ORDER = 100, MAXKEY;
vector<int> ID_Offset;
char DB[50] = "YouTube";

void InitializeNode(Node* newNode)
{
    newNode->num = 0;
    /*多宣告一個位置給 FULL 時的 key ,之後會 split */
    newNode->rec = (oneRec*)malloc(sizeof(oneRec)*(ORDER+2));
    newNode->child = (Node**)malloc(sizeof(Node*)*(ORDER+2));
    for(int i = 0; i < ORDER+2; i++)
    {
        newNode->child[i] = NULL;
    }
    return;
}

void TreeCreate(Tree* tree)
{
    Node* root = (Node *)malloc(sizeof(Node) * 1);
    InitializeNode(root);
    root->leaf = true;
    tree->root = root;
    root = NULL;
    return;
}

void InitPara(int argc, char const *argv[])
{
    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "-d") == 0)
        {
            ORDER = atoi(argv[i+1]);
            i++;
        }
        if(strcmp(argv[i], "-db") == 0)
        {
            strcpy(DB, argv[i+1]);
            i+=2;
        }
    }
    MAXKEY = ORDER - 1;

    char fileName[100] = {};
    sprintf(fileName, "./%s/ID_Offset.rec", DB);
    FILE* fptr = fopen(fileName, "rb");
    if (fptr == NULL) {
        perror("Error while opening the file in InitPara.\n");
        exit(EXIT_FAILURE);
    }
    fseek(fptr , 0, SEEK_END);
    auto maxId = (int)ftell(fptr)/4;
    int* tempID_Offset = (int*)malloc(sizeof(int)*(maxId + 10));
    rewind(fptr);
    fread(tempID_Offset, sizeof(int), maxId, fptr);
    ID_Offset.assign(tempID_Offset, tempID_Offset+maxId);
    free(tempID_Offset);
    tempID_Offset = NULL;
    fclose(fptr);

    return;
}


/*
split one child (t key) to two child (each t/2 key)
*/
void SplitChild(Node* par, int idx)
{
    Node* newChild = (Node *)malloc(sizeof(Node) * 1);
    InitializeNode(newChild);
    Node* oriChild = par->child[idx];
    newChild->leaf = oriChild->leaf;

    int mid = ORDER/2;
    oneRec upwardKey = oriChild->rec[mid];

    /*give largest order/2 key and corresponding order/2+1 child*/
    for(auto i = mid + 1, j = 0; i < oriChild->num; i++, j++)
    {
        strcpy(newChild->rec[j].data, oriChild->rec[i].data);
        memset(oriChild->rec[i].data, 0, sizeof(oriChild->rec[i].data));
        newChild->rec[j].exNum = oriChild->rec[i].exNum;
        oriChild->rec[i].exNum = -1;
        newChild->num++;
    }

    for(auto i = mid + 1, j = mid + 1; i <= ORDER; i++)
    {
        newChild->child[i - j] = oriChild->child[i];
        oriChild->child[i] = NULL;
    }

    /*adjust ori child*/
    oriChild->num = mid;

    /*adjust parent child ptr, insert new child as a child of parent*/
    for(auto i = par->num; i >= idx + 1; i--)
        par->child[i+1] = par->child[i];
    par->child[idx + 1] = newChild;
    /*adjust parent rec, push mid key to parent*/
    if(par->num > 0)
        for(auto i = par->num; i >= idx; i--)
        {
            strcpy(par->rec[i+1].data, par->rec[i].data);
            par->rec[i+1].exNum = par->rec[i].exNum;
        }
    strcpy(par->rec[idx].data, upwardKey.data);
    par->rec[idx].exNum = upwardKey.exNum;
    par->num += 1;
    return;
}
/*
    1. Determine child of curNode to which recursion descends
    2. if child full, child split.
*/
void InsertNonFULL(Node* curNode, const oneRec* newRec)
{
    auto num = curNode->num;
    /*curNode is leaf*/
    if(curNode->leaf)
    {
        strcpy(curNode->rec[num].data, newRec->data);
        curNode->rec[num].exNum = newRec->exNum;
        curNode->num++;
    }
    else
    {
        auto idx = 0;
        /* determine child of curNode to which recursion descends , sort by (min to max) */
        for(auto i = 0; i < num; i++)
            if(memcmp(newRec->data, curNode->rec[i].data, strlen(curNode->rec[i].data)) < 0)
            {
                idx = i;
                break;
            }
            else    /* case: newRec point to last child */
            {
                idx = i + 1;
            }
        if(curNode->child[idx]->num > MAXKEY)
        {
            SplitChild(curNode, idx);
            /*determine which child should go*/
            if(memcmp(newRec->data, curNode->rec[idx].data, strlen(newRec->data)) > 0)
                idx = idx + 1;
        }
        InsertNonFULL(curNode->child[idx], newRec);
    }
    return;
}
/*
    Handle the case root is full,
    findish by calling InsertNonFULL().
    Guarantee the root is not full!
*/
Tree* Insert(Tree* tree, const oneRec* newRec)
{
    Node* root = tree->root;
    /*case: root is full*/
    if(root->num > MAXKEY)
    {
        Node* newRoot = (Node*)malloc(sizeof(Node));
        InitializeNode(newRoot);
        tree->root = newRoot;
        newRoot->child[0] = root;
        newRoot->leaf = false;
        SplitChild(tree->root, 0);
        InsertNonFULL(tree->root, newRec);
        newRoot = NULL;
    }
    else
    {
        InsertNonFULL(root, newRec);
    }
    return tree;
}

int search(Node* node, const char* key)
{
    /* idx ==> the last node[idx] greater than search key */
    int idx = 0, comp = 0;
    for(int i = 0; (i < node->num); i++)
    {
        comp = memcmp(key, node->rec[i].data, (int)strlen(node->rec[i].data));
        if(comp > 0)
            idx++;
        else
            break;
    }
    // excatlly find in tree
    if(idx < node->num && comp == 0)
        return node->rec[idx].exNum;
    // go to find external file
    if(node->leaf)
        return node->rec[idx-1].exNum;
    else
        return search(node->child[idx], key);
}

void Build(Tree* tree, TreeInfo* treeInfo, const char* dbName)
{
    TreeCreate(tree);

    /* Read info from blockInfo to build tree*/
    char fileName[100] = {};
    sprintf(fileName, "./%s/blockInfo", dbName);
    FILE *fptr = NULL, *blockInfoFile = fopen(fileName, "rb");
    if(blockInfoFile == NULL)
    {
        perror("Error while opening the file in Build().\n");
        exit(EXIT_FAILURE);
    }
    fseek(blockInfoFile, 0, SEEK_END);
    auto totalBlockNum = (int)ftell(blockInfoFile) / 4;
    rewind(blockInfoFile);
    int *blockList = (int*)malloc(sizeof(int)*(totalBlockNum + 10));
    fread(blockList, sizeof(int), totalBlockNum, blockInfoFile);
    fclose(blockInfoFile);

    /* Read min data from each blockfile and insert to tree*/
    oneRec newRec;
    for(int i = 0; i < totalBlockNum; i++)
    {
        sprintf(fileName, "./%s/block%d", dbName, i);
        fptr = fopen(fileName, "rb");
        fread(&newRec.data, 12, 1, fptr);
        newRec.exNum = blockList[i];
        tree = Insert(tree, &newRec);

        fclose(fptr);
    }

    /*store tree info*/
    treeInfo->blockNum = totalBlockNum;
    treeInfo->blockList.assign(blockList, blockList+totalBlockNum);
    free(blockList);
    blockList = NULL;

    return;
}

void DeleteNode(Node* node)
{
    free(node->rec);
    free(node->child);
    return;
}

void DeleteTree(Node* node)
{
    if (node == NULL) return;
    /* first delete both subtrees */
    for(int i = 0; i < node->num; i++)
        DeleteTree(node->child[i]);
    DeleteNode(node);
    return;
}

/*
-----------------------------------------------------------
-----------------------------------------------------------
*/

int searchRec(Tree* tree, const char* key)
{
    int blockNum = search(tree->root, key);
    char fileName[100]={}, nullchar = '\0', vid[100] = {};
    int id;
    auto found = false;

    sprintf(fileName, "./%s/block%d", DB, blockNum);
    FILE* fptr = fopen(fileName, "rb");
    if(fptr == NULL)
    {
        perror("Error while opening the file in searchRec()\n");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        if(feof(fptr))
            break;
        fread(&vid, sizeof(char), 11, fptr);
        fread(&nullchar, sizeof(char), 1, fptr);
        fread(&id, sizeof(int), 1, fptr);
        fread(&nullchar, sizeof(char), 1, fptr);
        if(memcmp(key, vid, (int)strlen(vid)) == 0)
        {
            found = true;
            break;
        }
    }
    fclose(fptr);

    if(!found)
    {
        return -1;
    }
    else
    {
        return id;
    }
}

char* getRecFromData(int id)
{
    char* line = new char[510], fileName[100] = {};
    sprintf(fileName, "./%s/data.rec", DB);
    FILE *fptr = fopen(fileName, "rb");
    if(fptr == NULL)
    {
        perror("Error when opening the file in searchRec()");
        exit(EXIT_FAILURE);
    }
    fseek(fptr, ID_Offset[id], SEEK_SET );
    fread(line, sizeof(char), 500, fptr);
    fclose(fptr);
    return line;
}
/*
    if new record not exist, insert new record,
    else update record.
*/
int updateRec(Tree* tree, FullRec* rec)
{
    char *vid;
    char tempUrl[500] = {};
    strcpy(tempUrl, rec->url);
    vid = strtok(tempUrl, "?v="); // token = youtube.com
    vid = strtok(NULL, "?v="); // token = vid

    /*
        check new rec existed or not
    */
    int id = searchRec(tree, vid);
    /*
        if rec not exist, insert as a new record.
    */
    if(id == -1)
    {
        auto blockNum = search(tree->root, vid);
        cout<<"should insert to block"<<blockNum<<endl;
        /*update internal node*/
        oneRec newRec;
        strcpy(newRec.data, vid);
        newRec.exNum = blockNum;
        tree = Insert(tree, &newRec);

        /*write to block file*/
        int id = (int)ID_Offset.size();
        char fileName[100] = {}, nullchar = '\0';
        sprintf(fileName, "./%s/block%d", DB, blockNum);
        FILE* fptr = fopen(fileName, "r+b");
        if(fptr == NULL)
        {
            perror("Error when opening the file in insertRec()\n");
            exit(EXIT_FAILURE);
        }
        fseek(fptr , 0, SEEK_END);
        cout<<"write out: "<<vid<<endl;
        fwrite(&vid, 11, 1, fptr);
        fwrite(&nullchar, sizeof(char), 1, fptr);
        fwrite(&id, sizeof(int), 1, fptr);
        fwrite(&nullchar, sizeof(char), 1, fptr);
        fclose(fptr);

        /*write to data file*/
        sprintf(fileName, "./%s/data.rec", DB);
        fptr = fopen(fileName, "r+b");
        if(fptr == NULL)
        {
            perror("Error when opening the file in insertRec()\n");
            exit(EXIT_FAILURE);
        }

        /*update id offset table*/
        fseek(fptr , 0, SEEK_END);
        auto offset = (int)ftell(fptr);
        ID_Offset.push_back(offset);

        char output[10010] = {};
        memset(output, 0, sizeof(output));
        sprintf(output, "%s\t%s\t%s\t%d\t%d\t%d", rec->url, rec->title, rec->content, rec->viewCount, rec->res, rec->duration);
        fwrite(output, sizeof(char), 500, fptr);
        cout<<"blockNum = "<<blockNum<<" id = "<<id<<" offset = "<<ID_Offset[id]<<endl;
        cout<<"new record : \n"<<output<<endl;
        fclose(fptr);

        /*
            update id_offset file at end of program
        */

        return 0;
    }
    else
    {
        /*update data file*/
        char fileName[100] = {};
        sprintf(fileName, "./%s/data.rec", DB);
        FILE *fptr = fopen(fileName, "r+b");
        if(fptr == NULL)
        {
            perror("Error when opening the file in searchRec()");
            exit(EXIT_FAILURE);
        }
        fseek(fptr, ID_Offset[id], SEEK_SET );
        auto offset = (int)ftell(fptr);
        /*update id offset table*/
        ID_Offset[id] = offset;
        cout<<"id = "<<id<<" offset = "<<offset<<endl;
        char output[10010] = {};
        memset(output, 0, sizeof(output));
        sprintf(output, "%s\t%s\t%s\t%d\t%d\t%d", rec->url, rec->title, rec->content, rec->viewCount, rec->res, rec->duration);
        cout<<"update record : \n"<<output<<endl;
        fclose(fptr);

        /*
            update id_offset file at end of program
        */

        return 1;
    }

}

// void deleteRec(Tree* tree, FullRec* rec)
// {
//     char *vid;
//     vid = strtok(rec->url, "?v="); // token = youtube.com
//     vid = strtok(rec->url, "?v="); // token = vid
//     int blockNum = search(tree->root, vid);
//
//     char fileName[100]={}, nullchar = '\0';
//     sprintf(fileName, "%s%d", preFileName, blockNum);
//     FILE* fptr = fopen(fileName, "wb+");
//     int idx = -1, i = 0;
//     vector<BlockRec> vec;
//     BlockRec tempRec;
//     while(!feof(fptr))
//     {
//         fread(&tempRec.vid, 11, 1, fptr);
//         fread(&nullchar, sizeof(char), 1, fptr);
//         fread(&tempRec.id, sizeof(int), 1, fptr);
//         fread(&nullchar, sizeof(char), 1, fptr);
//         if(memcmp(vid, tempRec.vid, strlen(vid)) == 0)
//         {
//             idx = i;
//         }
//         vec.push_back(tempRec);
//         i += 1;
//     }
//     fclose(fptr);
// }

void RewriteID_Offset()
{
    char fileName[100] = {};
    sprintf(fileName, "./%s/ID_Offset.rec", DB);
    ofstream fptr(fileName, ios::out | ios::binary);
    fptr.write((char*)&ID_Offset[0], ID_Offset.size() * sizeof(int));
    fptr.close();
    return;
}
