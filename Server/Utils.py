import paho.mqtt.client as mqtt
import subprocess

import yaml
import time
import numpy as np


##
##  testParse Functions
##

# Turn the data String into a list
def stringToList(dataString):
    data = dataString.split(",")
    return data

# db_name, table_name, data_name, data, mac_address, start_timestamp, interval
# shake    Z           value      1,2,3 00.00.00.00.00 1692491796.001 .001
def parseString(exampleString):
    returnDict = {}
    
    stringList = exampleString.split(" ")

    # clean up the ints
    returnDict["db_name"] = stringList[0]
    returnDict["table_name"] = stringList[1]
    returnDict["data_name"]  = stringList[2]
    returnDict["data"] = stringToList(stringList[3])
    returnDict["mac_address"] = stringList[4]
    returnDict["start_timestamp"] = float(stringList[5])
    returnDict["interval"] = float(stringList[6])
    
    
    return returnDict
         
        
# New influx
def write_influx(influx):
    
    # put into config file soon
    influxConfig = {}
    influxConfig["influx_ip"] = 'https://sensorweb.us'
    influxConfig["influx_user"] = 'test'
    influxConfig["influx_pass"] =  'sensorweb'
    
    start_timestamp = influx['start_timestamp']
    
    ## function to parse data
    http_post  = "curl -i -k -XPOST \'"+ influxConfig['influx_ip']+":8086/write?db="+influx['db_name']+"\' -u "+ influxConfig['influx_user']+":"+ influxConfig['influx_pass']+" --data-binary \' "
    count = 0
    dataLength = len(influx['data'])
    for value in influx['data']:
        count += 1
        http_post += "\n" + influx['table_name'] +",location=" + influx['mac_address'] + " "
        http_post += influx['data_name'] + "=" + str(value) + " " + str(int(start_timestamp*10e8))
        start_timestamp +=  influx['interval']
            
    http_post += "\'  &"
    print(http_post)
    subprocess.call(http_post, shell=True)
    print("printed to iflux!")



##
##  ExampleString Functions
##

def getEpochTime():
    return round(time.time(), 3)

def createSineWave(influx_packet,influx_frequency):
    
    maxTime = influx_packet * influx_frequency
    DataValues = 10 * np.sin(2 * np.pi * np.linspace(0, maxTime, influx_packet) / 1)
    DataValues = np.round(DataValues, decimals = 2)
    
    return DataValues

def arrayToString(data):
    returnDataString = ""
    for i in range(len(data) - 2):
        returnDataString += str(data[i]) + ","
    
    returnDataString += str(data[len(data)-1])
    return returnDataString

def getString():
    returnString = "shake testData value "
    returnString += arrayToString(createSineWave(200,.01))
    returnString += " 00.00.00.00.00 "
    returnString += str(getEpochTime())
    returnString += " .01"
    
    return(returnString)







    