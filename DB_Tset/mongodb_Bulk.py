import pymongo
import random
myclient = pymongo.MongoClient("mongodb://localhost:27017")

mydb = myclient["YouTubeDB"]

mycol = mydb["HW1data"]


with open("oneLineData.rec", "r", encoding='utf-8') as fin:
    line = fin.readlines()
    index = 0
    num = 0
    mylist = []
    while index < len(line):
        temp = line[index].split('\t')
        if(len(temp) >= 6):
            tempdict = {
                'url': temp[0],
                'title': temp[1],
                'content': temp[2],
                'viewCount': temp[3],
                'res': temp[4],
                'duration': temp[5]
            }
            mylist.append(tempdict)
            num += 1
        if num > 1000:
            num = 0
            x = mycol.insert_many(mylist)
            mylist = []

        index += 1
        if(index % 100000 == 0):
            print(index)
    x = mycol.insert_many(mylist)
