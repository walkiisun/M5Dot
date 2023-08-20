# This function tests the new utils
from Utils import parseString, getString, write_influx
import numpy as np
import time
        
        
def main():
    while(True):
        exampleString = getString()  
        finalDict = parseString(exampleString)  
        print("main function") 
        write_influx(finalDict)
        time.sleep(2)
        

        
if __name__ == '__main__': 
    main()
    