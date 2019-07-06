#include "fullSearch.h"
#include "Btree.h"
using namespace std;

unsigned long hash33(char *str)
{
    unsigned long hash = 5381;
    int c;

    for (int i = 0; i < (int)strlen(str); ++i)
        str[i] = tolower(str[i]);

    while (c = *str++)
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}


void buildInvertTable(vector<vector<hashNode>>& hashTable, const char* dbName)
{
    char fileName[50] = {};
    sprintf(fileName, "./%s/data.rec", dbName);
    FILE * fptr = fopen(fileName, "rb");
    // FILE * fptr = fopen("./YouTube/data.rec", "rb");
    if(fptr == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    // insert
    int offset = 0;
    unsigned long hashNum = 0;
    hashNode tempNode;
    char block[1000] = {}, *token, *title;
    while(1)
    {
        if(feof(fptr))
            break;
        fread(block, sizeof(char), 500, fptr);
        /*if 500 char are '\0', this block was deleted from data*/
        if(strlen(block) == 0)
            continue;
        title = strtok(block, "\t");
        title = strtok(NULL, "\t"); /*get title*/

        token = strtok(title, " ,.-()");
        while(token != NULL)
        {
            hashNum = hash33(token);
            tempNode.key = token;
            tempNode.offset = offset;
            int idx = hashNum%200000;
            // cout<<idx<<endl;
            hashTable[idx].push_back(tempNode);
            token = strtok(NULL, " ,.-()");
        }
        offset = (int)ftell(fptr);
    }
    fclose(fptr);
    return;
}

vector<int> getIntersection(vector<vector<int>> &sets)
{
    if(sets.size() < 2)
        return sets[0];

    vector<int> re;
    vector<int>::iterator it;

    /*sort*/
    int maxSize = 0;
    for(int i = 0; i < (int)sets.size(); i++)
    {
        sort(sets[0].begin(), sets[0].end());
        maxSize = ((int)sets[i].size() > maxSize)?sets[i].size():maxSize;
    }
    re.resize(maxSize);

    /*get intersection*/
    it = set_intersection(sets[0].begin(), sets[0].end(), sets[1].begin(), sets[1].end(), re.begin());
    re.resize(it-re.begin());
    for(int i = 2; i < (int)sets.size(); i++)
    {
        it = set_intersection(sets[i].begin(), sets[i].end(), re.begin(), re.end(), re.begin());
        re.resize(it-re.begin());
    }

    return re;
}

void readFile(vector<int>& offset, const char* fileName)
{
    FILE *fptr = fopen(fileName, "rb");
    vector<int>::iterator it;
    char block[1000] = {};
    for(it = offset.begin(); it != offset.end(); it++)
    {
        fseek(fptr, *it, SEEK_SET);
        fread(block, sizeof(char), 500, fptr);
        cout<<block<<endl;
    }
    fclose(fptr);

    return ;
}
// Search
// /*Talib Kweli - Move Something (DogmaMts Remix) - YouTube*/
vector<int> fullSearch(vector<vector<hashNode>>& hashTable, char* input, const char* dbName)
{
    vector<vector<int>> allSets;
    vector<int> oneSet;
    hashNode tempNode;

    char *token;
    token = strtok(input, " ,.-()");
    while(token != NULL)
    {
        unsigned long hashNum = hash33(token);
        int idx = hashNum % 200000;
        for (vector<hashNode>::iterator it = hashTable[idx].begin() ; it != hashTable[idx].end(); ++it)
        {
            if(it->key.compare(token) == 0)
            {
                // cout << ' ' << it->key << " "<<it->offset<<endl;
                for(auto deleteTBit = deleteTB.begin(); deleteTBit != deleteTB.end(); deleteTBit++)
                    if(*deleteTBit == it->offset)
                        continue;
                oneSet.push_back(it->offset);
            }
        }
        allSets.push_back(oneSet);
        // cout<<idx<<endl;
        token = strtok(NULL, " ,.-()");
        oneSet.clear();
    }

    vector<int> intersection = getIntersection(allSets);

    char fileName[100] = {};
    sprintf(fileName, "./%s/data.rec", dbName);
    readFile(intersection, fileName);
    return intersection;
}
