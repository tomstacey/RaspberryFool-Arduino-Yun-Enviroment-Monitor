#!/usr/bin/python

import gspread
import time
import sys
import csv
#import numpy

sys.path.insert(0, '/usr/lib/python2.7/bridge/')

from bridgeclient import BridgeClient as bridgeclient

value = bridgeclient()

#open config file
	
csv_file_object = csv.reader(open('config.txt', 'rb')) 
header = csv_file_object.next()  
data=[]                          #Create a variable called 'data'
for row in csv_file_object:      #Run through each row in the csv file
    data.extend(row)             #adding each row to the data variable
			        

# Login with your Google account

gc = gspread.login(data[0], data[1])
sh = gc.open_by_key(data[2])
wks= sh.get_worksheet(0)

timestamp = 0
nodeid = 0
temperature = 0
humidity = 0
lightlevel = 0
update=0
row=2
col=0


try:
#       print("python has started")
        while True:
                update = value.get('update')
                #print(update)
                #print(lightlevel)
                if update > 2:
                        value.put('update', 0)
#                       print("Update spreadsheet")

#                       timestamp = ("=now()")
#                       timestamp = time.strftime("%d:%m:%Y-%H:%M:%S")         
                        timestamp = value.get('timestamp')
#                       print(timestamp)
                        nodeid = value.get('nodeid')
                        humidity = value.get('humidity')
                        temperature = value.get('temperature')
                        lightlevel = value.get('lightlevel')
                        audiolevel = value.get('audiolevel')

                        wks.update_cell(row, 1, timestamp)
                        wks.update_cell(row, 2, nodeid)
                        wks.update_cell(row, 3, humidity)
                        wks.update_cell(row, 4, temperature)
                        wks.update_cell(row, 5, lightlevel)
                        wks.update_cell(row, 6, audiolevel)
                        #timestamp = wks.cell(row, 1)
                        #print(timestamp)
                        #wks.update_cell(row, 1, timestamp)

                        row += 1

#       time.sleep(1)
        update = 0


except KeyboardInterrupt:
        print("\n done")


