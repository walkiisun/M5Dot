import yaml

# This function parses the config file and returns a list of values
def GetInfo():
    keys = []
    values = []
    with open("config.yaml", "r") as stream:
        dictOfKeyValues = yaml.safe_load(stream)
        for i in range(len(list(dictOfKeyValues))):        
            keys.append(list(dictOfKeyValues)[i]) # gets the topic
            values.append(list(dictOfKeyValues.values())[i]) #gets the values
            
    return values
