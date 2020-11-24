import SimulatedGenericI2cController
from numpy import *
from datetime import timedelta
from datetime import datetime

###
### Simplified simulation of: Everlight ALS-PDIC17-57B/TR8 digital ambient light sensor
###

measuredValues = [9008, 9010, 9009, 9983, 10511, 10512, 10105, 4598, 3015, 2301, 2305, 2302, 2297, 2299]

# Example with extreme values:
# measuredValues = [9008, 9010, 9009, 9983, 10511, 13315, 25459, 27815, 27719, 27718, 60356, 81512, 81511, 81513]

detectionRanges = [
    # (IntegrationTime, MaximumMeasuredValue, LuxPerOneCount)
    # See table 3 on page 15 in ALS-PDIC17-57B datasheet
    (75, 88400, 2.7),   # Target integration time: 50
    (150, 44200, 1.35), # Target integration time: 100
    (250, 22200, 0.68), # Target integration time: 200
    (350, 14500, 0.45), # Target integration time: 300
    (400, 11100, 0.34)  # Target integration time: 400
]

class DeviceEverlightALS_PDIC17_57B:
    def __init__(self):
        self.isActive = False   # Start in shutdown mode
        self.integrationInProgress = False
        self.dataRegister = 0

    def start(self, slaveAddr, isRead):
        if (slaveAddr != 0x29):
            return False

        return True

    def write(self, dataBytes):
        global measuredValues, detectionRanges

        if SimulatedGenericI2cController.enableLog: print(">>> SIMULATION LOG > device ALS-PDIC17-57B > I2C write:", dataBytes)
        if dataBytes[0] & 0x80 != 0:    # Shutdown command
            self.isActive = False
            return 1

        if dataBytes[0] == 0x04:        # Activate command
            self.isActive = True
            return 1

        if not self.isActive:
            if SimulatedGenericI2cController.enableLog: print(">>> SIMULATION LOG > device ALS-PDIC17-57B > Device is shutdown, IGNORING THIS COMMAND ...")
            return 1

        if dataBytes[0] == 0x08:        # Start integration
            self.dataRegister = uint16(0)
            self.integrationInProgress = True
            self.integrationStartTime = datetime.now()
            return 1

        if dataBytes[0] == 0x30:        # Stop integration
            self.integrationInProgress = False
            endTime = datetime.now()

            delta = endTime - self.integrationStartTime
            ms = delta.total_seconds() * 1000
            for detectionRange in detectionRanges:  # Find detection range used in the measurement
                if ms < detectionRange[0]:
                    break

            value = measuredValues[0]   # Get current measurement value in lux
            if len(measuredValues) > 1:
                measuredValues = measuredValues[1:]

            if value > detectionRange[1]:
                value = detectionRange[1]

            count = round(value / detectionRange[2])    # Convert value in lux into normalized count
            self.dataRegister = uint16(count) | 0x8000  # Set data valid bit
            if SimulatedGenericI2cController.enableLog: print(">>> SIMULATION LOG > device ALS-PDIC17-57B > Integration time:", delta.total_seconds(), "s, Selected range:", detectionRange, ", Measured value:", value, "lux = count of:", count, "; dataRegister: 0x", format(self.dataRegister, "02X"))

            return 1

        return 1

    def read(self, bytesExpected):
        return [uint8(self.dataRegister), uint8(self.dataRegister >> 8)]
