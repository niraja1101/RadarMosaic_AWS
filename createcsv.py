import os
import re
import datetime
import sys
import csv
from pathlib import Path
if os.name == "posix":
    home = str(Path.home())
    print(home)
def calc_diff(mosaic, temp):
    tempdate = datetime.datetime.strptime(temp, '%Y%m%d_%H%M%S')
    timedelta = mosaic - tempdate
    diff = timedelta.total_seconds() / 60
    if diff > 0:
        return diff
    else:
        return -1


csv_stlist = []
firstrow = ['ID', 'Mosaic time']
station_ids = []
n = int(input("Enter number of stations "))
for i in range(n):
    st_id = input("Enter station ID: ")
    if os.name == "posix":
        directory = home + os.sep + st_id
    else:
        directory = os.path.curdir + os.sep + st_id

    while not os.path.exists(directory):
        st_id = input("Directory " + directory + "does not exist, enter correct station ID ")
        if os.name == "posix":
            directory = home + os.sep + st_id
        else:
            directory = os.path.curdir + os.sep + st_id
    station_ids.append(st_id)
    firstrow.append(st_id)
csv_stlist.append(firstrow)

mosaic_d = input("Enter mosaic date in format YYYYMMDD or q to exit ")
if mosaic_d == 'q':
    sys.exit(0)
while not re.match('^[0-9]{8}$', mosaic_d):
    mosaic_d = input("Please enter correct mosaic time in format YYYYMMDD or quit(q) ")
    if mosaic_d == 'q':
        sys.exit(0)

mosaic_date = datetime.datetime.strptime(mosaic_d, '%Y%m%d')
print(mosaic_date)
f = input("Enter frequency of mosaic time ")
maxim = int(1440 / int(f))
date_list = [mosaic_date + datetime.timedelta(minutes=int(f) * x) for x in range(1, maxim + 1)]

row_id = 0
for mosaic in date_list:
    row_id += 1
    newlist = [str(row_id), mosaic.strftime('%Y%m%d_%H%M%S')]
    for each in station_ids:

        if os.name == "posix":
            folder = home + os.sep + each
        else:
            folder = os.path.curdir + os.sep + each

        files = []
        exp = each + mosaic_d
        for filp in os.listdir(folder):
            if exp in filp:
                files.append(filp)

        finfile = "-"
        lasttimediff = 15
        for everyfile in files:

            if 'V06.gz' in everyfile:
                temp = everyfile[4:-7]
            elif 'V03.gz' in everyfile:
                temp = everyfile[4:-7]
            elif 'V04.gz' in everyfile:
                temp = everyfile[4:-7]
            elif 'V00.gz' in everyfile:
                temp = everyfile[4:-7]
            elif 'V01.gz' in everyfile:
                temp = everyfile[4:-7]
            elif 'V06' in everyfile:
                temp = everyfile[4:-4]
            elif 'V03' in everyfile:
                temp = everyfile[4:-4]
            elif 'V04' in everyfile:
                temp = everyfile[4:-4]
            elif 'V00' in everyfile:
                temp = everyfile[4:-4]
            elif 'V01' in everyfile:
                temp = everyfile[4:-4]
            elif '.gz' in everyfile:
                temp = everyfile[4:-3]
            else:
                temp = everyfile[4:]

            timediff = calc_diff(mosaic, temp)

            if timediff == 0:
                finfile = everyfile
                break

            if timediff > 0 and timediff < 15 and timediff < lasttimediff:
                lasttimediff = timediff
                finfile = everyfile
            else:
                continue

        newlist.append(finfile)
    csv_stlist.append(newlist)
myFile = open('task.csv', 'w',  newline='')
with myFile:
    writer = csv.writer(myFile)
    writer.writerows(csv_stlist)
