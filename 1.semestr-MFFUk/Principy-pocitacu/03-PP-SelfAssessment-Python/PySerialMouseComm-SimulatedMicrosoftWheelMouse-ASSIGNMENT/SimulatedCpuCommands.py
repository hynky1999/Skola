from datetime import datetime

dataRegister = 0
statusRegister = 0

startTime = datetime.now()
inputEvents = []

def setSimulatedInputEvents(simulatedInputEvents):
    global inputEvents
    inputEvents = simulatedInputEvents

def _tryReceiveData():
    global inputEvents, statusRegister, dataRegister

    if len(inputEvents) > 0:
        if statusRegister == 0:     # Simulate input buffer, so no data bytes are lost
            nextEvent = inputEvents[0]
            currentDelta = datetime.now() - startTime
            if nextEvent[0] < currentDelta:
                inputEvents.pop(0)
                dataRegister = nextEvent[1]
                statusRegister = 1

def readDataRegister(controllerId):
    global statusRegister, dataRegister

    _tryReceiveData()
    
    statusRegister = 0
    return dataRegister

def readStatusRegister(controllerId):
    global statusRegister

    _tryReceiveData()
    
    return statusRegister


