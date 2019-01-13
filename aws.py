import os
import sys
import boto3
from pathlib import Path
import botocore.exceptions
from botocore.handlers import disable_signing
from datetime import date, timedelta

if os.name == "posix":
    home = str(Path.home())
s3 = boto3.resource('s3')
s3.meta.client.meta.events.register('choose-signer.s3.*', disable_signing)
mybucket = s3.Bucket("noaa-nexrad-level2")

if len(sys.argv) < 4:
    station_id = input("Enter station ID: ")
    while len(station_id) != 4 or (not station_id.isupper()):
        station_id = input("Please enter correct station id: ")
    date_from = input("Enter start date in format YYYY/MM/DD ")
    date_to = input("Enter date to in format YYYY/MM/DD ")
else:
    station_id = sys.argv[1]
    date_from = sys.argv[2]
    date_to = sys.argv[3]

list_from = date_from.split('/')
# print(list_from)
date_f = date(int(list_from[0]), int(list_from[1]), int(list_from[2]))
# print(date_f)


list_to = date_to.split('/')
# print(list_to)
date_t = date(int(list_to[0]), int(list_to[1]), int(list_to[2]))
# print(date_t)
print("Downloading all files from " + date_from + " to " + date_to + " from station " + station_id)
delta = date_t - date_f
print(delta)
# print(delta.days)
date_list = []

for i in range(delta.days + 1):
    date_list.append((date_f + timedelta(i)).strftime('%Y/%m/%d') + "/" + station_id)

if os.name == "posix":
    directory = home + os.sep + station_id
else:
    directory = os.path.curdir + os.sep + station_id
if not os.path.exists(directory):
    os.makedirs(directory)

for bucket_prefix in date_list:
    print(bucket_prefix)
    for obj in mybucket.objects.filter(Prefix=bucket_prefix):
        try:
            print(" Key =" + obj.key)
            check = directory + os.sep + obj.key.split('/')[4]
            if not os.path.exists(check):
                mybucket.download_file(obj.key, directory + os.sep + obj.key.split('/')[4])
                print("Downloaded " + obj.key)
            if check.endswith(".gz"):
                os.system(f"gzip -d {check}")
            if check.endswith(".bz2") or check.endswith(".bzip2"):
                os.system(f"bzip2 -d {check}")

        except botocore.exceptions.ClientError as e:
            if e.response['Error']['Code'] == "404":
                print("The object does not exist.")
            else:
                raise
