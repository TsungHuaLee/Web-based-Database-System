from elasticsearch import helpers
import elasticsearch
es = elasticsearch.Elasticsearch()

result = es.indices.delete(index='youtubedb', ignore=[400, 404])
#for i in range(1,200000):
#    es.delete(index='youtubedb', doc_type='hw1data', id=i)

# insert bulk

es.indices.create(index='youtubedb', ignore=400)

with open("oneLineData.rec", "r", encoding='utf-8') as fin:
    line = fin.readlines()
    index = 1
    num = 0
    ACTIONS = []
    while index < len(line):
        temp = line[index].split('\t')
        if(len(temp) >= 6):
            tempdict = {
                '_op_type': 'index',
                "_index":"youtubedb",
                "_type":"hw1data",
                "_id": index,
                "_source":{
                    'url': temp[0],
                    'title': temp[1],
                    'content': temp[2],
                    'viewCount': temp[3],
                    'res': temp[4],
                    'duration': temp[5]
                }
            }
            #result = es.index(index='youtubedb', doc_type='hw1data', id=index, body=tempdict)
            ACTIONS.append(tempdict)
            num += 1
        if num > 10:
            num = 0
            helpers.bulk(es, ACTIONS, index='youtubedb')
            ACTIONS = []

        index += 1
        if(index % 100000 == 0):
            print(index)
    if len(ACTIONS) > 0:
        helpers.bulk(es, ACTIONS, index='youtubedb')
        #elasticsearch.bulk(es, actions)

'''
#search
with open("keyword.rec", "r") as fin:
    line = fin.readlines()
    for i in line:
        body = '{"query":{"match":{"content":"%s"}}}'%(i.rstrip('\n'))
        re = es.search(index="youtubedb",doc_type="hw1data",body=body)
        print(re)
'''
