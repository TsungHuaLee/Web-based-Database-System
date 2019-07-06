#include "fullSearch.h"
#include "Btree.h"
#include<bits/stdc++.h>
using namespace std;

int main(int argc, char const *argv[])
{
    vector<vector<hashNode>> hashTable;
    hashTable.resize(200000);
    buildInvertTable(hashTable);

    InitPara(argc, argv);
    Tree tree;
    TreeInfo treeInfo;

    Build(&tree, &treeInfo);

    FullRec fullrec = {"https://www.youtube.com/watch?v=88888888888",
    "test new rec title",
    "test new rec conten",
    0,
    0,
    0
    };
    char input[510];
    while(1)
    {
        cout<<"command:";
        cin.getline(input, 500);
        /*quit*/
        if(strcmp(input, "q") == 0)
            break;
        /*search*/
        if(strcmp(input, "s") == 0)
        {
            cout<<"input search key:";
            cin.getline(input, 500);

            int id = searchRec(&tree, input);
            if(id == -1)
            {
                cout<<"REC not found!!!\n";
                continue;
            }
            char* result = new char[510];
            result = getRecFromData(id);
            cout<<result<<endl;
            delete result;
            continue;
        }
        /*fullsearch*/
        if(strcmp(input, "f") == 0)
        {
            cout<<"input search key:";
            cin.getline(input, 500);
            vector<int> re = fullSearch(hashTable, input);
            if(re.size() == 0)
                cout<<"input not found by Full Search!\n";
            else
                readFile(re, "./YouTube/data.rec");
            continue;
        }
        /*update*/
        if(strcmp(input, "u") == 0)
        {
            int a = updateRec(&tree, &fullrec);
            if(a)
                cout<<"update currently data successfully!\n";
            else
            {
                cout<<"insert new data successfully!\n";
            }
            continue;
        }
        /*update*/
        if(strcmp(input, "d") == 0)
        {
            int a = deleteRec(&tree, NULL, "88888888888");
            if(a)
                cout<<"delete successfully!\n";
            else
                cout<<"Failed to delete!\n";
            continue;
        }
    }
    RewriteID_Offset();
    DeleteTree(tree.root);

    return 0;
}
