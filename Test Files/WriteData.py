
import subprocess
import datetime
import numpy as np
import time

#############################################################################################################
###                                         Example Strings                                               ###
#############################################################################################################

#                                          Example String 
                                        ###################
                                        
# <Tablename>  <Dataname> <Data> <StartTime> <EndTime> <MacAddress>
# "M5 accX 1,2,3,4,5,6,7 1689732152.000  1689732152.000 00:00:00:00:00:00"

                                       
#                                            Curl String 
                                         ###################
                                        
# "curl -k -POST 'https://sensorweb.us:8086/write?db=shake' -u test:sensorweb --data-binary ' Z,location=00:00:00:00:00:00 value=999 1689731462967998976'"

#############################################################################################################
###                                          Documentation                                                ###
#############################################################################################################

# parsedValues[0] = Table Name
# parsedValues[1] = Data Name
# parsedValues[2] = Data
# parsedValues[3] = Start Time
# parsedValues[5] = End Time
# parsedValues[4] = Mac Address


#############################################################################################################
###                                         Helper Functions                                              ###
#############################################################################################################

# Function to parse the string
def parseString(inputString):
    valuesInTheString = inputString.split(" ")
    return valuesInTheString

# Converts scientific notation to a full number form
def convert_scientific_to_full(number):
    full_form = format(number, 'f').rstrip('0').rstrip('.')
    return full_form
    
# Function to return the data array form the string of data points
def returnDataArray(stringData):
    dataArray = stringData.split(",")
    return dataArray

# Function to create the an array of timestamps
def createTimeArray(startTime, endTime, dataArray):
    dataArrayLength = len(dataArray)
    
    largeStartTime = (float(startTime))*10e8
    largeEndTime = (float(endTime))*10e8
    
    spaced_array = np.linspace(largeStartTime, largeEndTime, num=dataArrayLength)
    return spaced_array




#############################################################################################################
###                            Create String and Send String Functions                                    ###
#############################################################################################################

# Function to create the Curl String
def createString(parsedValues, dataPoint, timestamp):
    stringTimeStamp = convert_scientific_to_full(timestamp)
    http_post = "curl -k -POST 'https://sensorweb.us:8086/write?db=shake' -u test:sensorweb --data-binary ' "
    http_post += parsedValues[1] + ",location=" + parsedValues[5].lower() + " value=" + str(dataPoint) + " " + stringTimeStamp + "' &"
    return http_post
    
# Send a curl request to the influxDB
def sendToInflux(inputString):
    # print("Sending Data to InfluxDB with curl command: " + inputString)
    subprocess.call(inputString, shell=True)
    # print("Data sent to InfluxDB")
        
        
        
    
#############################################################################################################
###                                         Testing Functions                                             ###
#############################################################################################################
def completeCurl(inputString):  
    print("Entered the Curl Function at: " + str(datetime.datetime.now()))
# Given an input String, this Function will send the data to the influxDB via a Curl command. one data point at a time
# #Parse the input into a string array
    parsedValues = parseString(inputString)
    
    #Create the data array
    dataArray = returnDataArray(parsedValues[2])
    
    #Create timeStamp Data
    timeArray = createTimeArray(parsedValues[3], parsedValues[4], dataArray)
    
    # print((dataArray))
    
    # Send the data to the influxDB
    for i in range(len(dataArray)):
        httpString = createString(parsedValues, dataArray[i], timeArray[i])
        sendToInflux(httpString)
    #print("Fake sending Data")
    print("Leaving the Curl Function at: " + str(datetime.datetime.now()))


def Test1():
    inputString = "M5 accX 1,2,3,4,5,6,7 1689732152.000 1689732152.000 00:00:00:00:00:00"
    completeCurl(inputString)

if __name__ == '__main__': 
    Test1()