import pymysql
import re
import time

db = pymysql.connect(host="localhost", user="root",
        password="lee1271232", db="YouTubeDB", charset="utf8mb4")

cursor = db.cursor()
cursor.execute("SET NAMES utf8mb4")
cursor.execute("SET CHARACTER SET utf8mb4")
cursor.execute("SET character_set_connection = utf8mb4")


start = time.time()
with open("dele.sql") as fin:
    line = fin.readline()
    num = 1
    while line:
        try:
            m = cursor.execute(line)
            db.commit()
        except Exception as err:
            db.rollback()
        num += 1

        line = fin.readline()

'''
with open("sqldata2.rec", "r", encoding='utf-8') as fin:
    line = fin.readlines()
    index = 0
    while index < 2000000:
        param=[]
        for i in range(1000):
            temp = line[index].split('\t')
            data = tuple(temp)
            param.append(data)
            index += 1
        #print(index)
        sql = "INSERT INTO HW1data(url, title, content, viewCount, res, duration) values(%s,%s,%s,%s,%s,%s)"
        try:
            cursor.executemany(sql, param)
            db.commit()
        except Exception as e:
            #print(e)
            db.rollback()
'''
db.close()
