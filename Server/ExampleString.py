from Utils import parseDemoData, createSineWave, getEpochTime, convertDataToString

# def createExampleString(influx):
#     returnString = influx[]
#     returnString += 

def initInflux():
    # get influx with all values but emoch time and data values
    influx = parseDemoData()
    
    # get sine wave from influx parameters
    data = createSineWave(influx['influx_packet'],influx['influx_frequency'])
    
    # Covert the data to a string
    dataString = convertDataToString(data)
    influx["influx_data"] = dataString
    
    # get epoch times
    startTime = getEpochTime()
    influx["influx_startTime"] = startTime
    
    return influx

    
def convertToString(influx):
    returnString = str(influx["mqtt_ip"]) + " "
    returnString += str(influx["mqtt_port"]) + " "
    returnString += influx["influx_ip"] + " "
    returnString += influx["influx_user"] + " "
    returnString += influx["influx_pass"] + " "
    returnString += influx["influx_db"] + " "
    returnString += influx["influx_table"] + " "
    returnString += influx["influx_data"] + " "
    returnString += str(influx["influx_frequency"]) + " "
    returnString += str(influx["influx_startTime"]) + " "
    returnString += str(influx["influx_packet"]) + " "
    returnString += str(influx["influx_macAddress"]) 
    return returnString

def getString():
    influx = initInflux()
    return convertToString(influx)


if __name__ == '__main__': 
    print(getString())
    

    